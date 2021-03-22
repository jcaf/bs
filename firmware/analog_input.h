#ifndef ANALOG_INPUT_H_
#define ANALOG_INPUT_H_

#define NUM_ANALOG_INPUT_MAX 4

extern float analog_input[NUM_ANALOG_INPUT_MAX];

void analog_input_job(void);

uint16_t analog_input_readvalA1(void);
uint16_t analog_input_readvalA2(void);
uint16_t analog_input_readvalA3(void);
uint16_t analog_input_readvalA4(void);

struct _an_input_factors
{
    float scale;
    float offset;
    uint8_t units;
};

extern struct _an_input_factors an_input_factors[NUM_ANALOG_INPUT_MAX];

#endif


