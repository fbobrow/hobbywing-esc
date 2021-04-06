#ifndef esc_h
#define esc_h

#include "mbed.h"

#define POLES 14.0

#define V_REF 3.3  
#define V_DIV 21.0 
#define V_OFF 0.53 

#define ADC_RES 4096.0
#define PWM_RES 590.0

#define DIFFAMP_GAIN 10.0
#define DIFFAMP_SHUNT 0.25/1000.0

#define NTC_RS 10000.0
#define NTC_RF 47000.0
#define NTC_BETA 3950.0
#define NTC_TR 298.15

#define ALPHA 0.5
#define OMEGA_C 25 
#define DT 20E-3

class ESC
{
  public:
    ESC();
    void update();
    float throttle, pwm, rpm, voltage, current, temperature;
  private:
    BufferedSerial serial;
    char id;
    char data[18];
    float update_throttle(int);
    float update_pwm(int);
    float update_rpm(int);
    float update_voltage(int);
    float update_current(int);
    float update_temperature(int);
};

#endif