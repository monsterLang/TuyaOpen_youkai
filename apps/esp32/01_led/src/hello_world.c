#include "tuya_cloud_types.h"
#include "tal_api.h"
#include "tkl_output.h"
#include "tal_cli.h"
#include "tkl_gpio.h"


/***********************************************************
*************************micro define***********************
***********************************************************/
#ifndef EXAMPLE_OUTPUT_PIN
#define EXAMPLE_OUTPUT_PIN TUYA_GPIO_NUM_12
#endif

#ifndef EXAMPLE_OUTPUT_PIN2
#define EXAMPLE_OUTPUT_PIN2 TUYA_GPIO_NUM_13
#endif

// #ifndef EXAMPLE_IRQ_PIN
// #define EXAMPLE_IRQ_PIN TUYA_GPIO_NUM_6
// #endif

#define TASK_GPIO_PRIORITY THREAD_PRIO_2
#define TASK_GPIO_SIZE     4096

/***********************************************************
***********************typedef define***********************
***********************************************************/

/***********************************************************
***********************variable define**********************
***********************************************************/
static THREAD_HANDLE sg_gpio_handle;

/***********************************************************
***********************function define**********************
***********************************************************/
// /**
//  * @brief interrupt callback function
//  *
//  * @param[in] args:parameters
//  * @return none
//  */
// static void __gpio_irq_callback(void *args)
// {
//     /* Both TAL_PR_ and PR_ have locks in these two types of printing and should not be used in interrupts. */
//     tkl_log_output("\r\n------------ GPIO IRQ Callbcak ------------\r\n");
// }

/**
 * @brief gpio task
 *
 * @param[in] param:Task parameters
 * @return none
 */
static void __example_gpio_task(void *param)
{
    OPERATE_RET rt = OPRT_OK;
    uint8_t i = 0;
    // TUYA_GPIO_LEVEL_E read_level = 0;

    /*GPIO output init*/
    TUYA_GPIO_BASE_CFG_T out_pin_cfg = {
        .mode = TUYA_GPIO_PUSH_PULL, .direct = TUYA_GPIO_OUTPUT, .level = TUYA_GPIO_LEVEL_LOW};
    TUYA_CALL_ERR_LOG(tkl_gpio_init(EXAMPLE_OUTPUT_PIN, &out_pin_cfg));

    /*GPIO output init*/
    TUYA_GPIO_BASE_CFG_T out_pin2_cfg = {
        .mode = TUYA_GPIO_PUSH_PULL, .direct = TUYA_GPIO_OUTPUT, .level = TUYA_GPIO_LEVEL_LOW};
    TUYA_CALL_ERR_LOG(tkl_gpio_init(EXAMPLE_OUTPUT_PIN2, &out_pin_cfg));

    // /*GPIO input init*/
    // TUYA_GPIO_BASE_CFG_T in_pin_cfg = {
    //     .mode = TUYA_GPIO_PULLUP,
    //     .direct = TUYA_GPIO_INPUT,
    // };
    // TUYA_CALL_ERR_LOG(tkl_gpio_init(EXAMPLE_INPUT_PIN, &in_pin_cfg));

    // /*GPIO irq init*/
    // TUYA_CALL_ERR_LOG(tkl_gpio_init(EXAMPLE_IRQ_PIN, &in_pin_cfg));
    // TUYA_GPIO_IRQ_T irq_cfg = {
    //     .cb = __gpio_irq_callback,
    //     .arg = NULL,
    //     .mode = TUYA_GPIO_IRQ_RISE,
    // };
    // TUYA_CALL_ERR_LOG(tkl_gpio_irq_init(EXAMPLE_IRQ_PIN, &irq_cfg));

    // /*irq enable*/
    // TUYA_CALL_ERR_LOG(tkl_gpio_irq_enable(EXAMPLE_IRQ_PIN));

    while (1) {
        /* GPIO output */
        if (i == 0) {
            tkl_gpio_write(EXAMPLE_OUTPUT_PIN, TUYA_GPIO_LEVEL_HIGH);
            tkl_gpio_write(EXAMPLE_OUTPUT_PIN2, TUYA_GPIO_LEVEL_LOW);
            PR_DEBUG("pin output high %d", i);
        } else {
            tkl_gpio_write(EXAMPLE_OUTPUT_PIN, TUYA_GPIO_LEVEL_LOW);
            tkl_gpio_write(EXAMPLE_OUTPUT_PIN2, TUYA_GPIO_LEVEL_HIGH);
            PR_DEBUG("pin output low");
        }
        i = i ^ 1;

        // /* GPIO read */
        // TUYA_CALL_ERR_LOG(tkl_gpio_read(EXAMPLE_INPUT_PIN, &read_level));
        // if (read_level == 1) {
        //     PR_DEBUG("GPIO read high level");
        // } else {
        //     PR_DEBUG("GPIO read low level");
        // }

        tal_system_sleep(5000);
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

    static THREAD_CFG_T thrd_param = {.priority = TASK_GPIO_PRIORITY, .stackDepth = TASK_GPIO_SIZE, .thrdname = "gpio"};
    TUYA_CALL_ERR_LOG(tal_thread_create_and_start(&sg_gpio_handle, NULL, NULL, __example_gpio_task, NULL, &thrd_param));

    int cnt = 0;
    while (1) {
        PR_DEBUG("cnt is %d", cnt++);
        tal_system_sleep(10000);
    }
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