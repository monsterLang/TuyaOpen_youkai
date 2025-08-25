
#include "tal_cli.h"

#include "tuya_cloud_types.h"

#include "tal_api.h"
#include "tkl_output.h"
#include "tkl_spi.h"
#include "tkl_system.h"

// lvgl
#include "lvgl.h"
// #include "demos/lv_demos.h"
// #include "examples/lv_examples.h"
// #include "lv_vendor.h"

// board
// #include "board_com_api.h"

// lvgl gui output
// #include "events_init.h"
// #include "gui_guider.h"
// #include "custom.h"
#include "lvgl_ui/generated/events_init.h"
#include "lvgl_ui/generated/gui_guider.h"
#include "lvgl_ui/custom/custom.h"

lv_ui guider_ui;

void lv_demo_01_show_pic()
{
    PR_DEBUG("%s", __func__);



    setup_ui(&guider_ui);
    events_init(&guider_ui);
    custom_init(&guider_ui);
}

static void user_main(void)
{
    /* basic init */
    tal_log_init(TAL_LOG_LEVEL_DEBUG, 4096, (TAL_LOG_OUTPUT_CB)tkl_log_output);

    /*hardware register*/
    board_register_hardware();

    lv_vendor_init(DISPLAY_NAME);

    // lv_demo_widgets();
    lv_demo_01_show_pic();

    lv_vendor_start();
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