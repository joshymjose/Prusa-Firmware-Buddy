//guiconfig.h - guiapi configuration file
#ifndef _GUICONFIG_H
#define _GUICONFIG_H

#include "../guiapi/include/display.h"

//--------------------------------------
//GUI configuration
#define GUI_USE_RTOS
#define GUI_JOGWHEEL_SUPPORT
#define GUI_WINDOW_SUPPORT

//--------------------------------------
//FreeRTOS Signals

//redraw (gui thread is waiting for this signal, window_0.draw is called)
#define GUI_SIG_REDRAW 0x0001

//st7789v - spi DMA transmit complete (triggered from callback, gui thread is waiting for this signal)
#define ST7789V_SIG_SPI_TX 0x0008
#define ILI9488_SIG_SPI_TX 0x0008

#define MENU_TIMEOUT_MS 30000
//--------------------------------------
#ifdef USE_ST7789
//ST7789v configuration
#define ST7789V_USE_RTOS
#define ST7789V_PNG_SUPPORT
#endif

#ifdef USE_ILI9488
//ILI9488 configuration
#define ILI9488_USE_RTOS
#define ILI9488_PNG_SUPPORT
#endif


#endif //_GUICONFIG_H
