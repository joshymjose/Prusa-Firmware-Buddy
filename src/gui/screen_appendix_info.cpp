/*
 * screen_appendix_info.cpp
 *
 *  Created on: Jun 30, 2020
 *      Author: joshy
 */

#include "gui.h"
#include "config.h"
#include "screen_menu.h"
#include <stdlib.h>
#include "version.h"
#include "resource.h"
#include "sys.h"

enum {
    TAG_QUIT = 10
};

#pragma pack(push)
#pragma pack(1)

//"C inheritance" of screen_menu_data_t with data items
typedef struct
{
    screen_menu_data_t base;
    menu_item_t items[1];

} this_screen_data_t;

#pragma pack(pop)

void screen_menu_appendix_info_init(screen_t *screen) {
    //=============SCREEN INIT===============
    screen_menu_init(screen, "APPENDIX INFO", ((this_screen_data_t *)screen->pdata)->items, 1, 0, 0);

    p_window_header_set_icon(&(psmd->header), IDR_PNG_header_icon_info);

    psmd->items[0] = menu_item_return;

    uint16_t id = window_create_ptr(WINDOW_CLS_TEXT, psmd->root.win.id, rect_ui16(10, 80, 220, 200), &(psmd->help));
    psmd->help.font = resource_font(IDR_FNT_NORMAL);
    if (sys_fw_appendix_status()) {
        char *appendix_info_str = "Appendix broken";
        window_set_text(id, appendix_info_str);
    } else {
        char *appendix_info_str2 = "Appendix exists";
        window_set_text(id, appendix_info_str2);
    }
}

void screen_menu_appendix_info_done(screen_t *screen) {

    screen_menu_done(screen);
}

screen_t screen_appendix_info = {
    0,
    0,
    screen_menu_appendix_info_init,
    screen_menu_appendix_info_done,
    screen_menu_draw,
    screen_menu_event,
    sizeof(this_screen_data_t), //data_size
    0,                          //pdata
};

const screen_t *pscreen_appendix_info = &screen_appendix_info;
