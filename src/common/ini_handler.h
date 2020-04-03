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
#ifdef ENABLE_DNS
    _NETVAR_DNS1_IP4,
    _NETVAR_DNS2_IP4,
#endif
    _NETVAR_CONNECT_IP4,
    _NETVAR_CONNECT_PORT,
} _NETVAR_t;
#define _NETVAR_MSK(n_id) ((uint16_t)1 << (n_id))
#define _NETVAR_STATIC_LAN_ADDRS \
    (_NETVAR_MSK(_NETVAR_LAN_IP4_ADDR) | _NETVAR_MSK(_NETVAR_LAN_IP4_MSK) | _NETVAR_MSK(_NETVAR_LAN_IP4_GW))
typedef struct {
    char token[CONNECT_TOKEN_SIZE + 1];
    uint32_t port;
    ip4_addr_t ip4;
} connect_t; 

typedef struct {
    ip4_addr_t addr_ip4;
    ip4_addr_t msk_ip4;
    ip4_addr_t gw_ip4;
} lan_t;

typedef struct {
    uint8_t lan_flag;
    char hostname[LAN_HOSTNAME_MAX_LEN + 1];
#ifdef BUDDY_ENABLE_CONNECT
    connect_t connect;
#endif // BUDDY_ENABLE_CONNECT
    lan_t lan;
    uint16_t set_flag;
#ifdef ENABLE_DNS
    ip4_addr_t dns1_ip4;
    ip4_addr_t dns2_ip4;
#endif // ENABLE_DNS
} _networkconfig_t;

#endif //INI_HANDLER_H
