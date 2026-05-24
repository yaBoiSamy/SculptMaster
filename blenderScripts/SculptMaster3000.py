import bpy
from mathutils import Quaternion, Vector
import pyautogui
import time
import sys
import os
import threading

classes_to_clean = ["GloveSimulationProperties", "VIEW3D_OT_start_motion", "VIEW3D_OT_stop_motion", "VIEW3D_PT_motion_panel"]
for cls_name in classes_to_clean:
    if hasattr(bpy.types, cls_name):
        try:
            bpy.utils.unregister_class(getattr(bpy.types, cls_name))
        except Exception:
            pass
if hasattr(bpy.types.Scene, "glove_sim"):
    del bpy.types.Scene.glove_sim

BASE_DIR = os.path.dirname(bpy.data.filepath)
if not BASE_DIR:
    BASE_DIR = os.path.dirname(__file__)
if BASE_DIR not in sys.path:
    sys.path.append(BASE_DIR)

try:
    import SculptMaster.blenderScripts.BleReceiver as ble
except ImportError:
    class MockBle:
        glove_lock = threading.Lock() if 'threading' in sys.modules else None
        glove_state = {"linear_accel": (0,0,0), "angular_vel": (0,0,0), "buttons": [False]*4}
    ble = MockBle()

running = False
last_click = False
camera_rot = Quaternion((1, 0, 0, 0))
last_time = time.time()

deadzone = 0.05
mouse_sensitivity = 3.0
move_speed_multiplier = 0.2
rotation_speed_multiplier = 0.2

def get_view3d():
    for area in bpy.context.window.screen.areas:
        if area.type == 'VIEW_3D':
            for space in area.spaces:
                if space.type == 'VIEW_3D':
                    return space.region_3d
    return None

class GloveSimulationProperties(bpy.types.PropertyGroup):
    use_sim: bpy.props.BoolProperty(name="Mode Simulation", default=False) # type: ignore
    btn_0: bpy.props.BoolProperty(name="Bouton 0 (Déplacement)", default=False) # type: ignore
    btn_1: bpy.props.BoolProperty(name="Bouton 1 (Rotation)", default=False) # type: ignore
    btn_2: bpy.props.BoolProperty(name="Bouton 2 (Souris Mvmt)", default=False) # type: ignore
    btn_3: bpy.props.BoolProperty(name="Bouton 3 (Clic)", default=False) # type: ignore
    sim_x: bpy.props.FloatProperty(name="Axe X", default=0.0, min=-5.0, max=5.0) # type: ignore
    sim_y: bpy.props.FloatProperty(name="Axe Y", default=0.0, min=-5.0, max=5.0) # type: ignore
    sim_z: bpy.props.FloatProperty(name="Axe Z", default=0.0, min=-5.0, max=5.0) # type: ignore

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
    
    now = time.time()
    dt = now - last_time
    last_time = now

    sim_data = bpy.context.scene.glove_sim

    if sim_data.use_sim:
        g = {
            "linear_accel": (sim_data.sim_x, sim_data.sim_y, sim_data.sim_z),
            "angular_vel": (sim_data.sim_x, sim_data.sim_y, sim_data.sim_z),
            "buttons": [sim_data.btn_0, sim_data.btn_1, sim_data.btn_2, sim_data.btn_3]
        }
    else:
        with ble.glove_lock:
            g = {
                "linear_accel": ble.glove_state["linear_accel"][:],
                "angular_vel": ble.glove_state["angular_vel"][:],
                "buttons": ble.glove_state["buttons"][:],
                "connected": ble.glove_state["connected"]
            }

    ax, ay, az = g["linear_accel"]
    wx, wy, wz = g["angular_vel"]

    if g["buttons"][0]:
        rv3d.view_location.x += ax * move_speed_multiplier
        rv3d.view_location.y += ay * move_speed_multiplier
        rv3d.view_location.z += az * move_speed_multiplier

    if g["buttons"][1]:
        scaled_w = (wx * rotation_speed_multiplier, 
                    wy * rotation_speed_multiplier, 
                    wz * rotation_speed_multiplier)
        delta = angular_velocity_to_quat(scaled_w, dt)
        camera_rot = delta @ camera_rot
        camera_rot.normalize()
        rv3d.view_rotation = camera_rot.inverted()

    if g["buttons"][2]:
        dx = ax if abs(ax) > deadzone else 0.0
        dy = ay if abs(ay) > deadzone else 0.0
        pyautogui.moveRel(dx * mouse_sensitivity, dy * mouse_sensitivity)

    if g["buttons"][3]:
        if not last_click:
            try:
                bpy.ops.view3d.select(extend=False, deselect_all=False, location=(0,0))
            except Exception as e:
                print("Click simulation error:", e)
    last_click = g["buttons"][3]

def update():
    global running
    if not running:
        return None
    try:
        apply_glove()
    except Exception as e:
        print("Glove error:", e)
    return 0.1

class VIEW3D_OT_start_motion(bpy.types.Operator):
    bl_idname = "view3d.start_motion"
    bl_label = "Start View Motion"
    def execute(self, context):
        global running
        running = True
        if not bpy.app.timers.is_registered(update):
            bpy.app.timers.register(update)
        return {'FINISHED'}

class VIEW3D_OT_stop_motion(bpy.types.Operator):
    bl_idname = "view3d.stop_motion"
    bl_label = "Stop View Motion"
    def execute(self, context):
        global running
        running = False
        return {'FINISHED'}

class VIEW3D_PT_motion_panel(bpy.types.Panel):
    bl_label = "Glove Control"
    bl_idname = "VIEW3D_PT_motion_panel"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Glove"

    def draw(self, context):
        layout = self.layout
        
        if not hasattr(context.scene, "glove_sim"):
            layout.label(text="Error: Reload script", icon="ERROR")
            return
            
        sim_data = context.scene.glove_sim
        
        layout.operator("view3d.start_motion", icon="PLAY")
        layout.operator("view3d.stop_motion", icon="PAUSE")
        layout.separator()
        
        layout.prop(sim_data, "use_sim", text="Activer la Simulation", toggle=True)
        layout.separator()
        
        box = layout.box()
        box.label(text="1. Configurer les Axes :")
        box.prop(sim_data, "sim_x")
        box.prop(sim_data, "sim_y")
        box.prop(sim_data, "sim_z")
        
        box_btn = layout.box()
        box_btn.label(text="2. Activer une Action :")
        box_btn.prop(sim_data, "btn_0", toggle=True, text="Action Déplacement (Btn 0)")
        box_btn.prop(sim_data, "btn_1", toggle=True, text="Action Rotation (Btn 1)")
        box_btn.prop(sim_data, "btn_2", toggle=True, text="Action Souris (Btn 2)")
        box_btn.prop(sim_data, "btn_3", toggle=True, text="Action Clic (Btn 3)")

def register():
    bpy.utils.register_class(GloveSimulationProperties)
    bpy.types.Scene.glove_sim = bpy.props.PointerProperty(type=GloveSimulationProperties)
    bpy.utils.register_class(VIEW3D_OT_start_motion)
    bpy.utils.register_class(VIEW3D_OT_stop_motion)
    bpy.utils.register_class(VIEW3D_PT_motion_panel)

def unregister():
    bpy.utils.unregister_class(GloveSimulationProperties)
    if hasattr(bpy.types.Scene, "glove_sim"):
        del bpy.types.Scene.glove_sim
    bpy.utils.unregister_class(VIEW3D_OT_start_motion)
    bpy.utils.unregister_class(VIEW3D_OT_stop_motion)
    bpy.utils.unregister_class(VIEW3D_PT_motion_panel)

if __name__ == "__main__":
    register()