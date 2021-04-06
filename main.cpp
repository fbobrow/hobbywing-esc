#include "mbed.h"
#include "esc.h"

BufferedSerial pc(USBTX,USBRX);
DigitalOut led(LED1);
ESC esc;

Ticker tic_esc, tic_print, tic_blink; 

bool flag_esc = false;
bool flag_print = false;
bool flag_blink = false;
void callback_esc() { flag_esc = true; }
void callback_print() { flag_print = true; }
void callback_blink() { flag_blink = true; }

char input;

int main()
{
    tic_esc.attach(&callback_esc, 20ms);
    tic_print.attach(&callback_print, 200ms);
    tic_blink.attach(&callback_blink, 500ms);
    while (true)
    {
        if(flag_esc)
        {
            flag_esc = false;
            esc.update();
        }
        if(flag_print)
        {
            flag_print = false;  
            if(pc.readable())
            {
                pc.read(&input,1);
                // printf("%3.0f%% | ",esc.throttle*100);
                // printf("%3.0f%% | ",esc.pwm*100);
                // printf("%4.0frpm | ",esc.rpm); 
                // printf("%4.1fV | ",esc.voltage);
                // printf("%4.1fA | ",esc.current);
                // printf("%4.1fC ",esc.temperature);
                // printf("\n");
                printf("%.4f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\n",esc.throttle,esc.pwm,esc.rpm,esc.voltage,esc.current,esc.temperature);
            }
        }
        if(flag_blink)
        {
            flag_blink = false;
            led = !led;
        }
    }
}
