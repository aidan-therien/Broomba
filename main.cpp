#include "mbed.h"
#include "PinNames.h"
#include <arm_compat.h>




// main() runs in its own thread in the OS
Timer t1;
Timer t2;
InterruptIn left(p24);
InterruptIn right(p23);
InterruptIn control(p25);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);
DigitalOut leftH(p20);
DigitalOut rightH(p19);
PwmOut servo(p21);
PwmOut servo2(p22);
// DigitalOut tservo(p22);
void left_rise_int();
// void left_fall_int();
void right_rise_int();
// int servoon;
void stop_go();
float duty1;
float duty2;
float time1;
float time2;
bool go;
bool turn;
int main()
{
    // left.mode(PullDown);
    // left.disable_irq();
    
    // left.fall(&left_fall_int);
    // servo.suspend();
    // servo.period_ms(20);
    // servo.write(0.1);
    // servo.resume();
    // servoon = 1;
    // left.enable_irq();
    // tservo = 1;
    printf("\n\nstart\n");
    go = true;
    turn = false;
    control.rise(&stop_go);
    time1 = 0;
    time2 = 0;
    duty1 = 0.05f;
    // duty2 = 0.9f;
    leftH = 1;
    rightH = 0;
    // servo = 1;
    // wait_us(5);
    servo.period(0.1f);
    // servo2.period(0.1f);
    servo.write(duty1);
    // servo2.write(duty2);
    wait_us(200000);
    t1.start();
    t2.start();
    
    left.rise(&left_rise_int);
    right.rise(&right_rise_int);
    while (true) {
        // led1 = left;
        // sleep();
        if (go){
            if (chrono::duration_cast<std::chrono::milliseconds>(t1.elapsed_time()).count() > 300){
                servo = 1;
                wait_us(5);
                servo.write(duty1);
            }
            // if (chrono::duration_cast<std::chrono::milliseconds>(t2.elapsed_time()).count() > 300){
            //     servo2.write(duty2);
            // }
            printf("%d\n", int(duty1*100));
            servo.write(duty1);
        }
        if (turn){
            leftH = 0;
            rightH = 1;
        }
        
        ThisThread::sleep_for(chrono::milliseconds(50));
    }
}

void left_rise_int(){
    t1.stop();
    // led1 = 1;
    if (!go){
        // servo.write(0);
        // servo2.write(0);
        t1.reset();
        t1.start();
        return;
    }
    time1 = chrono::duration<float>(t1.elapsed_time()).count();
    if (time2 == 0){
        // led3 = 1;
        t1.reset();
        t1.start();
        return;
    }
    if (time1 > time2){
        duty1 = duty1 - 0.01;
        // duty2 = duty2 - 0.01;
        // led1 = 1;
        // led2 = 0;
    }
    else{
        duty1 = duty1 + 0.01;
        // duty2 = duty2 + 0.01;
        // led1 = 0;
        // led2 = 1;
    }
    // if (duty1 < 0.5f || duty2 < 0.5f){
    //     duty1 = 0.9f;
    //     duty2 = 0.9f;
    // }
    // servo.write(duty1);
    // servo2.write(duty2);
    t1.reset();
    t1.start();
}

void right_rise_int(){
    t2.stop();
    if (!go){
        // servo.write(0);
        // servo2.write(0);
        t2.reset();
        t2.start();
        return;
    }
    // led2 = 1;
    time2 = chrono::duration<float>(t2.elapsed_time()).count();
    if (time1 == 0){
        // led4 = 1;
        t2.reset();
        t2.start();
        return;
    }
    if (time1 > time2){
        duty1 = duty1 - 0.01;
        // duty2 = duty2 - 0.01;
        // led1 = 1;
        // led2 = 0;
    }
    else{
        duty1 = duty1 + 0.01;
        // duty2 = duty2 + 0.01;
        // led1 = 0;
        // led2 = 1;
    }
    // if (duty1 < 0.5f || duty2 < 0.5f){
    //     duty1 = 0.9f;
    //     duty2 = 0.9f;
    // }
    // servo.write(duty1);
    // servo2.write(duty2);
    t2.reset();
    t2.start();
}

void stop_go(){
    __disable_irq();
    go = !go;
    if (go){
        // servo.write(duty1);
        // servo2.write(duty2);
    }
    else{
        servo.write(0);
        // servo2.write(0);
    }
    __enable_irq();
}