
#ifndef LINDUINO_H
#define LINDUINO_H

//! @name PIN ASSIGNMENTS
//! @{

#define QUIKEVAL_GPIO 9
#define QUIKEVAL_CS SS
#define QUIKEVAL_MUX_MODE_PIN 8

// Macros
//! Set "pin" low
//! @param pin pin to be driven LOW
#define output_low(pin)   digitalWrite(pin, LOW)
//! Set "pin" high
//! @param pin pin to be driven HIGH
#define output_high(pin)  digitalWrite(pin, HIGH)
//! Return the state of pin "pin"
//! @param pin pin to be read (HIGH or LOW).
//! @return the state of pin "pin"
#define input(pin)        digitalRead(pin)

//! @todo Make a note about whether Arduino/Linduino is Big Endian or Little Endian. Raspberry Pi appears to be the opposite.
//! This union splits one int16_t (16-bit signed integer) or uint16_t (16-bit unsigned integer)
//! into two uint8_t's (8-bit unsigned integers) and vice versa.
  union LT_union_int16_2bytes
  {
    int16_t LT_int16;    //!< 16-bit signed integer to be converted to two bytes
    uint16_t LT_uint16;  //!< 16-bit unsigned integer to be converted to two bytes
    uint8_t LT_byte[2];  //!< 2 bytes (unsigned 8-bit integers) to be converted to a 16-bit signed or unsigned integer
  };

//! @todo Make a note about whether Arduino/Linduino is Big Endian or Little Endian. Raspberry Pi appears to be the opposite.
//! This union splits one int32_t (32-bit signed integer) or uint32_t (32-bit unsigned integer)
//! into four uint8_t's (8-bit unsigned integers) and vice versa.
union LT_union_int32_4bytes
{
  int32_t LT_int32;    //!< 32-bit signed integer to be converted to four bytes
  uint32_t LT_uint32;  //!< 32-bit unsigned integer to be converted to four bytes
  uint8_t LT_byte[4];  //!< 4 bytes (unsigned 8-bit integers) to be converted to a 32-bit signed or unsigned integer
};

//! @todo Make a note about whether Arduino/Linduino is Big Endian or Little Endian. Raspberry Pi appears to be the opposite.
//! This union splits one float into four uint8_t's (8-bit unsigned integers) and vice versa.
union LT_union_float_4bytes
{
  float LT_float;      //!< float to be converted to four bytes
  uint8_t LT_byte[4];  //!< 4 bytes (unsigned 8-bit integers) to be converted to a float
};


#endif  // LINDUINO_H
