/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdint>
#include <events/mbed_events.h>
#include "InterruptIn.h"
#include "PinNames.h"
#include "ble/BLE.h"
#include "gatt_client_process.h"

static EventQueue event_queue(/* event count */ 10 * EVENTS_EVENT_SIZE);

InterruptIn left(BUTTON4);
InterruptIn right(BUTTON2);
InterruptIn up(BUTTON1);
InterruptIn down(BUTTON3);
uint8_t value;

const static uint16_t EXAMPLE_SERVICE_UUID         = 0xA000;
const static uint16_t WRITABLE_CHARACTERISTIC_UUID = 0xA001;

static DiscoveredCharacteristic writable_characteristic;
static bool writable_characteristic_found = false;

void service_discovery(const DiscoveredService *service) {
    if (service->getUUID().shortOrLong() == UUID::UUID_TYPE_SHORT) {
        if (service->getUUID().getShortUUID() == EXAMPLE_SERVICE_UUID) {
            printf("We found the service we were looking for\r\n");
        }
    }
}

void characteristic_discovery(const DiscoveredCharacteristic *characteristic) {
    if (characteristic->getUUID().getShortUUID() == WRITABLE_CHARACTERISTIC_UUID) {
        printf("We found the characteristic we were looking for\r\n");
        writable_characteristic = *characteristic;
        writable_characteristic_found = true;
    }
}

void discovery_termination(ble::connection_handle_t connectionHandle) {
    if (writable_characteristic_found) {
        writable_characteristic_found = false;
        event_queue.call([]{ writable_characteristic.read(); });
    }
}

void on_read(const GattReadCallbackParams *response) {
    if (response->handle == writable_characteristic.getValueHandle()) {
        /* increment the value we just read */
        // uint8_t value = response->data[0];
        // value++;

        /* and write it back */
        writable_characteristic.write(1, &value);

        printf("Written new value of %x\r\n", value);
    }
}

void on_write(const GattWriteCallbackParams *response) {
    if (response->handle == writable_characteristic.getValueHandle()) {
        event_queue.call_in(1000ms, []{ writable_characteristic.read(); });
    }
}

class GattClientDemo {
public:
    GattClientDemo() { }
    ~GattClientDemo() { }

    /** Callback triggered when the ble initialization process has finished */
    void start(BLE &ble, events::EventQueue &event_queue) {
        ble.gattClient().onDataRead(on_read);
        ble.gattClient().onDataWritten(on_write);
    }

    void start_discovery(BLE &ble, events::EventQueue &event_queue, const ble::ConnectionCompleteEvent &event) {
        printf("We are looking for a service with UUID 0xA000\r\n");
        printf("And a characteristic with UUID 0xA001\r\n");

        ble.gattClient().onServiceDiscoveryTermination(discovery_termination);
        ble.gattClient().launchServiceDiscovery(
            event.getConnectionHandle(),
            service_discovery,
            characteristic_discovery,
            EXAMPLE_SERVICE_UUID,
            WRITABLE_CHARACTERISTIC_UUID
        );
    }

};

void send_left(){
    value = 3;
}
void send_right(){
    value = 1;
}
void send_up(){
    value = 0;
}
void send_down(){
    value = 2;
}

int main()
{
    value = -1;
    left.rise(&send_left);
    right.rise(&send_right);
    up.rise(&send_up);
    down.rise(&send_down);

    BLE &ble = BLE::Instance();

    printf("\r\nGattClient demo of a writable characteristic\r\n");

    GattClientDemo demo;

    /* this process will handle basic setup and advertising for us */
    GattClientProcess ble_process(event_queue, ble);

    /* once it's done it will let us continue with our demo*/
    ble_process.on_init(callback(&demo, &GattClientDemo::start));
    ble_process.on_connect(callback(&demo, &GattClientDemo::start_discovery));

    ble_process.start();

    return 0;
}


