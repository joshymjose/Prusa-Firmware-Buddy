/*
 * wui.h
 * \brief main interface functions for Web User Interface (WUI) thread
 *
 *  Created on: Dec 12, 2019
 *      Author: joshy
 */

#include "wui.h"
#include "wui_vars.h"
#include "marlin_client.h"
#include "lwip.h"
#include "ethernetif.h"
#include "http_client.h"
#include "eeprom.h"
#include <string.h>

#define MAX_WUI_REQUEST_LEN       100
#define MAX_MARLIN_REQUEST_LEN    100
#define WUI_FLG_PEND_REQ          0x0001
//#define BUDDY_DISABLE_HTTP_CLIENT //disabling http client for next public release

osMessageQId tcpclient_wui_queue = 0; // char input queue (uint8_t)
osSemaphoreId tcpclient_wui_sema = 0; // semaphore handle
osMutexDef(wui_thread_mutex);         // Declare mutex
osMutexId(wui_thread_mutex_id);       // Mutex ID

typedef struct {
    uint32_t flags;
    marlin_vars_t *wui_marlin_vars;
    char request[MAX_WUI_REQUEST_LEN];
    uint8_t request_len;
} web_client_t;
web_vars_t web_vars;

web_client_t wui;

static void wui_queue_cycle(void);
static int process_wui_request(void);



void update_web_vars(void) {
    osMutexWait(wui_thread_mutex_id, osWaitForever);
    web_vars.pos[Z_AXIS_POS] = wui.wui_marlin_vars->pos[Z_AXIS_POS];
    web_vars.temp_nozzle = wui.wui_marlin_vars->temp_nozzle;
    web_vars.temp_bed = wui.wui_marlin_vars->temp_bed;
    web_vars.print_speed = wui.wui_marlin_vars->print_speed;
    web_vars.flow_factor = wui.wui_marlin_vars->flow_factor;
    web_vars.print_dur = wui.wui_marlin_vars->print_duration;
    web_vars.sd_precent_done = wui.wui_marlin_vars->sd_percent_done;
    web_vars.sd_printing = wui.wui_marlin_vars->sd_printing;
    if(web_vars.device_state != wui.wui_marlin_vars->device_state){
        web_vars.device_state = wui.wui_marlin_vars->device_state;
        http_client_send_message(MSG_EVENTS_STATE_CHANGED, &web_vars.device_state);
    }
    web_vars.device_state = wui.wui_marlin_vars->device_state;
    if (marlin_event(MARLIN_EVT_GFileChange)) {
        marlin_get_printing_gcode_name(web_vars.gcode_name);
    }
    osMutexRelease(wui_thread_mutex_id);
}

void StartWebServerTask(void const *argument) {
    osMessageQDef(wuiQueue, 64, uint8_t);
    tcpclient_wui_queue = osMessageCreate(osMessageQ(wuiQueue), NULL);
    osSemaphoreDef(wuiSema);
    tcpclient_wui_sema = osSemaphoreCreate(osSemaphore(wuiSema), 1);
    wui_thread_mutex_id = osMutexCreate(osMutex(wui_thread_mutex));
    wui.wui_marlin_vars = marlin_client_init(); // init the client
    if (wui.wui_marlin_vars) {
        wui.wui_marlin_vars = marlin_update_vars(MARLIN_VAR_MSK_WUI);
        update_web_vars();
    }
    wui.wui_marlin_vars->device_state = DEVICE_STATE_IDLE;
    wui.flags = wui.request_len = 0;

    MX_LWIP_Init();
    http_server_init();
    for (;;) {
        ethernetif_link(&eth0);
        wui_queue_cycle();

        if (wui.wui_marlin_vars) {
            marlin_client_loop();
            update_web_vars();
        }
#ifndef BUDDY_DISABLE_HTTP_CLIENT
        buddy_http_client_loop();
#endif
        osDelay(100);
    }
}

static void wui_queue_cycle() {
    osEvent ose;
    char ch;

    if (wui.flags & WUI_FLG_PEND_REQ) {
        if (process_wui_request()) {
            wui.flags &= ~WUI_FLG_PEND_REQ;
            wui.request_len = 0;
        }
    }

    while ((ose = osMessageGet(tcpclient_wui_queue, 0)).status == osEventMessage) {
        ch = (char)((uint8_t)(ose.value.v));
        switch (ch) {
        case '\r':
        case '\n':
            ch = 0;
            break;
        }
        if (wui.request_len < MAX_WUI_REQUEST_LEN)
            wui.request[wui.request_len++] = ch;
        else {
            //TOO LONG
            wui.request_len = 0;
        }
        if ((ch == 0) && (wui.request_len > 1)) {
            if (process_wui_request()) {
                wui.request_len = 0;
            } else {
                wui.flags |= WUI_FLG_PEND_REQ;
                break;
            }
        }
    }
}
static int process_wui_request() {

    if(strncmp(wui.request, "!cip ", 5) == 0){
        uint32_t ip;
        if(sscanf(wui.request + 5, "%lu", &ip)){
            eeprom_set_var(EEVAR_CONNECT_IP, variant8_ui32(ip));
        }
    } else if (strncmp(wui.request, "!ck ", 4) == 0){
        eeprom_set_string(EEVAR_CONNECT_KEY_START, wui.request + 4, CONNECT_SEC_KEY_LEN);
    } else if (strncmp(wui.request, "!cn ", 4) == 0){
        eeprom_set_string(EEVAR_LAN_HOSTNAME_START, wui.request + 4, LAN_HOSTNAME_MAX_LEN);
    } else {
        marlin_wui_gcode(wui.request);
    }
    return 1;
}
