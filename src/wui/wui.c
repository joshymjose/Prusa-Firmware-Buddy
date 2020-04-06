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
#include "ini_handler.h"

#define MAX_WUI_REQUEST_LEN    100
#define MAX_MARLIN_REQUEST_LEN 100
#define WUI_FLG_PEND_REQ       0x0001
#define TCP_WUI_QUEUE_SIZE     64

osMessageQId tcp_wui_queue_id = 0;
osSemaphoreId tcp_wui_semaphore_id = 0;
osMutexDef(wui_thread_mutex);   // Mutex object for exchanging WUI thread TCP thread
osMutexId(wui_thread_mutex_id); // Mutex ID

typedef struct {
    uint32_t flags;
    char request[MAX_WUI_REQUEST_LEN];
    uint8_t request_len;
} wui_web_request_t;

static wui_web_request_t wui_req;
static marlin_vars_t *wui_marlin_vars;
wui_vars_t wui_vars; // global vriable for data relevant to WUI

static void wui_queue_cycle(void);
static int process_wui_request(void);

#if 0
static void device_state_change() {

    if (marlin_event(MARLIN_EVT_DevStateChange)) {
        wui_vars.device_state = wui_marlin_vars->device_state;
        switch (wui_vars.device_state) {
        case DEVICE_STATE_IDLE:

            break;
        case DEVICE_STATE_ERROR:

        case DEVICE_STATE_PRINTING:

            break;
        case DEVICE_STATE_PAUSED:

            break;
        case DEVICE_STATE_FINISHED:

            break;
        default:

            break;
        }
    }
}
#endif
void update_wui_vars(void) {
    osMutexWait(wui_thread_mutex_id, osWaitForever);
    wui_vars.pos[Z_AXIS_POS] = wui_marlin_vars->pos[Z_AXIS_POS];
    wui_vars.temp_nozzle = wui_marlin_vars->temp_nozzle;
    wui_vars.temp_bed = wui_marlin_vars->temp_bed;
    wui_vars.print_speed = wui_marlin_vars->print_speed;
    wui_vars.flow_factor = wui_marlin_vars->flow_factor;
    wui_vars.print_dur = wui_marlin_vars->print_duration;
    wui_vars.sd_precent_done = wui_marlin_vars->sd_percent_done;
    wui_vars.sd_printing = wui_marlin_vars->sd_printing;

    if (marlin_event(MARLIN_EVT_GFileChange)) {
        marlin_get_printing_gcode_name(wui_vars.gcode_name);
    }

    osMutexRelease(wui_thread_mutex_id);
}

void StartWebServerTask(void const *argument) {
    // message queue for commands from tcp thread to wui main loop
    osMessageQDef(tcp_wui_queue, TCP_WUI_QUEUE_SIZE, uint32_t);
    tcp_wui_queue_id = osMessageCreate(osMessageQ(tcp_wui_queue), NULL);
    // semaphore for filling tcp - wui message qeue
    osSemaphoreDef(wuiSema);
    tcp_wui_semaphore_id = osSemaphoreCreate(osSemaphore(wuiSema), 1);
    // mutex for passing marlin variables to tcp thread
    wui_thread_mutex_id = osMutexCreate(osMutex(wui_thread_mutex));
    // marlin client initialization
    wui_marlin_vars = marlin_client_init(); // init the client
    if (wui_marlin_vars) {
        wui_marlin_vars = marlin_update_vars(MARLIN_VAR_MSK_WUI);
        update_wui_vars();
    }
    wui_marlin_vars->device_state = DEVICE_STATE_IDLE;
    wui_req.flags = wui_req.request_len = 0;

    MX_LWIP_Init();
    if(load_config_from_ini()){
        //SUCCESS
    }
    http_server_init();
#ifdef BUDDY_ENABLE_CONNECT
    buddy_httpc_handler_init();
#endif // BUDDY_ENABLE_CONNECT
    for (;;) {
        ethernetif_link(&eth0);
        wui_queue_cycle();

        if (wui_marlin_vars) {
            marlin_client_loop();
            update_wui_vars();
        }
#ifdef BUDDY_ENABLE_CONNECT
        buddy_httpc_handler();
#endif // BUDDY_ENABLE_CONNECT
        osDelay(100);
    }
}

static void wui_queue_cycle() {
    osEvent ose;
    char ch;

    if (wui_req.flags & WUI_FLG_PEND_REQ) {
        if (process_wui_request()) {
            wui_req.flags &= ~WUI_FLG_PEND_REQ;
            wui_req.request_len = 0;
        }
    }

    while ((ose = osMessageGet(tcp_wui_queue_id, 0)).status == osEventMessage) {
        ch = (char)((uint8_t)(ose.value.v));
        switch (ch) {
        case '\r':
        case '\n':
            ch = 0;
            break;
        }
        if (wui_req.request_len < MAX_WUI_REQUEST_LEN)
            wui_req.request[wui_req.request_len++] = ch;
        else {
            //TOO LONG
            wui_req.request_len = 0;
        }
        if ((ch == 0) && (wui_req.request_len > 1)) {
            if (process_wui_request()) {
                wui_req.request_len = 0;
            } else {
                wui_req.flags |= WUI_FLG_PEND_REQ;
                break;
            }
        }
    }
}
static int process_wui_request() {

    if (strncmp(wui_req.request, "!cip ", 5) == 0) {
        uint32_t ip;
        if (sscanf(wui_req.request + 5, "%lu", &ip)) {
            eeprom_set_var(EEVAR_CONNECT_IP4, variant8_ui32(ip));
        }
    } else if (strncmp(wui_req.request, "!ck ", 4) == 0) {
        variant8_t token = variant8_pchar(wui_req.request + 4, 0, 0);
        eeprom_set_var(EEVAR_CONNECT_TOKEN, token);
        //variant8_done() is not called because variant_pchar with init flag 0 doesnt hold its memory
    } else if (strncmp(wui_req.request, "!cn ", 4) == 0) {
        variant8_t hostname = variant8_pchar(wui_req.request + 4, 0, 0);
        eeprom_set_var(EEVAR_LAN_HOSTNAME, hostname);
        //variant8_done() is not called because variant_pchar with init flag 0 doesnt hold its memory
    } else {
        marlin_wui_gcode(wui_req.request);
    }
    return 1;
}
