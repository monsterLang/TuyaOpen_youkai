#include "tuya_cloud_types.h"
#include "tal_api.h"
#include "tkl_output.h"
#include "tkl_gpio.h"
#include "tkl_i2c.h"
#include "tkl_pinmux.h"
#include "tal_cli.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
#ifndef EXAMPLE_I2C_SCL_PIN
#define EXAMPLE_I2C_SCL_PIN TUYA_GPIO_NUM_7
#endif

#ifndef EXAMPLE_I2C_SDA_PIN
#define EXAMPLE_I2C_SDA_PIN TUYA_GPIO_NUM_6
#endif

#define TASK_GPIO_PRIORITY THREAD_PRIO_2
#define TASK_GPIO_SIZE     4096

#define I2C_SENSOR_BH1750 0
// #define I2C_SENSOR_SHT4X 1

#define I2C_EXAMPLE_SENSOR_TYPE I2C_SENSOR_BH1750
/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
static THREAD_HANDLE sg_i2c_handle;

/***********************************************************
***********************function define**********************
***********************************************************/

/**
 * @brief i2c task
 *
 * @param[in] param:Task parameters
 * @return none
 */
static void __example_i2c_task(void *param)
{
    OPERATE_RET op_ret = OPRT_OK;
    TUYA_IIC_BASE_CFG_T cfg;

    tal_log_init(TAL_LOG_LEVEL_DEBUG, 1024, (TAL_LOG_OUTPUT_CB)tkl_log_output);

    PR_NOTICE("Application information:");
    PR_NOTICE("Project name:        %s", PROJECT_NAME);
    PR_NOTICE("App version:         %s", PROJECT_VERSION);
    PR_NOTICE("Compile time:        %s", __DATE__);
    PR_NOTICE("TuyaOpen version:    %s", OPEN_VERSION);
    PR_NOTICE("TuyaOpen commit-id:  %s", OPEN_COMMIT);
    PR_NOTICE("Platform chip:       %s", PLATFORM_CHIP);
    PR_NOTICE("Platform board:      %s", PLATFORM_BOARD);
    PR_NOTICE("Platform commit-id:  %s", PLATFORM_COMMIT);

    tkl_io_pinmux_config(EXAMPLE_I2C_SCL_PIN, TUYA_IIC0_SCL);
    tkl_io_pinmux_config(EXAMPLE_I2C_SDA_PIN, TUYA_IIC0_SDA);

    /*i2c init*/
    cfg.role = TUYA_IIC_MODE_MASTER;
    cfg.speed = TUYA_IIC_BUS_SPEED_100K;
    cfg.addr_width = TUYA_IIC_ADDRESS_7BIT;

    op_ret = tkl_i2c_init(TUYA_I2C_NUM_0, &cfg);
    if (OPRT_OK != op_ret) {
        PR_ERR("i2c init fail, err<%d>!", op_ret);
    }

    int cnt = 0;

    while (1) {
        PR_DEBUG("cnt is %d", cnt++);
        tal_system_sleep(1000);

#if (I2C_EXAMPLE_SENSOR_TYPE == I2C_SENSOR_BH1750)
        uint16_t light = 0;
        extern OPERATE_RET bh1750_read_light(int port, uint16_t *light);

        op_ret = bh1750_read_light(TUYA_I2C_NUM_0, &light);
        if (op_ret != OPRT_OK) {
            PR_ERR("bh1750 read fail, err<%d>!", op_ret);
            continue;
        }
        PR_INFO("bh1750 light:%d.%d\n", light / 1000, light % 1000);
// #elif (I2C_EXAMPLE_SENSOR_TYPE == I2C_SENSOR_SHT4X)

#endif
    }
}

/**
 * @brief user_main
 *
 * @return none
 */
static void user_main(void)
{
    OPERATE_RET rt = OPRT_OK;
    tal_log_init(TAL_LOG_LEVEL_DEBUG, 1024, (TAL_LOG_OUTPUT_CB)tkl_log_output);

    PR_DEBUG("hello world\r\n");

    static THREAD_CFG_T thrd_param = {.priority = TASK_GPIO_PRIORITY, .stackDepth = TASK_GPIO_SIZE, .thrdname = "i2c"};
    TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&sg_i2c_handle, NULL, NULL, __example_i2c_task, NULL, &thrd_param));

    // int cnt = 0;
    // while (1) {
    //     PR_DEBUG("cnt is %d", cnt++);
    //     tal_system_sleep(1000);
    // }
}

/**
 * @brief main
 *
 * @param argc
 * @param argv
 * @return void
 */
#if OPERATING_SYSTEM == SYSTEM_LINUX
void main(int argc, char *argv[])
{
    user_main();
}
#else

/* Tuya thread handle */
static THREAD_HANDLE ty_app_thread = NULL;

/**
 * @brief  task thread
 *
 * @param[in] arg:Parameters when creating a task
 * @return none
 */
static void tuya_app_thread(void *arg)
{
    user_main();

    tal_thread_delete(ty_app_thread);
    ty_app_thread = NULL;
}

void tuya_app_main(void)
{
    THREAD_CFG_T thrd_param = {4096, 4, "tuya_app_main"};
    tal_thread_create_and_start(&ty_app_thread, NULL, NULL, tuya_app_thread, NULL, &thrd_param);
}
#endif