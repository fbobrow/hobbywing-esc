#include "esc.h"
#include "can_api.h"

ESC::ESC() : serial(D1,D0,19200)
{
    throttle = 0.0;
    pwm = 0.0;
    rpm = 0.0; 
    voltage = 0.0;
    current = 0.0;
    temperature = 0.0;
}

void ESC::update()
{
    if(serial.readable())
    {
        serial.read(&id,1);
        if(id == 0x9B)
        {
            serial.read(&data,18);
            int raw_throttle = data[3] << 8 | data[4];
            int raw_rpm = data[7] << 16 | data[8] << 8 | data[9];
            int raw_pwm = data[5] << 8 | data[6];
            int raw_voltage = data[10] << 8 | data[11];
            int raw_current = data[12] << 8 | data[13];
            int raw_temperature = data[14] << 8 | data[15];
            if (raw_throttle > 1024 || raw_pwm > 1024 || raw_rpm > 200000 || data[10] & 0xF0 || data[12] & 0xF0 || data[14] & 0xF0 || data[16] & 0xF0)
            {
            }
            else 
            {
                throttle += ALPHA*(update_throttle(raw_throttle)-throttle);
                rpm += ALPHA*(update_rpm(raw_rpm)-rpm);
                pwm += ALPHA*(update_pwm(raw_pwm)-pwm);
                voltage += ALPHA*(update_voltage(raw_voltage)-voltage);
                current += ALPHA*(update_current(raw_current)-current);
                temperature += ALPHA*(update_temperature(raw_temperature)-temperature);
            }
        }
    }
}

float ESC::update_throttle(int raw_throttle)
{
    return raw_throttle/PWM_RES;
}

float ESC::update_pwm(int raw_pwm)
{
    return raw_pwm/PWM_RES;
}    

float ESC::update_rpm(int raw_rpm)
{
    return raw_rpm*(2.0/POLES);
}  

float ESC::update_voltage(int raw_voltage)
{
    return raw_voltage*(V_REF/ADC_RES)*V_DIV;
}  

float ESC::update_current(int raw_current)
{
    float curr = raw_current*(V_REF/ADC_RES)/(DIFFAMP_GAIN*DIFFAMP_SHUNT)/sqrt(3.0);
    if (curr < 0)
        return 0;
    return curr;
} 

float ESC::update_temperature(int raw_temperature)
{
    float v_out = raw_temperature * (V_REF/ADC_RES);
    float rt = (v_out*NTC_RS/(V_REF-v_out));
    if (rt <= 0)
        return 0;
    float temp = 1/(log(rt/NTC_RF)/NTC_BETA+1/NTC_TR)-273.15;
    if (temp <= 0)
        return 0;
    return temp;
} 