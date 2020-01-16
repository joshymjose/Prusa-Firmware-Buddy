//resource.c - generated file - do not edit!

#include "gui.h"
#include "config.h"
#include "res/cc/font_7x13.c" //IDR_FNT_SMALL
#include "res/cc/font_11x18.c" //IDR_FNT_NORMAL
#include "res/cc/font_12x21.c" //IDR_FNT_BIG
#include "res/cc/font_9x15.c" //IDR_FNT_TERMINAL
#include "res/cc/font_9x16.c" //IDR_FNT_SPECIAL
#include "res/cc/font_original_prusa.c" //IDR_FNT_original_prusa

#include "res/cc/png_splashscreen_logo_marlin.c" //IDR_PNG_splash_logo_marlin
#if (PRINTER_TYPE == PRINTER_PRUSA_MK3)
    #ifdef _DEBUG
        #include "res/cc/png_statusscreen_logo_prusamk3_debug.c" //IDR_PNG_status_logo_prusa_prn
    #else //_DEBUG
        #include "res/cc/png_statusscreen_logo_prusamk3.c" //IDR_PNG_status_logo_prusa_prn
    #endif //_DEBUG
#elif (PRINTER_TYPE == PRINTER_PRUSA_MINI)
    #ifdef _DEBUG
        #include "res/cc/png_statusscreen_logo_prusamini_debug.c" //IDR_PNG_status_logo_prusa_prn
    #else //_DEBUG
        #include "res/cc/png_statusscreen_logo_prusamini.c" //IDR_PNG_status_logo_prusa_prn
    #endif //_DEBUG
#elif (PRINTER_TYPE == PRINTER_PRUSA_XL)
    #ifdef _DEBUG
        #include "res/cc/png_statusscreen_logo_prusaxl_debug.c" //IDR_PNG_status_logo_prusa_prn
    #else //_DEBUG
        #include "res/cc/png_statusscreen_logo_prusaxl.c" //IDR_PNG_status_logo_prusa_prn
    #endif //_DEBUG
#elif (PRINTER_TYPE == PRINTER_PRUSA_iXL)
    #ifdef _DEBUG
        #include "res/cc/png_statusscreen_logo_prusaxl_debug.c" //IDR_PNG_status_logo_prusa_prn
    #else //_DEBUG
        #include "res/cc/png_statusscreen_logo_prusaxl.c" //IDR_PNG_status_logo_prusa_prn
    #endif //_DEBUG
#elif (PRINTER_TYPE == PRINTER_PRUSA_MK4)
    #ifdef _DEBUG
        #include "res/cc/png_statusscreen_logo_prusamk4_debug.c" //IDR_PNG_status_logo_prusa_prn
    #else //_DEBUG
        #include "res/cc/png_statusscreen_logo_prusamk4.c" //IDR_PNG_status_logo_prusa_prn
    #endif //_DEBUG
#endif // PRINTER_TYPE

#include "res/cc/png_statusscreen_icon_nozzle.c" //IDR_PNG_status_icon_nozzle
#include "res/cc/png_statusscreen_icon_heatbed.c" //IDR_PNG_status_icon_heatbed
#include "res/cc/png_statusscreen_icon_prnspeed.c" //IDR_PNG_status_icon_prnspeed
#include "res/cc/png_statusscreen_icon_filament.c" //IDR_PNG_status_icon_filament
#include "res/cc/png_statusscreen_icon_z_axis.c" //IDR_PNG_status_icon_z_axis
#include "res/cc/png_statusscreen_icon_home.c" //IDR_PNG_status_icon_home
#include "res/cc/png_statusscreen_icon_printing.c" //IDR_PNG_status_icon_printing

#include "res/cc/png_menu_icon_print.c" //IDR_PNG_menu_icon_print
#include "res/cc/png_menu_icon_preheat.c" //IDR_PNG_menu_icon_preheat
#include "res/cc/png_menu_icon_spool.c" //IDR_PNG_menu_icon_spool
#include "res/cc/png_menu_icon_reprint.c" //IDR_PNG_menu_icon_reprint
#include "res/cc/png_menu_icon_calibration.c" //IDR_PNG_menu_icon_calibration
#include "res/cc/png_menu_icon_settings.c" //IDR_PNG_menu_icon_settings
#include "res/cc/png_menu_icon_info.c" //IDR_PNG_menu_icon_info
#include "res/cc/png_menu_icon_pause.c" //IDR_PNG_menu_icon_pause
#include "res/cc/png_menu_icon_stop.c" //IDR_PNG_menu_icon_stop
#include "res/cc/png_menu_icon_resume.c" //IDR_PNG_menu_icon_resume
#include "res/cc/png_menu_icon_home.c" //IDR_PNG_menu_icon_home
#include "res/cc/png_menu_icon_back.c" //IDR_PNG_menu_icon_back

#include "res/cc/png_filescreen_icon_home.c" //IDR_PNG_filescreen_icon_home
#include "res/cc/png_filescreen_icon_folder.c" //IDR_PNG_filescreen_icon_folder
#include "res/cc/png_filescreen_icon_up_folder.c" //IDR_PNG_filescreen_icon_up_folder

#include "res/cc/png_icon_pepa.c" //IDR_PNG_icon_pepa

#include "res/cc/png_msgbox_icon_error.c" //IDR_PNG_msgbox_icon_error
#include "res/cc/png_msgbox_icon_question.c" //IDR_PNG_msgbox_icon_question
#include "res/cc/png_msgbox_icon_warning.c" //IDR_PNG_msgbox_icon_warning
#include "res/cc/png_msgbox_icon_info.c" //IDR_PNG_msgbox_icon_info

#include "res/cc/png_wizard_icon_na.c" //IDR_PNG_wizard_icon_na
#include "res/cc/png_wizard_icon_ok.c" //IDR_PNG_wizard_icon_ok
#include "res/cc/png_wizard_icon_ng.c" //IDR_PNG_wizard_icon_ng
#include "res/cc/png_wizard_icon_ip0.c" //IDR_PNG_wizard_icon_ip0
#include "res/cc/png_wizard_icon_ip1.c" //IDR_PNG_wizard_icon_ip1
#include "res/cc/png_wizard_icon_hourglass.c" //IDR_PNG_wizard_icon_hourglass
#include "res/cc/png_wizard_icon_autohome.c" //IDR_PNG_wizard_icon_autohome
#include "res/cc/png_wizard_icon_search.c" //IDR_PNG_wizard_icon_search
#include "res/cc/png_wizard_icon_measure.c" //IDR_PNG_wizard_icon_measure

#include "res/cc/png_icon_pepa_psod.c" //IDR_PNG_icon_pepa_psod

#include "res/cc/png_header_icon_usb.c" //IDR_PNG_header_icon_usb
#include "res/cc/png_header_icon_lan.c" //IDR_PNG_header_icon_lan
#include "res/cc/png_header_icon_wifi.c" //IDR_PNG_header_icon_wifi
#include "res/cc/png_header_icon_info.c" //IDR_PNG_header_icon_info
#include "res/cc/png_header_icon_error.c" //IDR_PNG_header_icon_error
#include "res/cc/png_header_icon_warning.c" //IDR_PNG_header_icon_warning
#include "res/cc/png_header_icon_question.c" //IDR_PNG_header_icon_question

#include "res/cc/png_arrow_up.c" //IDR_PNG_arrow_up
#include "res/cc/png_arrow_down.c" //IDR_PNG_arrow_down

RESOURCE_TABLE_BEGIN
RESOURCE_ENTRY_NUL() //IDR_NULL
//fonts
RESOURCE_ENTRY_FNT(font_7x13) //IDR_FNT_SMALL
RESOURCE_ENTRY_FNT(font_11x18) //IDR_FNT_NORMAL
RESOURCE_ENTRY_FNT(font_12x21) //IDR_FNT_BIG
RESOURCE_ENTRY_FNT(font_9x15) //IDR_FNT_TERMINAL
RESOURCE_ENTRY_FNT(font_9x16) //IDR_FNT_SPECIAL
RESOURCE_ENTRY_FNT(font_original_prusa)

//pngs
RESOURCE_ENTRY_PNG(png_splashscreen_logo_marlin) //IDR_PNG_splash_logo_marlin
RESOURCE_ENTRY_PNG(png_statusscreen_logo_prusa_prn) //IDR_PNG_status_logo_prusa_prn

RESOURCE_ENTRY_PNG(png_statusscreen_icon_nozzle)
RESOURCE_ENTRY_PNG(png_statusscreen_icon_heatbed)
RESOURCE_ENTRY_PNG(png_statusscreen_icon_prnspeed)
RESOURCE_ENTRY_PNG(png_statusscreen_icon_filament)
RESOURCE_ENTRY_PNG(png_statusscreen_icon_z_axis)
RESOURCE_ENTRY_PNG(png_statusscreen_icon_home)
RESOURCE_ENTRY_PNG(png_statusscreen_icon_printing)

RESOURCE_ENTRY_PNG(png_menu_icon_print)
RESOURCE_ENTRY_PNG(png_menu_icon_preheat)
RESOURCE_ENTRY_PNG(png_menu_icon_spool)
RESOURCE_ENTRY_PNG(png_menu_icon_reprint)
RESOURCE_ENTRY_PNG(png_menu_icon_calibration)
RESOURCE_ENTRY_PNG(png_menu_icon_settings)
RESOURCE_ENTRY_PNG(png_menu_icon_info)
RESOURCE_ENTRY_PNG(png_menu_icon_pause)
RESOURCE_ENTRY_PNG(png_menu_icon_stop)
RESOURCE_ENTRY_PNG(png_menu_icon_resume)
RESOURCE_ENTRY_PNG(png_menu_icon_home)

RESOURCE_ENTRY_PNG(png_filescreen_icon_home)
RESOURCE_ENTRY_PNG(png_filescreen_icon_folder)
RESOURCE_ENTRY_PNG(png_filescreen_icon_up_folder)

RESOURCE_ENTRY_PNG(png_icon_pepa)

RESOURCE_ENTRY_PNG(png_msgbox_icon_error)
RESOURCE_ENTRY_PNG(png_msgbox_icon_question)
RESOURCE_ENTRY_PNG(png_msgbox_icon_warning)
RESOURCE_ENTRY_PNG(png_msgbox_icon_info)

RESOURCE_ENTRY_PNG(png_wizard_icon_na)
RESOURCE_ENTRY_PNG(png_wizard_icon_ok)
RESOURCE_ENTRY_PNG(png_wizard_icon_ng)
RESOURCE_ENTRY_PNG(png_wizard_icon_ip0)
RESOURCE_ENTRY_PNG(png_wizard_icon_ip1)
RESOURCE_ENTRY_PNG(png_wizard_icon_hourglass)
RESOURCE_ENTRY_PNG(png_wizard_icon_autohome)
RESOURCE_ENTRY_PNG(png_wizard_icon_search)
RESOURCE_ENTRY_PNG(png_wizard_icon_measure)

RESOURCE_ENTRY_PNG(png_icon_pepa_psod)

RESOURCE_ENTRY_PNG(png_header_icon_usb)
RESOURCE_ENTRY_PNG(png_header_icon_lan)
RESOURCE_ENTRY_PNG(png_header_icon_wifi)
RESOURCE_ENTRY_PNG(png_header_icon_info)
RESOURCE_ENTRY_PNG(png_header_icon_error)
RESOURCE_ENTRY_PNG(png_header_icon_warning)
RESOURCE_ENTRY_PNG(png_header_icon_question)

RESOURCE_ENTRY_PNG(png_arrow_up)
RESOURCE_ENTRY_PNG(png_arrow_down)

RESOURCE_ENTRY_PNG(png_menu_icon_back)
RESOURCE_TABLE_END
