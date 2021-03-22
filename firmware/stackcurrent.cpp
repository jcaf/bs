#include <Arduino.h>
#include "stackcurrent.h"
#include "ltc6804.h"
#include "boot.h"

//MURATA DC SHUNT 3020-01105-0 1000A/50mV Resistance 0.00005 OHM


//#define DC_SHUNT_OUTPUT_mV_FULLSCALE 50E-3 //50mV
//#define DC_SHUNT_RATED_CURRENT 1000.0 //1000 Amps
//float DC_SHUNT_RESISTANCE = (setacq_vars[SHUNT_VOLT_FULLSCALE].value/setacq_vars[SHUNT_RATED_CURRENT].value);//5E-5

#define CSA_GAIN 100.0 //Current Sense Amplifier

//#define LTC_ADC_RESOL 10000.0 //100uV

float get_stack_current(uint16_t output_xgain)// dc mv *  gain
{
    //converting to voltage DC X.XXXX
    float voutput_xgain = ((float)output_xgain)/LTC_ADC_RESOL;
    float current = (voutput_xgain * setacq_vars[SHUNT_RATED_CURRENT].value)/ (setacq_vars[SHUNT_VOLT_FULLSCALE].value * CSA_GAIN);

    // Avoid Noise
//    if (current <= 0.09f)
//    //if (current <= 10.0f)
//    {
//        current = 0.0f;
//    }

    return current;
}
#define MONITOR_CURRENT_TEMP_AMB (25+10)//
#define DERATING_FACTOR_CONTINUOS_AT_TEMP_AMB (2.0/3.0)
//#define Pa_POWER_AT_TEMP_AMB ( (2.0/3.0)*DC_SHUNT_RATED_CURRENT*DC_SHUNT_OUTPUT_mV_FULLSCALE )//Pa = 0.667 x shuntâ€™s rated power at 25Â°C ambient

float Pa_POWER_AT_TEMP_AMB ( (2.0/3.0)*setacq_vars[SHUNT_RATED_CURRENT].value*setacq_vars[SHUNT_VOLT_FULLSCALE].value );//Pa = 0.667 x shuntâ€™s rated power at 25Â°C ambient



uint8_t current_over_permissible_limit(float current, float temperature)
{
    float Ie;// Maximum permissible continuous current at elevated stack_temperature Te
    float Pe;// Maximum power dissipation at the elevated stack_temperature Te
    float Te;// Elevated stack_temperature

    uint8_t ov_flag;


    //Continuous-Operating derating
    if ( temperature < MONITOR_CURRENT_TEMP_AMB )
    {
        if ( current > (DERATING_FACTOR_CONTINUOS_AT_TEMP_AMB*setacq_vars[SHUNT_RATED_CURRENT].value ) )
            ov_flag = 1;
        else
            ov_flag = 0;
    }
    else //Temperature derating
    {
        Te = temperature;
        Pe= Pa_POWER_AT_TEMP_AMB*(1-((Te-25)/100) );

        float DC_SHUNT_RESISTANCE = (setacq_vars[SHUNT_VOLT_FULLSCALE].value/setacq_vars[SHUNT_RATED_CURRENT].value);//5E-5

        Ie = sqrt(Pe/DC_SHUNT_RESISTANCE);

        if ( current > Ie )
            ov_flag = 1;
        else
            ov_flag = 0;

    }
    return ov_flag;
}
