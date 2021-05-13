#include "mbed.h"
#include "esc.h"
#include <cstdio>

BufferedSerial pc(USBTX,USBRX,115200);
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

// Log variables
bool flag_log = true;
int trial;
float data[50][6];
int i = 0;

// Flash memmory
FlashIAP flash;
const uint32_t trial_size = sizeof(trial);
const uint32_t data_size = sizeof(data);
const uint32_t trial_addr = flash.get_flash_start()+flash.get_flash_size()-trial_size;
const uint32_t data_addr[] = {flash.get_flash_start()+flash.get_flash_size()-trial_size-data_size, flash.get_flash_start()+flash.get_flash_size()-trial_size-2*data_size};

int main()
{
    flash.init();
    flash.read(&trial, trial_addr, trial_size);
    flash.deinit();
    tic_esc.attach(&callback_esc, 200ms);
    tic_print.attach(&callback_print, 200ms);
    tic_blink.attach(&callback_blink, 500ms);
    while (true)
    {
        if(flag_esc)
        {
            flag_esc = false;
            esc.update();
            printf("%.4f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\n",esc.throttle,esc.pwm,esc.rpm,esc.voltage,esc.current,esc.temperature);
            if (flag_log)
            {
                data[i][0] = esc.throttle;
                data[i][1] = esc.pwm;
                data[i][2] = esc.rpm;
                data[i][3] = esc.voltage;
                data[i][4] = esc.current;
                data[i][5] = esc.temperature;
                i++;
                if (i>49)
                {
                    flag_log = false;
                    flash.init();
                    flash.erase(data_addr[trial], data_size);
                    flash.program(&data, data_addr[trial], data_size); 
                    trial++; 
                    flash.erase(trial_addr, trial_size);
                    flash.program(&trial, trial_addr, trial_size);
                    flash.deinit(); 
                }
                // printf("%d\n",trial);
            }
        }
        if(flag_print)
        {
            flag_print = false;  
            if(pc.readable())
            {
                pc.read(&input,1);
                printf("%.4f\t%.4f\t%.2f\t%.2f\t%.2f\t%.2f\n",esc.throttle,esc.pwm,esc.rpm,esc.voltage,esc.current,esc.temperature);
            }
        }
        if(pc.readable())
        {
            pc.read(&input,1);
            for (int t = 0; t<2; t++)
            {
                flash.init();
                flash.read(&data, data_addr[t], data_size);
                flash.deinit();
                printf("Trial %d\n",t);
                printf("THR(%%)\tPWM(%%)\tn(rpm)\tea(V)\tia(A)\tTc(C)\n");
                for(int r=0;r<50;r++)
                {
                    for (int c=0;c<6;c++)
                    {
                        printf("%.4f\t",data[r][c]);
                    }
                    printf("\n");
                }
                printf("\n");
            }
        } 
        if(flag_blink)
        {
            flag_blink = false;
            led = !led;
        }
    }
}
