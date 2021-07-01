#include "mbed.h"
#include "esc.h"
#include <cstdio>

BufferedSerial pc(USBTX,USBRX,115200);
DigitalOut led(LED1);
ESC esc;

Ticker tic_esc, tic_blink; 

bool flag_esc, flag_blink = false;
void callback_esc() { flag_esc = true; }
void callback_blink() { flag_blink = true; }

char input;

bool flag_print = false;
bool flag_thrust = false; 

int main()
{
    tic_esc.attach(&callback_esc, 200ms);
    tic_blink.attach(&callback_blink, 500ms);
    while (true)
    {
        if(flag_esc)
        {
            flag_esc = false;
            esc.update();
            if (flag_print) 
            {
                printf("%.4f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f",esc.throttle,esc.pwm,esc.rpm,esc.voltage,esc.current,esc.temperature);
                if (flag_thrust) 
                {
                    flag_thrust = false;
                    printf("\tT");
                }
                printf("\n");
            }
        }
        if(flag_blink)
        {
            flag_blink = false;
            led = !led;
        }
        if(pc.readable())
        {
            pc.read(&input,1);
            switch (input) 
            {
                case 't':
                    flag_thrust = true;
                    break;
                case 'p':
                    flag_print = !flag_print;
                    break;
            }
        } 
    }
}
