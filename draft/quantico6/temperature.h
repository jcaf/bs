#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

//float ntc10k_st(uint16_t ADC_CURRENT_VAL, uint16_t ADC_TOP_VAL);

#define NTC10K_NO_CONNECTED -600
#define NTC10K_OUT_RANGE -601

float temperature_read(uint16_t ADC_CURRENT_VAL, uint16_t ADC_TOP_VAL);
#endif


