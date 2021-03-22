#ifndef GETSTRING_H_
#define GETSTRING_H_

void getString_voltage(float V, char *str_dest);
void getString_current(float I, char *str_dest);
void getString_temperature(float T, char *str_dest);
void getString_error(const char *Text, uint8_t E, char *str_dest);
void getString_cell_voltage(float CV, uint8_t num_cell, char *str_dest);


#endif // GETSTRING_H_


