/*
 * CCS811.h
 *
 *  Created on: Nov 9, 2026
 *      Author: tient
 */

#ifndef INC_CCS811_H_
#define INC_CCS811_H_

#include "stdio.h"
#include "string.h"
#include "main.h"
#include "i2c.h"
#define CCS811_ADDR       (0x5A << 1)

#define CCS811_STATUS     0x00
#define CCS811_MEAS_MODE  0x01
#define CCS811_ALG_RESULT 0x02
#define CCS811_HW_ID      0x20



#endif /* INC_CCS811_H_ */
