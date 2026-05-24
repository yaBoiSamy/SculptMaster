#ifndef BLEGLOVE_H
#define BLEGLOVE_H

#include <NimBLEDevice.h>
#include "PCB.h"

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

typedef struct __attribute__((packed))
{
    float acc[3];
    float gyro[3];
    uint8_t buttons[4];
} GloveState;

class BLEGlove {
public:
    NimBLECharacteristic* pCharacteristic;
    bool deviceConnected = false;
    GloveState state;

    class ServerCallbacks : public NimBLEServerCallbacks {
        BLEGlove* bleGlove;
    public:
        ServerCallbacks(BLEGlove* glove) : bleGlove(glove) {}
        void onConnect(NimBLEServer* pServer) override {
            bleGlove->deviceConnected = true;
        }
        void onDisconnect(NimBLEServer* pServer) override {
            bleGlove->deviceConnected = false;
            NimBLEDevice::startAdvertising();
        }
    };

    void setup() {
        NimBLEDevice::init("Glove");
        NimBLEServer* server = NimBLEDevice::createServer();
        server->setCallbacks(new ServerCallbacks(this));
        NimBLEService* service = server->createService(SERVICE_UUID);

        pCharacteristic = service->createCharacteristic(
            CHARACTERISTIC_UUID,
            NIMBLE_PROPERTY::NOTIFY
        );
        service->start();

        NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
        advertising->addServiceUUID(SERVICE_UUID);
        advertising->start();
    }

    void update(PCB& pcb) {
        if (!deviceConnected) return;

        Vect acc = pcb.get_acc();
        state.acc[0] = acc.x;
        state.acc[1] = acc.y;
        state.acc[2] = acc.z;

        Vect gyro = pcb.get_ang();
        state.gyro[0] = gyro.x;
        state.gyro[1] = gyro.y;
        state.gyro[2] = gyro.z;

        state.buttons[0] = 0; state.buttons[1] = 0; 
        state.buttons[2] = 0; state.buttons[3] = 0;

        pCharacteristic->setValue((uint8_t*)&state, sizeof(state));
        pCharacteristic->notify();
    }
};

#endif