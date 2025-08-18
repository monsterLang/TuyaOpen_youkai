/**
 * @file bh1750.c
 * @brief BH1750 sensor driver for Tuya IoT projects.
 *
 * This file provides an implementation of a driver for the BH1750 sensor, which is a humidity and temperature sensor.
 * It demonstrates the configuration and usage of the BH1750 sensor using the Tuya SDK.
 * The example covers initializing the sensor, sending commands to the sensor, and reading data from the sensor.
 *
 * The BH1750 sensor driver aims to help developers understand how to interface with the BH1750 sensor in Tuya IoT
 * projects. It includes detailed examples of setting up sensor configurations, sending commands, and reading data from
 * the sensor.
 *
 * @note This example is designed to be adaptable to various Tuya IoT devices and platforms, showcasing fundamental
 * sensor operations that are critical for IoT device development.
 *
 * @copyright Copyright (c) 2021-2024 Tuya Inc. All Rights Reserved.
 *
 */

#include "tuya_cloud_types.h"
#include "tal_api.h"
#include "tkl_output.h"
#include "tkl_i2c.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#define CRC_OK  (0)
#define CRC_ERR (-1)

// #define BH1750_CMD_FETCH_DATA    0xE000 // readout measurements for periodic mode
// #define BH1750_CMD_MEAS_PERI_1_H 0x2130 // measurement: periodic 1 mps, high repeatability

/**
 * @brief work status
 */
#define BH1750_CLOSE                 0
#define BH1750_OPEN                  (!BH1750_CLOSE)

/**
 * @brief resource array index
 */
#define BH1750_RSRC_INDEX_STAT       0
#define BH1750_RSRC_INDEX_ADDR       1
#define BH1750_RSRC_INDEX_PREC       2  // H-Resolution Mode/H-Resolution Mode2/L-Resolution
#define BH1750_RSRC_INDEX_FREQ       3  // one/continue
#define BH1750_RSRC_INDEX_RESL       4  // mode match value, such as H=1,H2=0.5,L=41

/**
 * @brief I2C address list
 */
#define SR_I2C_ADDR_BH1750_A         0x23    // BH1750 : ADDR pin - GND
#define SR_I2C_ADDR_BH1750_B         0x5c    // BH1750 : ADDR pin - VCC


/***********************************************************
***********************typedef define***********************
***********************************************************/

#define BH1750_CMD_POWER_DOWN   	    0x00	// power down
#define BH1750_CMD_POWER_ON			    0x01	// power on
#define BH1750_CMD_SOFT_RESET			0x07	// reset

#define BH1750_CMD_CON_H_RES_MODE	    0x10	// Continuously H-Resolution Mode
#define BH1750_CMD_CON_H_RES_MODE2	    0x11	// Continuously H-Resolution Mode2
#define BH1750_CMD_CON_L_RES_MODE	    0x13	// Continuously L-Resolution Mode
#define BH1750_CMD_ONE_H_RES_MODE	    0x20	// One Time H-Resolution Mode
#define BH1750_CMD_ONE_H_RES_MODE2	    0x21	// One Time H-Resolution Mode2
#define BH1750_CMD_ONE_L_RES_MODE	    0x23	// One Time L-Resolution Mode

// #define BH1750_CMD_SOFT_RESET        0x30A2  // soft reset
#define BH1750_CMD_READ_SERIALNBR    0x3780  // read serial number
#define BH1750_CMD_HEATER_ENABLE     0x306D  // enabled heater
#define BH1750_CMD_HEATER_DISABLE    0x3066  // disable heater
#define BH1750_CMD_READ_STATUS       0xF32D  // read status register
#define BH1750_CMD_CLEAR_STATUS      0x3041  // clear status register
#define BH1750_CMD_ART               0x2B32  // activate ART
#define BH1750_CMD_BREAK             0x3093  // stop periodic data acquisition mode
#define BH1750_CMD_FETCH_DATA        0xE000  // readout measurements for periodic mode
#define BH1750_CMD_MEAS_PERI_05_H    0x2032  // measurement: periodic 0.5 mps, high repeatability
#define BH1750_CMD_MEAS_PERI_05_M    0x2024  // measurement: periodic 0.5 mps, medium repeatability
#define BH1750_CMD_MEAS_PERI_05_L    0x202F  // measurement: periodic 0.5 mps, low repeatability
#define BH1750_CMD_MEAS_PERI_1_H     0x2130  // measurement: periodic 1 mps, high repeatability
#define BH1750_CMD_MEAS_PERI_1_M     0x2126  // measurement: periodic 1 mps, medium repeatability
#define BH1750_CMD_MEAS_PERI_1_L     0x212D  // measurement: periodic 1 mps, low repeatability
#define BH1750_CMD_MEAS_PERI_2_H     0x2236  // measurement: periodic 2 mps, high repeatability
#define BH1750_CMD_MEAS_PERI_2_M     0x2220  // measurement: periodic 2 mps, medium repeatability
#define BH1750_CMD_MEAS_PERI_2_L     0x222B  // measurement: periodic 2 mps, low repeatability
#define BH1750_CMD_MEAS_PERI_4_H     0x2334  // measurement: periodic 4 mps, high repeatability
#define BH1750_CMD_MEAS_PERI_4_M     0x2322  // measurement: periodic 4 mps, medium repeatability
#define BH1750_CMD_MEAS_PERI_4_L     0x2329  // measurement: periodic 4 mps, low repeatability
#define BH1750_CMD_MEAS_PERI_10_H    0x2737  // measurement: periodic 10 mps, high repeatability
#define BH1750_CMD_MEAS_PERI_10_M    0x2721  // measurement: periodic 10 mps, medium repeatability
#define BH1750_CMD_MEAS_PERI_10_L    0x272A  // measurement: periodic 10 mps, low repeatability
#define BH1750_CMD_MEAS_POLLING_H    0x2400  // measurement: polling, high repeatability
#define BH1750_CMD_MEAS_POLLING_M    0x240B  // measurement: polling, medium repeatability
#define BH1750_CMD_MEAS_POLLING_L    0x2416  // measurement: polling, low repeatability
#define BH1750_CMD_MEAS_CLOCKSTR_H   0x2C06  // measurement: clock stretching, high repeatability
#define BH1750_CMD_MEAS_CLOCKSTR_M   0x2C0D  // measurement: clock stretching, medium repeatability
#define BH1750_CMD_MEAS_CLOCKSTR_L   0x2C10  // measurement: clock stretching, low repeatability
#define BH1750_CMD_W_AL_LIM_HS       0x611D  // write alert limits, high set
#define BH1750_CMD_W_AL_LIM_HC       0x6116  // write alert limits, high clear
#define BH1750_CMD_W_AL_LIM_LC       0x610B  // write alert limits, low clear
#define BH1750_CMD_W_AL_LIM_LS       0x6100  // write alert limits, low set
#define BH1750_CMD_R_AL_LIM_LS       0xE102  // read alert limits, low set
#define BH1750_CMD_R_AL_LIM_LC       0xE109  // read alert limits, low clear
#define BH1750_CMD_R_AL_LIM_HS       0xE11F  // read alert limits, high set
#define BH1750_CMD_R_AL_LIM_HC       0xE114  // read alert limits, high clear

/***********************************************************
***********************variable define**********************
***********************************************************/

/***********************************************************
***********************function define**********************
***********************************************************/
/**
 * @brief delay (ms)
 *
 * @param[in] tm: delay time
 *
 * @return none
 */
static void __bh1750_delay_ms(const uint32_t tm)
{
    tal_system_sleep(tm);
}

// /**
//  * @brief get CRC8 value for bh1750
//  *
//  * @param[in] data: data to be calculated
//  * @param[in] len: data length
//  *
//  * @return CRC8 value
//  */
// static uint8_t __bh1750_get_crc8(const uint8_t *data, uint16_t len)
// {
//     uint8_t i;
//     uint8_t crc = 0xFF;

//     while (len--) {
//         crc ^= *data;

//         for (i = 8; i > 0; --i) {
//             if (crc & 0x80) {
//                 crc = (crc << 1) ^ 0x31;
//             } else {
//                 crc = (crc << 1);
//             }
//         }
//         data++;
//     }

//     return crc;
// }

// /**
//  * @brief check CRC8
//  *
//  * @param[in] data: data to be checked
//  * @param[in] len: data length
//  * @param[in] crc_val: crc value
//  *
//  * @return check result
//  */
// static int __bh1750_check_crc8(const uint8_t *data, const uint16_t len, const uint8_t crc_val)
// {
//     if (__bh1750_get_crc8(data, len) != crc_val) {
//         return CRC_ERR;
//     }
//     return CRC_OK;
// }

/**
 * @brief read data from bh1750
 *
 * @param[in] dev: device resource
 * @param[in] len: data length
 * @param[out] data: data received from bh1750
 *
 * @return none
 */
static OPERATE_RET __bh1750_read_data(const uint8_t port, const uint16_t len, uint8_t *data)
{
    return tkl_i2c_master_receive(port, SR_I2C_ADDR_BH1750_A, data, len, FALSE);
}

// /**
//  * @brief write command to bh1750
//  *
//  * @param[in] dev: device resource
//  * @param[in] cmd: control command
//  *
//  * @return none
//  */
// static OPERATE_RET __bh1750_write_cmd2(const uint8_t port, const uint16_t cmd)
// {
//     uint8_t buf[2];
//     buf[0] = (uint8_t)(cmd >> 8);
//     buf[1] = (uint8_t)(cmd & 0x00FF);

//     return tkl_i2c_master_send(port, SR_I2C_ADDR_BH1750_A, buf, 2, FALSE);
// }

static OPERATE_RET __bh1750_write_cmd(const uint8_t port, const uint16_t cmd)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(cmd & 0x00FF);
    buf[1] = (uint8_t)(cmd & 0x00FF);

    return tkl_i2c_master_send(port, SR_I2C_ADDR_BH1750_A, buf, 1, FALSE);
}

// /**
//  * @brief write command and data to bh1750
//  *
//  * @param[in] dev: device resource
//  * @param[in] cmd: control command
//  * @param[in] data: data to be written
//  *
//  * @return none
//  */
// static void __bh1750_write_2bytes_data(const uint8_t port, const uint16_t cmd, const uint16_t data)
// {
//     uint8_t buf[5];
//     buf[0] = (uint8_t)(cmd >> 8);
//     buf[1] = (uint8_t)(cmd & 0x00FF);
//     buf[2] = (uint8_t)(data >> 8);
//     buf[3] = (uint8_t)(data & 0x00FF);
//     buf[4] = __bh1750_get_crc8(buf+2, 2);

//     return tkl_i2c_master_send(port, SR_I2C_ADDR_BH1750_A, buf, 5, FALSE);
// }

/**
 * @brief read temperature and humidity from bh1750
 *
 * @param[in] dev: device resource
 * @param[out] light: temperature value
 *
 * @return OPRT_OK on success, others on error
 */
OPERATE_RET bh1750_read_temp_humi(int port, uint16_t *light)
{
    uint8_t buf[6] = {0};
    OPERATE_RET ret = OPRT_OK;

    // reset
    __bh1750_write_cmd(port, BH1750_CMD_SOFT_RESET);

    // set mode
    // __bh1750_write_cmd(port, dev->info[BH1750_RSRC_INDEX_MODE]);
    // __bh1750_start_periodic_measurement(port);
    __bh1750_write_cmd(port, BH1750_CMD_ONE_H_RES_MODE);

    // delay -- wait value return
    __bh1750_delay_ms(180);

    ret = __bh1750_read_data(port, 2, buf); 
    if(ret != OPRT_OK)
        return ret;

    *light = ((uint16_t)buf[0] << 8) | buf[1];

    return OPRT_OK;
}