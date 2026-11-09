/*
 * CCS811.c
 *
 *  Created on: Nov 9, 2026
 *      Author: tient
 */
#include "CCS811.h"

uint8_t ccs_data[5];

uint16_t eco2 = 0;
uint16_t tvoc = 0;

uint8_t hw_id;
uint8_t status;

char msg[100];

void CCS811_Init(void)
{
    HAL_I2C_Mem_Read(&hi2c1,
                     CCS811_ADDR,
                     CCS811_HW_ID,
                     I2C_MEMADD_SIZE_8BIT,
                     &hw_id,
                     1,
                     100);

    if(hw_id != 0x81)
    {
        return;
    }

    // APP_START: gửi trực tiếp bằng Master Transmit
    uint8_t cmd = 0xF4;

    HAL_I2C_Master_Transmit(&hi2c1,
                            CCS811_ADDR,
                            &cmd,
                            1,
                            100);

    HAL_Delay(20);

    // Chế độ đo mỗi 1 giây
    uint8_t mode = 0x10;

    HAL_I2C_Mem_Write(&hi2c1,
                      CCS811_ADDR,
                      CCS811_MEAS_MODE,
                      I2C_MEMADD_SIZE_8BIT,
                      &mode,
                      1,
                      100);
}
void CCS811_Read(void)
{
    // Đọc STATUS
    HAL_I2C_Mem_Read(&hi2c1,
                     CCS811_ADDR,
                     CCS811_STATUS,
                     I2C_MEMADD_SIZE_8BIT,
                     &status,
                     1,
                     100);

    // Kiểm tra DATA_READY (bit 3)
    if(status & 0x08)
    {
        if(HAL_I2C_Mem_Read(&hi2c1,
                            CCS811_ADDR,
                            CCS811_ALG_RESULT,
                            I2C_MEMADD_SIZE_8BIT,
                            ccs_data,
                            5,
                            100) == HAL_OK)
        {
            eco2 = (ccs_data[0] << 8) | ccs_data[1];

            tvoc = (ccs_data[2] << 8) | ccs_data[3];
        }
    }
}
