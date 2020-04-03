// INI file handler (ini_handler.h)
#ifndef INI_HANDLER_H
#define INI_HANDLER_H

#include <stdint.h>
#include "lwip.h"
#define MAX_INI_SIZE 200

typedef int (*ini_handler)(void* user, const char* section,
                           const char* name, const char* value);

uint8_t ini_save_file(const char *ini_save_str);
uint8_t ini_load_file(ini_handler handler, void *user_struct);
uint8_t load_config_from_ini(void);
char ini_file_str[MAX_INI_SIZE];

typedef enum{
    _NETVAR_LAN_FLAGS,
    _NETVAR_HOSTNAME,
    _NETVAR_CONNECT_TOKEN,
    _NETVAR_LAN_IP4_ADDR,
    _NETVAR_LAN_IP4_MSK,
    _NETVAR_LAN_IP4_GW,
    _NETVAR_DNS1,
    _NETVAR_DNS2,
    _NETVAR_CONNECT_IP4,
} _NETVAR_t;
#define _NETVAR_MSK(n_id) ((uint16_t)1 << (n_id))
#define _NETVAR_STATIC_LAN_ADDRS \
    (_NETVAR_MSK(_NETVAR_LAN_IP4_ADDR) | _NETVAR_MSK(_NETVAR_LAN_IP4_MSK) | _NETVAR_MSK(_NETVAR_LAN_IP4_GW))

typedef struct {
    uint8_t lan_flag;
    char hostname[LAN_HOSTNAME_MAX_LEN + 1];
#ifdef BUDDY_ENABLE_CONNECT
    char connect_token[CONNECT_TOKEN_SIZE + 1];
    ip4_addr_t connect_ip4;
#endif // BUDDY_ENABLE_CONNECT
    ip4_addr_t lan_ip4_addr;
    ip4_addr_t lan_ip4_msk;
    ip4_addr_t lan_ip4_gw;
    uint16_t set_flag;
    ip4_addr_t dns1_ip4;
    ip4_addr_t dns2_ip4;
} _networkconfig_t;

#endif //INI_HANDLER_H
