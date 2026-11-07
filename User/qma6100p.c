#include <debug.h>
#include <qma6100p.h>
#include "lib_i2c.h"

i2c_device_t dev = {
    .clkr = I2C_CLK_100KHZ,
    .type = I2C_ADDR_7BIT,
    .addr = QMA6100P_ADDR,
    .regb = 1,
    .tout = 2000,
};
const int8_t s = 0xB6;
const int8_t act = 0x84;
const int8_t act1 = 0x01;
const int8_t act2 = 0x0A;
const int8_t act3 = 0x0D;

void i2c_scan_callback(const uint8_t addr)
{
    Delay_Ms(1000);
//    printf("Address: 0x%02X Responded.\n", addr);
}

uint8_t initQMA6100P(){
    i2c_init(&dev);
    Delay_Ms(250);

//    printf("----Scanning I2C Bus for Devices----\n");
    i2c_scan(i2c_scan_callback);
//    printf("----Done Scanning----\n\n");

    // 1. Soft Reset (обязательно рекомендуется)
    uint8_t val = 0xB6;
    i2c_write_reg(&dev, 0x36, &val, 1);
    Delay_Ms(50);
    val = 0x00;
    i2c_write_reg(&dev, 0x36, &val, 1);
    Delay_Ms(10);

    val = 0x02;
    i2c_write_reg(&dev, 0x0F, &val, 1);

    val = 0xE0;
    i2c_write_reg(&dev, 0x10, &val, 1);

    val = 0x80;
    i2c_write_reg(&dev, 0x11, &val, 1);
    Delay_Ms(2);

//    printf("QMA6100P initialized!\n");

    uint8_t check = 0;
    i2c_read_reg(&dev, 0x11, &check, 1);
//    printf("PM register (0x11) after initialization: 0x%02X (should be 0x80 or higher)\n", check);
    return check;
}

uint8_t buf[6];
data readData(void) {
    i2c_read_reg(&dev, 0x01, buf, 6);   // 0x01..0x06

    int16_t x = (int16_t)((buf[1] << 8) | buf[0]) >> 2;  // 14-bit signed
    int16_t y = (int16_t)((buf[3] << 8) | buf[2]) >> 2;
    int16_t z = (int16_t)((buf[5] << 8) | buf[4]) >> 2;

    return (data){ .dataX = x, .dataY = y, .dataZ = z };
}
