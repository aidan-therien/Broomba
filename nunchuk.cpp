#include "mbed.h"
#include "PinNames.h"

InterruptIn left(p4);
InterruptIn leftup(p28);
InterruptIn up(p29);
InterruptIn rightup(p30);
InterruptIn right(p31);

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
DigitalOut led5(p3);
DigitalOut out(p27);

bool stop;
void left_rise();
void left_fall();

void leftup_rise();
void leftup_fall();

void up_rise();
void up_fall();

void rightup_rise();
void rightup_fall();

void right_rise();
void right_fall();

int main()
{
    // led5 = 1;
    led1 = 1;
    led2 = 1;
    led3 = 1;
    led4 = 1;
    stop = false;
    // left.mode(PullDown);
    left.rise(&left_rise);
    left.fall(&left_fall);

    // // leftup.mode(PullDown);
    // leftup.rise(&leftup_rise);
    // leftup.fall(&leftup_fall);

    // // up.mode(PullDown);
    // up.rise(&up_rise);
    // up.fall(&up_fall);

    // // rightup.mode(PullDown);
    // rightup.rise(&rightup_rise);
    // rightup.fall(&rightup_fall);

    // // rightup.mode(PullDown);
    // right.rise(&right_rise);
    // right.fall(&right_fall);
    while (true) {
        // sleep();
        if (stop){
            out = 1;
            wait_us(500);
            out = 0;
            stop = 0;
        }
    }
}

void left_rise(){
    stop = 1;
    led1 = 1;
}

void left_fall(){
    led1 = 0;
    stop = 1;
}

void leftup_rise(){
    led2 = 1;
}

void leftup_fall(){
    led2 = 0;
}

void up_rise(){
    led3 = 1;
}

void up_fall(){
    led3 = 0;
}

void rightup_rise(){
    led4 = 1;
}

void rightup_fall(){
    led4 = 0;
}

void right_rise(){
    // led5 = 1;
}

void right_fall(){
    // led5 = 0;
}
