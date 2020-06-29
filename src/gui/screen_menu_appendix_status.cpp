/*
 * screen_menu_appendix_status.cpp
 *
 *  Created on: Jun 29, 2020
 *      Author: joshy
 */

#include "screen_menu_appendix_status.h"
#include "screens.h"
#include "sys.h"
#include "screen_menu.hpp"
#include "WindowMenuItems.hpp"
#include "../lang/i18n.h"


using parent = ScreenMenu<EHeader::Off, EFooter::On, HelpLines_Default, MI_RETURN>;

class ScreenMenuAppendixStatus : public parent {
public:
    constexpr static const char *const label = N_("APPENDIX STATUS");
    static void Init(screen_t *screen);
    static int CEvent(screen_t *screen, window_t *window, uint8_t event, void *param);
};

/*****************************************************************************/
//static member method definition
void ScreenMenuAppendixStatus::Init(screen_t *screen) {
    Create(screen, label);
    auto *ths = reinterpret_cast<ScreenMenuAppendixStatus *>(screen->pdata);
    ths->help.font = resource_font(IDR_FNT_SPECIAL);
    data_exchange_t ram_data;
    if ((ram_data.fw_update_flag & 0b00000001) == 0) {
        window_set_text(ths->help.id, _("Appendix Status:\n ok"));
    } else {
        window_set_text(ths->help.id, _("Appendix Status:\n broken"));
    }
}

int ScreenMenuAppendixStatus::CEvent(screen_t *screen, window_t *window, uint8_t event, void *param) {
    return 0;
}

screen_t screen_menu_appendix_status = {
    0,
    0,
    ScreenMenuAppendixStatus::Init,
    ScreenMenuAppendixStatus::CDone,
    ScreenMenuAppendixStatus::CDraw,
    ScreenMenuAppendixStatus::CEvent,
    sizeof(ScreenMenuAppendixStatus), //data_size
    nullptr,                    //pdata
};

screen_t *const get_scr_menu_appendix_status() { return &screen_menu_appendix_status; }




