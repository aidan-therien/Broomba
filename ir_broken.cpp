/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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

#include <events/mbed_events.h>
#include "ble/BLE.h"
#include "gatt_server_process.h"
#include "PinNames.h"

DigitalOut lservo(p24);
DigitalOut rservo(p25);
bool usr;



DigitalIn IR_left(p21);
DigitalIn IR_middle(p1);
DigitalIn IR_right(p0);
Ticker IR_sensors;

static EventQueue event_queue(/* event count */ 10 * EVENTS_EVENT_SIZE);

class GattServerDemo : ble::GattServer::EventHandler {

    const static uint16_t EXAMPLE_SERVICE_UUID         = 0xA000;
    const static uint16_t WRITABLE_CHARACTERISTIC_UUID = 0xA001;

public:
    GattServerDemo()
    {
        const UUID uuid = WRITABLE_CHARACTERISTIC_UUID;
        _writable_characteristic = new ReadWriteGattCharacteristic<uint8_t> (uuid, &_characteristic_value);

        if (!_writable_characteristic) {
            printf("Allocation of ReadWriteGattCharacteristic failed\r\n");
        }
    }

    ~GattServerDemo()
    {
    }



    void start(BLE &ble, events::EventQueue &event_queue)
    {
        const UUID uuid = EXAMPLE_SERVICE_UUID;
        GattCharacteristic* charTable[] = { _writable_characteristic };
        GattService example_service(uuid, charTable, 1);

        ble.gattServer().addService(example_service);

        ble.gattServer().setEventHandler(this);

        printf("Example service added with UUID 0xA000\r\n");
        printf("Connect and write to characteristic 0xA001\r\n");
    }

private:
    /**
     * This callback allows the LEDService to receive updates to the ledState Characteristic.
     *
     * @param[in] params Information about the characterisitc being updated.
     */
    virtual void onDataWritten(const GattWriteCallbackParams &params)
    {
        int value = *(params.data);
        printf("Value written: %d \n", value);
        if (value / 4 & 1) {
            usr = true;
        } else if (value / 8 & 1) {
            usr = false;
        }
        if (usr) {
            switch(value & 3) {
                case 0 : 
                    lservo = 0;
                    rservo = 0;
                    break;
                case 1 : 
                    lservo = 0;
                    rservo = 1;
                    break;
                case 2 : 
                    lservo = 1;
                    rservo = 0;
                    break;
                case 3 : 
                    lservo = 1;
                    rservo = 1;
                    break;
                default : 
                    lservo = 0;
                    rservo = 0;
                    break;
            }
        }

    }

private:
    ReadWriteGattCharacteristic<uint8_t> *_writable_characteristic = nullptr;
    uint8_t _characteristic_value = 0;
};

void IR_sensing(){
    if (usr) {
        return;
    }
        if(IR_left == 1 && IR_middle == 1 && IR_right == 1){
            lservo = 1;
            rservo = 1;
        }
        else if(IR_left == 1 && IR_middle == 1 && IR_right == 0){
            lservo = 0;
            rservo = 1;
        }
        else if(IR_left == 1 && IR_middle == 0 && IR_right == 1){
            lservo = 0;
            rservo = 1;
        }
        else if(IR_left == 1 && IR_middle == 0 && IR_right == 0){
            lservo = 0;
            rservo = 1;
        }
        else if(IR_left == 0 && IR_middle == 1 && IR_right == 1){
            lservo = 1;
            rservo = 0;
        }
        else if(IR_left == 0 && IR_middle == 1 && IR_right == 0){
            lservo = 1;
            rservo = 0;
        }
        else if(IR_left == 0 && IR_middle == 0 && IR_right == 1){
            lservo = 1;
            rservo = 0;
        }
        else if(IR_left == 0 && IR_middle == 0 && IR_right == 0){
            lservo = 0;
            rservo = 1;
        }
    }

int main()
{
    lservo = 0;
    rservo = 0;
    usr = false;
    IR_sensors.attach(&IR_sensing, 2ms);
    BLE &ble = BLE::Instance();

    printf("\r\nGattServer demo of a writable characteristic\r\n");

    GattServerDemo demo;

    /* this process will handle basic setup and advertising for us */
    GattServerProcess ble_process(event_queue, ble);

    /* once it's done it will let us continue with our demo*/
    ble_process.on_init(callback(&demo, &GattServerDemo::start));

    ble_process.start();

    return 0;
}
