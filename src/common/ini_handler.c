// INI file handler (ini_handler.c)

#include "ini_handler.h"
#include "ff.h"
#include "eeprom.h"
#include "lwip/dhcp.h"
#include "lwip/netifapi.h"
#include "dns.h"
#include "ini.h"

#define MAX_UINT16  65535
static const char network_ini_file_name[] = "/lan_settings.ini"; //change -> change msgboxes in screen_lan_settings

static int ini_config_handler(void *user, const char *section, const char *name, const char *value);

uint8_t ini_save_file(const char *ini_save_str) {

    UINT ini_config_len = strlen(ini_save_str);
    UINT written_bytes = 0;
    FIL ini_file;

    f_unlink(network_ini_file_name);

    uint8_t i = f_open(&ini_file, network_ini_file_name, FA_WRITE | FA_CREATE_NEW);
    uint8_t w = f_write(&ini_file, ini_save_str, ini_config_len, &written_bytes);
    uint8_t c = f_close(&ini_file);

    if (i || w || c || written_bytes != ini_config_len)
        return 0;

    return 1;
}

uint8_t ini_load_file(ini_handler handler, void *user_struct) {
    UINT written_bytes = 0;
    FIL ini_file;

    uint8_t file_init = f_open(&ini_file, network_ini_file_name, FA_READ);
    uint8_t file_read = f_read(&ini_file, ini_file_str, MAX_INI_SIZE, &written_bytes);
    uint8_t file_close = f_close(&ini_file);

    if (file_init || file_read || file_close) {
        return 0;
    }

    if (ini_parse_string(ini_file_str, handler, user_struct) < 0) {
        return 0;
    }
    return 1;
}

static void _change_any_to_static(_networkconfig_t * config) {
    if (netif_is_up(&eth0)) {
        netifapi_netif_set_down(&eth0);
    }
    config->lan_flag |= LAN_EEFLG_TYPE;
    eeprom_set_var(EEVAR_LAN_FLAG, variant8_ui8(config->lan_flag));

    netifapi_netif_set_addr(&eth0,
        (const ip4_addr_t *)&(config->lan.addr_ip4),
        (const ip4_addr_t *)&(config->lan.msk_ip4),
        (const ip4_addr_t *)&(config->lan.gw_ip4)
        );
    if (netif_is_link_up(&eth0) && !(config->lan_flag & LAN_EEFLG_ONOFF)) {
        netifapi_netif_set_up(&eth0);
    }
}

static void _change_static_to_dhcp(uint8_t lan_flg) {
    if (netif_is_up(&eth0)) {
        netifapi_netif_set_down(&eth0);
    }
    lan_flg &= ~LAN_EEFLG_TYPE;
    eeprom_set_var(EEVAR_LAN_FLAG, variant8_ui8(lan_flg));
    if (netif_is_link_up(&eth0) && !(lan_flg & LAN_EEFLG_ONOFF)) {
        netifapi_netif_set_up(&eth0);
    }
}

uint8_t load_config_from_ini(void){

    _networkconfig_t tmp_config;
    tmp_config.lan_flag = eeprom_get_var(EEVAR_LAN_FLAG).ui8;
    tmp_config.set_flag = 0;

    if (ini_load_file(ini_config_handler, &tmp_config) == 0) {
        return 0;
    }

    // type=STATIC/DHCP is in INI file
    if (tmp_config.set_flag & _NETVAR_MSK(_NETVAR_LAN_FLAGS)) {
        // if lan type is set to STATIC
        if ((tmp_config.lan_flag & LAN_EEFLG_TYPE)){
            if ((tmp_config.set_flag & _NETVAR_STATIC_LAN_ADDRS) != _NETVAR_STATIC_LAN_ADDRS) {
                return 0;
            }
            eeprom_set_var(EEVAR_LAN_IP4_ADDR, variant8_ui32(tmp_config.lan.addr_ip4.addr));
            eeprom_set_var(EEVAR_LAN_IP4_MSK, variant8_ui32(tmp_config.lan.msk_ip4.addr));
            eeprom_set_var(EEVAR_LAN_IP4_GW, variant8_ui32(tmp_config.lan.gw_ip4.addr));
        }
    }
    if (tmp_config.set_flag & _NETVAR_MSK(_NETVAR_HOSTNAME)) {
        strlcpy(interface_hostname, tmp_config.hostname, LAN_HOSTNAME_MAX_LEN + 1);
        eth0.hostname = interface_hostname;
        variant8_t hostname = variant8_pchar(tmp_config.hostname, 0, 0);
        eeprom_set_var(EEVAR_LAN_HOSTNAME, hostname);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
#ifdef BUDDY_ENABLE_CONNECT
    if (tmp_config.set_flag & _NETVAR_MSK(_NETVAR_CONNECT_TOKEN)) {
        variant8_t token = variant8_pchar(tmp_config.connect.token, 0, 0);
        eeprom_set_var(EEVAR_CONNECT_TOKEN, token);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
    if (tmp_config.set_flag & _NETVAR_MSK(_NETVAR_CONNECT_IP4)) {
        eeprom_set_var(EEVAR_CONNECT_IP4, variant8_ui32(tmp_config.connect.ip4.addr));
    }
    if (tmp_config.set_flag & _NETVAR_MSK(_NETVAR_CONNECT_PORT)) {
        eeprom_set_var(EEVAR_CONNECT_PORT, variant8_ui32(tmp_config.connect.port));
    }
#endif // BUDDY_ENABLE_CONNECT

    // type=STATIC/DHCP is in INI file    
    if (tmp_config.set_flag & _NETVAR_MSK(_NETVAR_LAN_FLAGS)) {
        // if there is change from STATIC to DHCP
        if (!(tmp_config.lan_flag & LAN_EEFLG_TYPE) && 
            ((eeprom_get_var(EEVAR_LAN_FLAG).ui8 & LAN_EEFLG_TYPE) != (tmp_config.lan_flag & LAN_EEFLG_TYPE))) {
            _change_static_to_dhcp(tmp_config.lan_flag);
        } else if (tmp_config.lan_flag & LAN_EEFLG_TYPE){
            _change_any_to_static(&tmp_config);
        }
    }
    return 1;
}

static int ini_config_handler(void *user, const char *section, const char *name, const char *value) {
    _networkconfig_t *tmp_config = (_networkconfig_t *)user;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("lan_ip4", "type")) {
        if (strncmp(value, "DHCP", 4) == 0 || strncmp(value, "dhcp", 4) == 0) {
            tmp_config->lan_flag &= ~LAN_EEFLG_TYPE;
            tmp_config->set_flag |= _NETVAR_MSK(_NETVAR_LAN_FLAGS);
        } else if (strncmp(value, "STATIC", 6) == 0 || strncmp(value, "static", 6) == 0) {
            tmp_config->lan_flag |= LAN_EEFLG_TYPE;
            tmp_config->set_flag |= _NETVAR_MSK(_NETVAR_LAN_FLAGS);
        }
    } else if (MATCH("lan_ip4", "hostname")) {
        strlcpy(tmp_config->hostname, value, LAN_HOSTNAME_MAX_LEN + 1);
        tmp_config->hostname[LAN_HOSTNAME_MAX_LEN] = '\0';
        tmp_config->set_flag |= _NETVAR_MSK(_NETVAR_HOSTNAME);
    } else if (MATCH("lan_ip4", "address")) {
        if (ip4addr_aton(value, &tmp_config->lan.addr_ip4)) {
            tmp_config->set_flag |= _NETVAR_MSK(_NETVAR_LAN_IP4_ADDR);
        }
    } else if (MATCH("lan_ip4", "mask")) {
        if (ip4addr_aton(value, &tmp_config->lan.msk_ip4)) {
            tmp_config->set_flag |= _NETVAR_MSK(_NETVAR_LAN_IP4_MSK);
        }
    } else if (MATCH("lan_ip4", "gateway")) {
        if (ip4addr_aton(value, &tmp_config->lan.gw_ip4)) {
            tmp_config->set_flag |= _NETVAR_MSK(_NETVAR_LAN_IP4_GW);
        }
    }
#ifdef BUDDY_ENABLE_CONNECT
    else if (MATCH("connect", "address")) {
        if (ip4addr_aton(value, &(tmp_config->connect.ip4))) {
            tmp_config->set_flag |= _NETVAR_MSK(_NETVAR_CONNECT_IP4);
        }
    } else if (MATCH("connect", "token")) {
        strlcpy(tmp_config->connect.token, value, CONNECT_TOKEN_SIZE + 1);
        tmp_config->connect.token[CONNECT_TOKEN_SIZE] = '\0';
        tmp_config->set_flag |= _NETVAR_MSK(_NETVAR_CONNECT_TOKEN);
    } else if (MATCH("connect", "port")) {
        uint64_t val = atoi(value);
        if(val <= MAX_UINT16){
            tmp_config->connect.port = val;
            tmp_config->set_flag |= _NETVAR_MSK(_NETVAR_CONNECT_PORT);
        }
    }
#endif // BUDDY_ENABLE_CONNECT
    else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}