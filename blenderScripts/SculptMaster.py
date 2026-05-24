import bpy
import asyncio
import struct
import threading
import time

from mathutils import Quaternion, Vector
import pyautogui

from bleak import BleakClient, BleakScanner

DEVICE_NAME = "Glove"
CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

FORMAT = "<6f4B"
PACKET_SIZE = struct.calcsize(FORMAT)

glove_state = {
    "linear_accel": [0.0, 0.0, 0.0],
    "angular_vel": [0.0, 0.0, 0.0],
    "buttons": [False, False, False, False],
}

glove_lock = threading.Lock()

running = False
camera_rot = Quaternion((1, 0, 0, 0))
last_time = time.time()
last_click = False

deadzone = 0.05
mouse_sensitivity = 3.0
move_speed = 0.2
rot_speed = 1.0

# Moyenne à decay exponentiel. Alpha varie entre 0 et 1. 1 Priorise les nouvelles valeurs et 0 priorise les vielles
class MovingAverage:
    def __init__(self, alpha):
        self.alpha = alpha
        self.moving_average = 0
    
    def push_val(self, val):
        self.moving_average = self.alpha * val + (1 - self.alpha) * self.moving_average

    def get(self):
        return self.moving_average


def handle_data(sender, data):
    if len(data) != PACKET_SIZE:
        return

    try:
        unpacked = struct.unpack(FORMAT, data)

        with glove_lock:
            glove_state["linear_accel"] = list(unpacked[0:3])
            glove_state["angular_vel"] = list(unpacked[3:6])
            glove_state["buttons"] = [
                bool(unpacked[6]),
                bool(unpacked[7]),
                bool(unpacked[8]),
                bool(unpacked[9]),
            ]
    except Exception as e:
        print("BLE parse error:", e)


async def ble_loop():
    while True:
        try:
            print("Searching for glove...")

            device = await BleakScanner.find_device_by_name(
                DEVICE_NAME,
                timeout=10.0
            )

            if not device:
                print("Glove not found.")
                await asyncio.sleep(2)
                continue

            print("Found:", device.address)

            disconnected = asyncio.Event()

            def on_disconnect(client):
                print("Disconnected.")
                disconnected.set()

            async with BleakClient(
                device.address,
                disconnected_callback=on_disconnect
            ) as client:

                await client.start_notify(CHAR_UUID, handle_data)
                await disconnected.wait()

        except Exception as e:
            print("BLE error:", e)
            await asyncio.sleep(2)


def start_ble_thread():
    def runner():
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        loop.run_until_complete(ble_loop())

    t = threading.Thread(target=runner, daemon=True)
    t.start()
    return t

def get_view3d():
    for area in bpy.context.window.screen.areas:
        if area.type == 'VIEW_3D':
            return area.spaces[0].region_3d
    return None


def angular_velocity_to_quat(av, dt):
    v = Vector(av)
    if v.length < 1e-8:
        return Quaternion((1, 0, 0, 0))

    axis = v.normalized()
    angle = v.length * dt
    return Quaternion(axis, angle)


def apply_glove():
    global camera_rot, last_time, last_click

    rv3d = get_view3d()
    if not rv3d:
        return

    dt = time.time() - last_time
    last_time = time.time()

    with glove_lock:
        g = {
            "linear_accel": glove_state["linear_accel"][:],
            "angular_vel": glove_state["angular_vel"][:],
            "buttons": glove_state["buttons"][:],
        }

    ax, ay, az = g["linear_accel"]
    wx, wy, wz = g["angular_vel"]
    g["buttons"][0] = True

    if g["buttons"][0]:
        forward = rv3d.view_rotation @ Vector((0, 0, -1))
        right   = rv3d.view_rotation @ Vector((1, 0, 0))
        up = rv3d.view_rotation @ Vector((0, 1, 0))

        move = (wx * forward) + (-wy * right) + (-wz * up)

        rv3d.view_location += move * move_speed

    if g["buttons"][1]:
        scaled_w = Vector((wx * rot_speed, 
                    wy * rot_speed, 
                    wz * rot_speed))
        local_w = rv3d.view_rotation @ scaled_w
        delta = angular_velocity_to_quat(local_w, dt)
        rv3d.view_rotation = delta.inverted() @ rv3d.view_rotation

    if g["buttons"][2]:
        dx = ax if abs(ax) > deadzone else 0
        dy = ay if abs(ay) > deadzone else 0
        pyautogui.moveRel(dx * mouse_sensitivity, dy * mouse_sensitivity)

    if g["buttons"][3]:
        if not last_click:
            try:
                bpy.ops.view3d.select(location=(0, 0))
            except:
                pass

    last_click = g["buttons"][3]


def update():
    if running:
        apply_glove()
    return 0.005



class VIEW3D_OT_start(bpy.types.Operator):
    bl_idname = "view3d.glove_start"
    bl_label = "Start Glove"

    def execute(self, context):
        global running
        running = True
        if not bpy.app.timers.is_registered(update):
            bpy.app.timers.register(update)
        return {'FINISHED'}


class VIEW3D_OT_stop(bpy.types.Operator):
    bl_idname = "view3d.glove_stop"
    bl_label = "Stop Glove"

    def execute(self, context):
        global running
        running = False
        return {'FINISHED'}


class VIEW3D_PT_panel(bpy.types.Panel):
    bl_label = "Glove Control"
    bl_idname = "VIEW3D_PT_glove"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Glove"

    def draw(self, context):
        layout = self.layout
        layout.operator("view3d.glove_start")
        layout.operator("view3d.glove_stop")


def register():
    bpy.utils.register_class(VIEW3D_OT_start)
    bpy.utils.register_class(VIEW3D_OT_stop)
    bpy.utils.register_class(VIEW3D_PT_panel)

    start_ble_thread()


def unregister():
    bpy.utils.unregister_class(VIEW3D_OT_start)
    bpy.utils.unregister_class(VIEW3D_OT_stop)
    bpy.utils.unregister_class(VIEW3D_PT_panel)


if __name__ == "__main__":
    register()