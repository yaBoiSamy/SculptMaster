import asyncio
import struct
import threading
from bleak import BleakClient, BleakScanner

DEVICE_NAME = "Glove"
CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

FORMAT = "<6f4B"
PACKET_SIZE = struct.calcsize(FORMAT)

glove_state = {
    "linear_accel": [0.0, 0.0, 0.0],
    "angular_vel": [0.0, 0.0, 0.0],
    "buttons": [False, False, False, False],
    "connected": False,
}

glove_lock = threading.Lock()

def handle_data(sender, data):
    try:
        if len(data) != PACKET_SIZE:
            return

        unpacked = struct.unpack(FORMAT, data)

        with glove_lock:
            glove_state["linear_accel"] = [
                unpacked[0],
                unpacked[1],
                unpacked[2]
            ]

            glove_state["angular_vel"] = [
                unpacked[3],
                unpacked[4],
                unpacked[5]
            ]

            glove_state["buttons"] = [
                bool(unpacked[6]),
                bool(unpacked[7]),
                bool(unpacked[8]),
                bool(unpacked[9]),
            ]

    except Exception as e:
        print("Parse error:", e)

async def run():
    while True:
        try:
            print("Searching for glove...")

            device = await BleakScanner.find_device_by_name(
                DEVICE_NAME,
                timeout=10.0
            )

            if device is None:
                print("Glove not found.")
                await asyncio.sleep(2.0)
                continue

            print(f"Found glove: {device.address}")

            disconnected_event = asyncio.Event()

            def disconnected_callback(client):
                print("Glove disconnected.")

                with glove_lock:
                    glove_state["connected"] = False

                disconnected_event.set()

            async with BleakClient(
                device.address,
                disconnected_callback=disconnected_callback,
                timeout=20.0
            ) as client:

                print("Connected to glove!")

                with glove_lock:
                    glove_state["connected"] = True

                await client.start_notify(
                    CHAR_UUID,
                    handle_data
                )

                print("Receiving data...")

                await disconnected_event.wait()

        except Exception as e:
            print("BLE error:", e)

            with glove_lock:
                glove_state["connected"] = False

            await asyncio.sleep(2.0)

def start_ble_thread():
    def runner():
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)

        try:
            loop.run_until_complete(run())

        finally:
            loop.close()

    thread = threading.Thread(
        target=runner,
        daemon=True
    )

    thread.start()

    return thread

start_ble_thread()