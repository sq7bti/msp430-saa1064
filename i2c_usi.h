#ifndef I2C_USI_H
#define I2C_USI_H

// make sure it is even address:
// Address is 0x71/0x70 for R/W
#define I2C_Addr (0x38 << 1)

void Setup_I2C(unsigned char* led_buff, unsigned char* adc_buff, unsigned char* conf);

#endif
