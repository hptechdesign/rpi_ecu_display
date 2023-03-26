#include "lvgl/lvgl.h"
#include "ecu_configs.h"

#if RPI_ECU_DISPLAY
#include "init_rpi_env.h"
#endif // RPI_ECU_DISPLAY

#if SDL_ECU_DISPLAY
#include "sdl/sdl.h"
#include <stdio.h>
#endif  //SDL_ECU_DISPLAY


/// Common includes
#include "bar_waterTemp.h"
#include "meter_airPressure.h"
#include "meter_fuelPressure.h"
#include "meter_oilPressure.h"
#include "meter_rpm.h"

#define DISP_BUF_SIZE (128 * 1024)




int main(int argc, char *argv[])
{
    /*LittlevGL init*/
    lv_init();
    sdl_init();
    printf("Begin main loop\n");


#if SDL_ECU_DISPLAY
#define BUFFER_SIZE (SDL_HOR_RES * SDL_VER_RES)
    /*A static or global variable to store the buffers*/
    static lv_disp_draw_buf_t disp_buf;
/*Static or global buffer(s). The second buffer is optional*/
    static lv_color_t *buf_1[BUFFER_SIZE] = {0};
/*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
    lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, BUFFER_SIZE);
    static lv_disp_drv_t disp_drv;         /*A variable to hold the drivers. Must be static or global.*/
    lv_disp_drv_init(&disp_drv);           /*Basic initialization*/
    disp_drv.draw_buf = &disp_buf;         /*Set an initialized buffer*/
    disp_drv.flush_cb = sdl_display_flush; /*Set a flush callback to draw to the display*/
    disp_drv.hor_res  = SDL_HOR_RES;       /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res  = SDL_VER_RES;       /*Set the vertical resolution in pixels*/
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
    lv_theme_default_init(disp, lv_color_make(0x77, 0x44, 0xBB), lv_color_make(0x14, 0x14, 0x3C), 1, lv_font_default());

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv); /*Basic initialization*/
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = sdl_mouse_read;
    lv_indev_drv_register(&indev_drv);

#endif


#if RPI_ECU_DISPLAY
//// SETUP PI:
    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 800;
    disp_drv.ver_res    = 480;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_1.read_cb = evdev_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);


    /*Set a cursor for the mouse*/
    LV_IMG_DECLARE(mouse_cursor_icon)
    lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
    lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/
#endif // RPI_ECU_DISPLAY

    /*Draw Widgets*/
    bar_waterTemp1();
    bar_waterTemp2();
    meter_oilPressure();
    meter_airPressure();
    meter_fuelPressure();
    meter_rpm();
    

#if RPI_ECU_DISPLAY
    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }
#endif  /// RPI_ECU_DISPLAY

#if SDL_ECU_DISPLAY
    for (;;) {
        // Run LVGL engine
        lv_tick_inc(1);
        lv_timer_handler();
        usleep(1000);
    }
#endif // SDL_ECU_DISPLAY

    return 0;
}


#if RPI_ECU_DISPLAY
/// Used in RPI build, not windows:
/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
#endif // RPI_ECU_DISPLAY