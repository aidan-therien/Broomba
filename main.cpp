#include "mbed.h"
#include "nunchuk.h"

// I2C i2c(I2C_SDA, I2C_SCL);
// main() runs in its own thread in the OS
int main()
{
    // i2c.frequency(400000);
    printf("starting\n\n");
    if(!nunchuk_init()){
        printf("Init failed\n");
        while(1);
    }
    while (true) {
        if (!nunchuk_read()){
            printf("read failed\n");
        }
        else{
            nunchuk_print();
        }
        wait_us(500000);
    }
}

