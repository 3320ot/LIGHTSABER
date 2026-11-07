#ifndef QMA6100P_H_
#define QMA6100P_H_

#include <debug.h>
#include "lib_i2c.h"

typedef struct {
    int16_t dataX;
    int16_t dataY;
    int16_t dataZ;
} data;

#define QMA6100P_ADDR 0x13

uint8_t initQMA6100P();

void i2c_scan_callback(const uint8_t addr);

data readData();

float getDynamicAccelerationMagnitude(data raw);

#endif /* QMA6100P_H_ */
