# BMM350 Sensor API integration example
This repository will guide a developer through the basics of integrating the BMM350 sensor API into a user application.

## Setup
Clone repository & setup submodules
```bash
git clone https://github.com/everyday-dev/bmm350_example
cd bmm350_example
git submodule update --init
```

## Build example application
Building the application uses GCC and Make. Execute the following steps to build
```bash
make -j12
```

## Code explained
*All snippets are pulled from `main.c`*. To implement the sensor API, you must provide three functions via a `struct bmm350_dev` variable. These functions abstract away the actual I2c interface from the sensor driver, allowing for the driver to be portable across mutliple platforms.
```c
int main(void) {
    uint8_t dev_addr = BMM350_I2C_ADSEL_SET_LOW;
    struct bmm350_dev dev = {0x00};
    int8_t rslt;

    // Supply our Hardware abstraction functions to the BM350 dev struct
    dev.intf_ptr = &dev_addr;
    dev.read = bmm350_i2c_read;
    dev.write = bmm350_i2c_write;
    dev.delay_us = bmm350_delay;

    ...
}
```

Each of these functions should be filled out using your MCU specific code. Next we can just implement the sensor API as necessary for your
configuration and uses cases. No other special setup is needed.
```c
BMM350_INTF_RET_TYPE bmm350_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    // Place MCU specific code here to initiate the I2C read with the provided params

    return BMM350_OK;
}


BMM350_INTF_RET_TYPE bmm350_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    // Place MCU specific code here to initiate the I2C write with the provided params

    return BMM350_OK;
}


void bmm350_delay(uint32_t period, void *intf_ptr)
{
    // Place MCU specific code here to initiate a ms delay for the specified period
}
```

The example code in `main.c` is setup to poll the BM350 Magnetometer and tempature information via a delay as well as shows how to use the interrupt ready register (data ready) from the sensor itself.

# Contact
All questions can be sent to stephen@everydaydev.io