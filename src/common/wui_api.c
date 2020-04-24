/*
 * wui_api.h
 * \brief   interface functions for Web User Interface library
 *
 *  Created on: April 22, 2020
 *      Author: joshy <joshymjose[at]gmail.com>
 */
#include "wui_api.h"

#include "ini_handler.h"
#include "eeprom.h"
#include "string.h"

static int ini_handler_func(void *user, const char *section, const char *name, const char *value) {

    ETH_config_t * tmp_config = (ETH_config_t *)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("lan_ip4", "type")) {
        if (strncmp(value, "DHCP", 4) == 0 || strncmp(value, "dhcp", 4) == 0) {
            CHANGE_LAN_TO_DHCP(tmp_config->lan.flag);
            tmp_config->set_flag |= ETHVAR_MSK(ETHVAR_LAN_FLAGS);
        } else if (strncmp(value, "STATIC", 6) == 0 || strncmp(value, "static", 6) == 0) {
            CHANGE_LAN_TO_STATIC(tmp_config->lan.flag);
            tmp_config->set_flag |= ETHVAR_MSK(ETHVAR_LAN_FLAGS);
        }
    } else if (MATCH("lan_ip4", "hostname")) {
        strlcpy(tmp_config->hostname, value, ETH_HOSTNAME_LEN + 1);
        tmp_config->hostname[LAN_HOSTNAME_MAX_LEN] = '\0';
        tmp_config->set_flag |= ETHVAR_MSK(ETHVAR_HOSTNAME);
    } else if (MATCH("lan_ip4", "address")) {
        if (ip4addr_aton(value, &tmp_config->lan.addr_ip4)) {
            tmp_config->set_flag |= ETHVAR_MSK(ETHVAR_LAN_ADDR_IP4);
        }
    } else if (MATCH("lan_ip4", "mask")) {
        if (ip4addr_aton(value, &tmp_config->lan.msk_ip4)) {
            tmp_config->set_flag |= ETHVAR_MSK(ETHVAR_LAN_MSK_IP4);
        }
    } else if (MATCH("lan_ip4", "gateway")) {
        if (ip4addr_aton(value, &tmp_config->lan.gw_ip4)) {
            tmp_config->set_flag |= ETHVAR_MSK(ETHVAR_LAN_GW_IP4);
        }
    } else if (MATCH("connect", "address")) {
        if (ip4addr_aton(value, &tmp_config->connect.ip4)) {
            tmp_config->set_flag |= ETHVAR_MSK(ETHVAR_CONNECT_IP4);
        }
    } else if (MATCH("connect", "token")) {
        strlcpy(tmp_config->connect.token, value, CONNECT_TOKEN_LEN + 1);
        tmp_config->set_flag |= ETHVAR_MSK(ETHVAR_CONNECT_TOKEN);
    } else {
        return 0; /* unknown section/name, error */
    }
    return 1;
}

static ini_handler wui_ini_handler = ini_handler_func;

void load_ini_params(ETH_config_t * config) {
    ini_load_file(wui_ini_handler, (void *)config);
}

uint32_t save_eth_params(ETH_config_t *config) {

    // type=STATIC/DHCP is in INI file
    if (config->set_flag & ETHVAR_MSK(ETHVAR_LAN_FLAGS)) {
        // if lan type is set to STATIC
        if (IS_LAN_STATIC(config->lan.flag) && (config->set_flag & ETHVAR_STATIC_LAN_ADDRS)) {
            uint8_t lan_flags = eeprom_get_var(EEVAR_LAN_FLAG).ui8;
            CHANGE_LAN_TO_STATIC(lan_flags);
            eeprom_set_var(EEVAR_LAN_FLAG, variant8_ui8(lan_flags));
            eeprom_set_var(EEVAR_LAN_IP4_ADDR,
                variant8_ui32(config->lan.addr_ip4.addr));
            eeprom_set_var(EEVAR_LAN_IP4_MSK,
                variant8_ui32(config->lan.msk_ip4.addr));
            eeprom_set_var(EEVAR_LAN_IP4_GW,
                variant8_ui32(config->lan.gw_ip4.addr));
        } else {
            uint8_t lan_flags = eeprom_get_var(EEVAR_LAN_FLAG).ui8;
            CHANGE_LAN_TO_DHCP(lan_flags);
            eeprom_set_var(EEVAR_LAN_FLAG, variant8_ui8(lan_flags));
        }
    }

    if (config->set_flag & ETHVAR_MSK(ETHVAR_HOSTNAME)) {
        variant8_t hostname = variant8_pchar(config->hostname, 0, 0);
        eeprom_set_var(EEVAR_LAN_HOSTNAME, hostname);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }

    if (config->set_flag & ETHVAR_MSK(ETHVAR_CONNECT_TOKEN)) {
        variant8_t token = variant8_pchar(config->connect.token, 0, 0);
        eeprom_set_var(EEVAR_CONNECT_TOKEN, token);
        //variant8_done() is not called, variant_pchar with init flag 0 doesnt hold its memory
    }
    if (config->set_flag & ETHVAR_MSK(ETHVAR_CONNECT_IP4)) {
        eeprom_set_var(EEVAR_CONNECT_IP4, variant8_ui32(config->connect.ip4.addr));
    }

    return 0;
}
