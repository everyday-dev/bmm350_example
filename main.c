#include <stdio.h>
#include <stdlib.h>
#include "bmm350.h"
#include "bmm350_defs.h"

BMM350_INTF_RET_TYPE bmm350_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
BMM350_INTF_RET_TYPE bmm350_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
void bmm350_delay(uint32_t period, void *intf_ptr);

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

uint32_t get_millis(void) {
    // Return the current millis tick of the system.
    return 0;
}

int main(void) {
    uint8_t dev_addr = BMM350_I2C_ADSEL_SET_LOW;
    struct bmm350_dev dev = {0x00};
    int8_t rslt;

    // Supply our Hardware abstraction functions to the BM350 dev struct
    dev.intf_ptr = &dev_addr;
    dev.read = bmm350_i2c_read;
    dev.write = bmm350_i2c_write;
    dev.delay_us = bmm350_delay;

    uint8_t int_status, int_ctrl, err_reg_data = 0;
    uint8_t loop, set_int_ctrl;
    uint32_t time_ms = 0;

    struct bmm350_mag_temp_data mag_temp_data;
    struct bmm350_pmu_cmd_status_0 pmu_cmd_stat_0;

    /* Initialize BMM350 */
    rslt = bmm350_init(&dev);

    printf("Read : 0x00 : BMM350 Chip ID : 0x%X\n", dev.chip_id);

    /* Check PMU busy */
    rslt = bmm350_get_pmu_cmd_status_0(&pmu_cmd_stat_0, &dev);

    printf("Expected : 0x07 : PMU cmd busy : 0x0\n");
    printf("Read : 0x07 : PMU cmd busy : 0x%X\n", pmu_cmd_stat_0.pmu_cmd_busy);

    /* Get error data */
    rslt = bmm350_get_regs(BMM350_REG_ERR_REG, &err_reg_data, 1, &dev);

    printf("Expected : 0x02 : Error Register : 0x0\n");
    printf("Read : 0x02 : Error Register : 0x%X\n", err_reg_data);

    /* Configure interrupt settings */
    rslt = bmm350_configure_interrupt(BMM350_PULSED,
                                      BMM350_ACTIVE_HIGH,
                                      BMM350_INTR_PUSH_PULL,
                                      BMM350_UNMAP_FROM_PIN,
                                      &dev);

    /* Enable data ready interrupt */
    rslt = bmm350_enable_interrupt(BMM350_ENABLE_INTERRUPT, &dev);

    /* Get interrupt settings */
    rslt = bmm350_get_regs(BMM350_REG_INT_CTRL, &int_ctrl, 1, &dev);

    set_int_ctrl = (BMM350_INT_POL_ACTIVE_HIGH << 1) | (BMM350_INT_OD_PUSHPULL << 2) | (BMM350_ENABLE << 7);

    printf("Expected : 0x2E : Interrupt control : 0x%X\n", set_int_ctrl);
    printf("Read : 0x2E : Interrupt control : 0x%X\n", int_ctrl);

    if (int_ctrl & BMM350_DRDY_DATA_REG_EN_MSK)
    {
        printf("Data ready enabled\r\n");
    }

    /* Set ODR and performance */
    rslt = bmm350_set_odr_performance(BMM350_DATA_RATE_25HZ, BMM350_AVERAGING_8, &dev);

    /* Enable all axis */
    rslt = bmm350_enable_axes(BMM350_X_EN, BMM350_Y_EN, BMM350_Z_EN, &dev);

    if (rslt == BMM350_OK)
    {
        rslt = bmm350_set_powermode(BMM350_NORMAL_MODE, &dev);

        loop = 20;

        printf("\nCompensated Magnetometer and temperature data read with delay\n");
        printf("Timestamp(ms), Mag_X(uT), Mag_Y(uT), Mag_Z(uT), Temperature(degC)\n");

        // TO
        /* Time in milliseconds */
        time_ms = get_millis();

        while (loop)
        {
            rslt = bmm350_delay_us(36000, &dev);

            rslt = bmm350_get_compensated_mag_xyz_temp_data(&mag_temp_data, &dev);

            printf("%lu, %f, %f, %f, %f\n",
                   (long unsigned int)(get_millis() - time_ms),
                   mag_temp_data.x,
                   mag_temp_data.y,
                   mag_temp_data.z,
                   mag_temp_data.temperature);

            loop--;
        }

        loop = 22;

        printf("\nCompensated Magnetometer and temperature data read with INT\n");

        printf("Timestamp(ms), Mag_X(uT), Mag_Y(uT), Mag_Z(uT), Temperature(degC)\n");

        /* Time in milliseconds */
        time_ms = get_millis();

        while (loop)
        {
            int_status = 0;

            /* Get data ready interrupt status */
            rslt = bmm350_get_regs(BMM350_REG_INT_STATUS, &int_status, 1, &dev);

            /* Check if data ready interrupt occurred */
            if (int_status & BMM350_DRDY_DATA_REG_MSK)
            {
                rslt = bmm350_get_compensated_mag_xyz_temp_data(&mag_temp_data, &dev);

                printf("%lu, %f, %f, %f, %f\n",
                       (long unsigned int)(get_millis() - time_ms),
                       mag_temp_data.x,
                       mag_temp_data.y,
                       mag_temp_data.z,
                       mag_temp_data.temperature);

                loop--;
            }
        }
    }

    return rslt;
}