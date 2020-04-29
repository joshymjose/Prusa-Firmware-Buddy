#ifndef NETIF_SETTINGS_H
#define NETIF_SETTINGS_H

#include "ip_addr.h"

#define LAN_FLAG_ONOFF_POS      (1 << 0)    // position of ONOFF switch in lan.flag
#define LAN_FLAG_TYPE_POS       (1 << 1)    // position of DHCP/STATIC switch in lan.flag

#define IS_LAN_OFF(flg)         (flg & LAN_FLAG_ONOFF_POS)           // returns true if flag is set to OFF
#define IS_LAN_ON(flg)          (!IS_LAN_OFF(flg))              // returns true if flag is set to ON
#define IS_LAN_STATIC(flg)      (flg & LAN_FLAG_TYPE_POS)            // returns true if flag is set to STATIC
#define IS_LAN_DHCP(flg)        (!IS_LAN_STATIC(flg))           // returns true if flag is set to DHCP

#define CHANGE_LAN_TO_STATIC(flg)   (flg |= LAN_FLAG_TYPE_POS)      // flip lan type flg to STATIC
#define CHANGE_LAN_TO_DHCP(flg)     (flg &= ~LAN_FLAG_TYPE_POS)     // flip lan type flg to DHCP
#define TURN_LAN_ON(flg)            (flg &= ~LAN_FLAG_ONOFF_POS)    // flip lan switch flg to ON
#define TURN_LAN_OFF(flg)           (flg |= LAN_FLAG_ONOFF_POS)     // flip lan switch flg to OFF

#define ETH_HOSTNAME_LEN        20          // ethernet hostname MAX length
#define CONNECT_TOKEN_LEN       20          // CONNECT security token length

typedef enum {
    ETH_UNLINKED,       // ETH cabel is unlinked
    ETH_NETIF_DOWN,     // ETH interface is DOWN
    ETH_NETIF_UP,       // ETH interface is UP
} ETH_STATUS_t;

typedef struct {
    char token[CONNECT_TOKEN_LEN + 1];  // security token: 20 chars
    ip4_addr_t ip4;                     // user defined CONNECT ip4
    uint16_t port;                      // user defined CONNECT port
} connect_t; 

typedef struct {
    uint8_t flag;                       // lan flags: pos0 = switch(ON=0, OFF=1), pos1 = type(DHCP=0, STATIC=1)
    ip4_addr_t addr_ip4;                // user defined static ip4 address
    ip4_addr_t msk_ip4;                 // user defined ip4 netmask
    ip4_addr_t gw_ip4;                  // user define ip4 default gateway
} lan_t;

typedef struct {
    char hostname[ETH_HOSTNAME_LEN + 1];    // ETH hostname: MAX 20 chars
    connect_t connect;                      // user defined CONNECT configurations
    lan_t lan;                              // user defined CONNECT configurations
    uint32_t var_mask;                      // mask for setting ethvars
} ETH_config_t;

extern char eth_hostname[ETH_HOSTNAME_LEN + 1];     // static string storing ethernet hostname
#endif //NETIF_SETTINGS_H