// eeprom.c

#include "eeprom.h"
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "st25dv64k.h"
#include "dbg.h"
#include "cmsis_os.h"
#include "ff.h"
#include "tm_stm32f4_crc.h"




#define EEPROM_VARCOUNT  (sizeof(eeprom_map) / sizeof(eeprom_entry_t))
#define EEPROM_DATASIZE  sizeof(eeprom_vars_t)
#define EEPROM__PADDING  1

#pragma pack(push)
#pragma pack(1)

// eeprom map entry structure
typedef struct _eeprom_entry_t {
    uint8_t type;   // variant8 data type
    uint8_t count;  // number of elements
} eeprom_entry_t;

// eeprom vars structure (used for defaults)
typedef struct _eeprom_vars_t {
    uint16_t VERSION;
    uint16_t FEATURES;
    uint16_t DATASIZE;
    uint16_t FWVERSION;
    uint16_t FWBUILD;
    uint8_t FILAMENT_TYPE;
    uint32_t FILAMENT_COLOR;
    uint8_t RUN_SELFTEST;
    uint8_t RUN_XYZCALIB;
    uint8_t RUN_FIRSTLAY;
    uint8_t FSENSOR_ENABLED;
    float ZOFFSET;
    float PID_NOZ_P;
    float PID_NOZ_I;
    float PID_NOZ_D;
    float PID_BED_P;
    float PID_BED_I;
    float PID_BED_D;
    uint8_t LAN_FLAG;
    uint32_t LAN_IP4_ADDR;
    uint32_t LAN_IP4_MSK;
    uint32_t LAN_IP4_GW;
    uint32_t LAN_IP4_DNS1;
    uint32_t LAN_IP4_DNS2;
    char LAN_HOSTNAME[LAN_HOSTNAME_MAX_LEN + 1];
    char TEST[10];
    char _PADDING[EEPROM__PADDING];
    uint32_t CRC32;
} eeprom_vars_t;

#pragma pack(pop)


// eeprom map
const eeprom_entry_t eeprom_map[] = {
    { VARIANT8_UI16,  1  }, // EEVAR_VERSION
    { VARIANT8_UI16,  1  }, // EEVAR_FEATURES
    { VARIANT8_UI16,  1  }, // EEVAR_DATASIZE
    { VARIANT8_UI16,  1  }, // EEVAR_FW_VERSION
    { VARIANT8_UI16,  1  }, // EEVAR_FW_BUILD
    { VARIANT8_UI8,   1  }, // EEVAR_FILAMENT_TYPE
    { VARIANT8_UI32,  1  }, // EEVAR_FILAMENT_COLOR
    { VARIANT8_UI8,   1  }, // EEVAR_RUN_SELFTEST
    { VARIANT8_UI8,   1  }, // EEVAR_RUN_XYZCALIB
    { VARIANT8_UI8,   1  }, // EEVAR_RUN_FIRSTLAY
    { VARIANT8_UI8,   1  }, // EEVAR_FSENSOR_ENABLED
    { VARIANT8_FLT,   1  }, // EEVAR_ZOFFSET
    { VARIANT8_FLT,   1  }, // EEVAR_PID_NOZ_P
    { VARIANT8_FLT,   1  }, // EEVAR_PID_NOZ_I
    { VARIANT8_FLT,   1  }, // EEVAR_PID_NOZ_D
    { VARIANT8_FLT,   1  }, // EEVAR_PID_BED_P
    { VARIANT8_FLT,   1  }, // EEVAR_PID_BED_I
    { VARIANT8_FLT,   1  }, // EEVAR_PID_BED_D
    { VARIANT8_UI8,   1  }, // EEVAR_LAN_FLAG
    { VARIANT8_UI32,  1  }, // EEVAR_LAN_IP4_ADDR
    { VARIANT8_UI32,  1  }, // EEVAR_LAN_IP4_MSK
    { VARIANT8_UI32,  1  }, // EEVAR_LAN_IP4_GW
    { VARIANT8_UI32,  1  }, // EEVAR_LAN_IP4_DNS1
    { VARIANT8_UI32,  1  }, // EEVAR_LAN_IP4_DNS2
    { VARIANT8_PCHAR, LAN_HOSTNAME_MAX_LEN + 1 }, // EEVAR_LAN_HOSTNAME
    { VARIANT8_PCHAR, 10 }, // EEVAR_TEST
    { VARIANT8_PCHAR, EEPROM__PADDING }, // EEVAR__PADDING32
    { VARIANT8_UI32,  1  }, // EEVAR_CRC32
};

// eeprom variable names
const char *eeprom_var_name[] = {
    "VERSION",
    "FEATURES",
    "DATASIZE",
    "FW_VERSION",
    "FW_BUILD",
    "FILAMENT_TYPE",
    "FILAMENT_COLOR",
    "RUN_SELFTEST",
    "RUN_XYZCALIB",
    "RUN_FIRSTLAY",
    "FSENSOR_ENABLED",
    "ZOFFSET",
    "PID_NOZ_P",
    "PID_NOZ_I",
    "PID_NOZ_D",
    "PID_BED_P",
    "PID_BED_I",
    "PID_BED_D",
    "LAN_FLAG",
    "LAN_IP4_ADDR",
    "LAN_IP4_MSK",
    "LAN_IP4_GW",
    "LAN_IP4_DNS1",
    "LAN_IP4_DNS2",
    "LAN_HOSTNAME",
    "TEST",
    "_PADDING",
    "CRC32",
};

// eeprom variable defaults
const eeprom_vars_t eeprom_var_defaults = {
    EEPROM_VERSION,  // EEVAR_VERSION
    EEPROM_FEATURES, // EEVAR_FEATURES
    EEPROM_DATASIZE, // EEVAR_DATASIZE
    0,               // EEVAR_FW_VERSION
    0,               // EEVAR_FW_BUILD
    0,               // EEVAR_FILAMENT_TYPE
    0,               // EEVAR_FILAMENT_COLOR
    1,               // EEVAR_RUN_SELFTEST
    1,               // EEVAR_RUN_XYZCALIB
    1,               // EEVAR_RUN_FIRSTLAY
    1,               // EEVAR_FSENSOR_ENABLED
    0,               // EEVAR_ZOFFSET
    18.000000,       // EEVAR_PID_NOZ_P
    0.294400,        // EEVAR_PID_NOZ_I
    274.437500,      // EEVAR_PID_NOZ_D
    160.970001,      // EEVAR_PID_BED_P
    2.251200,        // EEVAR_PID_BED_I
    2877.437744,     // EEVAR_PID_BED_D
    0,               // EEVAR_LAN_FLAG
    0,               // EEVAR_LAN_IP4_ADDR
    0,               // EEVAR_LAN_IP4_MSK
    0,               // EEVAR_LAN_IP4_GW
    0,               // EEVAR_LAN_IP4_DNS1
    0,               // EEVAR_LAN_IP4_DNS2
    "PrusaMINI",     // EEVAR_LAN_HOSTNAME
    "TEST",          // EEVAR_TEST
    "",              // EEVAR__PADDING
    0xffffffff,      // EEVAR_CRC32
};


// semaphore handle (lock/unlock)
osSemaphoreId eeprom_sema = 0;


// forward declarations of private functions

uint16_t eeprom_var_size(uint8_t id);
uint16_t eeprom_var_addr(uint8_t id);
void eeprom_dump(void);
int eeprom_var_format(char *str, unsigned int size, uint8_t id, variant8_t var);
void eeprom_print_vars(void);
int eeprom_convert_from_v2(void);
int eeprom_convert_from(void);

void eeprom_lock(void);
void eeprom_unlock(void);

uint32_t eeprom_calc_crc32(uint32_t* p, uint16_t n);

int eeprom_check_crc32(void);
void eeprom_update_crc32(uint16_t addr, uint16_t size);

// public functions - described in header

uint8_t eeprom_init(void) {
    uint8_t ret = 0;
    int crcOK;
    st25dv64k_init();
    //eeprom_clear();
    //eeprom_dump();
    //osDelay(2000);
    //eeprom_save_bin_to_usb("eeprom.bin");
#if 0 // crc test
    uint32_t data[] = {0x01020304, 0x02030405, 0x03040506, 0x04050607, 0 };
    uint32_t crc = eeprom_calc_crc32(data, 4);
    data[4] = crc;
    crc = eeprom_calc_crc32(data, 5);
#endif
    crcOK = eeprom_check_crc32();
    if (crcOK) { // TODO: simplify this logic
		uint16_t version = eeprom_get_var(EEVAR_VERSION).ui16;
		if (version != EEPROM_VERSION) {
			if (version == 2) {
				if (eeprom_convert_from_v2() == 0)
					ret = 1;
			}
			else {
				if (eeprom_convert_from() == 0)
					ret = 1;
			}
		}
		else {
			uint16_t features = eeprom_get_var(EEVAR_FEATURES).ui16;
			if (features != EEPROM_FEATURES)
				if (eeprom_convert_from() == 0)
					ret = 1;
		}
    }
	else
		ret = 1;
    if (ret)
        eeprom_defaults();
    eeprom_print_vars();
    //eeprom_dump();
    return ret;
}

void eeprom_defaults(void) {
    eeprom_vars_t vars = eeprom_var_defaults;
    eeprom_lock();
    // calculate crc32
    vars.CRC32 = eeprom_calc_crc32((uint32_t*)(&vars), (EEPROM_DATASIZE - 4) / 4);
    // write data to eeprom
    st25dv64k_user_write_bytes(EEPROM_ADDRESS, (void*)&vars, EEPROM_DATASIZE);
    eeprom_unlock();
}

variant8_t eeprom_get_var(uint8_t id) {
    uint16_t addr;
    uint16_t size;
    uint16_t data_size;
    void* data_ptr;
    variant8_t var = variant8_empty();
    if (id < EEPROM_VARCOUNT) {
        eeprom_lock();
        var = variant8_init(eeprom_map[id].type, eeprom_map[id].count, 0);
        size = eeprom_var_size(id);
        data_size = variant8_data_size(&var);
        if (size == data_size)
        {
            addr = eeprom_var_addr(id);
            data_ptr = variant8_data_ptr(&var);
            st25dv64k_user_read_bytes(addr, data_ptr, size);
        }
        else
        {
            _dbg("error");
            //TODO:error
        }
        eeprom_unlock();
    }
    return var;
}

void eeprom_set_var(uint8_t id, variant8_t var) {
    uint16_t addr;
    uint16_t size;
    uint16_t data_size;
    void* data_ptr;
/*    uint32_t crc;
    uint32_t crc2;
    uint32_t crc3;
    uint32_t data_crc[8];
    uint16_t addr_crc;
    uint16_t count_crc;*/
    if (id < EEPROM_VARCOUNT) {
        eeprom_lock();
        if (var.type == eeprom_map[id].type) {
            size = eeprom_var_size(id);
            data_size = variant8_data_size(&var);
            if (size == data_size) {
                addr = eeprom_var_addr(id);
                data_ptr = variant8_data_ptr(&var);
/*
                st25dv64k_user_read_bytes(EEPROM_ADDRESS + EEPROM_DATASIZE - 4, &crc, 4);

                addr_crc = addr & 0xfffc;
                count_crc = (size + addr_crc - addr + 2) / 4;
                st25dv64k_user_read_bytes(addr_crc, data_crc, count_crc * 4);
                data_crc[count_crc] = crc;
                crc2 = eeprom_calc_crc32(data_crc, count_crc + 1);
*/
                st25dv64k_user_write_bytes(addr, data_ptr, size);
                eeprom_update_crc32(addr, size);
/*
                st25dv64k_user_read_bytes(addr_crc, data_crc, count_crc * 4);
                data_crc[count_crc] = crc2;
                crc3 = eeprom_calc_crc32(data_crc, count_crc + 1);

                st25dv64k_user_write_bytes(EEPROM_ADDRESS + EEPROM_DATASIZE - 4, &crc3, 4);
*/
            }
            else {
                // TODO: error
            }
        }
        else {
            // TODO: error
        }
        eeprom_unlock();
    }
}

void eeprom_clear(void) {
    uint16_t a;
    uint32_t data = 0xffffffff;
    eeprom_lock();
    for (a = 0x0000; a < 0x0800; a += 4)
        st25dv64k_user_write_bytes(a, &data, 4);
    eeprom_unlock();
}


// private functions

uint16_t eeprom_var_size(uint8_t id) {
    if (id < EEPROM_VARCOUNT)
        return variant8_type_size(eeprom_map[id].type & ~VARIANT8_PTR) * eeprom_map[id].count;
    return 0;
}

uint16_t eeprom_var_addr(uint8_t id) {
    uint16_t addr = EEPROM_ADDRESS;
    while (id)
        addr += eeprom_var_size(--id);
    return addr;
}

void eeprom_dump(void) {
    int i;
    int j;
    uint8_t b;
    char line[64];
    for (i = 0; i < 128; i++) // 128 lines = 2048 bytes
    {
        sprintf(line, "%04x", i * 16);
        for (j = 0; j < 16; j++) {
            b = st25dv64k_user_read(j + i * 16);
            sprintf(line + 4 + j * 3, " %02x", b);
        }
        _dbg("%s", line);
    }
}

int eeprom_var_format(char *str, unsigned int size, uint8_t id, variant8_t var) {
    int n = 0;
    switch (id) {
    // ip addresses
    case EEVAR_LAN_IP4_ADDR:
    case EEVAR_LAN_IP4_MSK:
    case EEVAR_LAN_IP4_GW:
    case EEVAR_LAN_IP4_DNS1:
    case EEVAR_LAN_IP4_DNS2: {
            uint8_t* p = (uint8_t*)(&(var.ui32));
            n = snprintf(str, size, "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
        }
        break;
    default: //use default conversion
        n = variant8_snprintf(str, size, 0, &var);
        break;
    }
    return n;
}

void eeprom_print_vars(void) {
    uint8_t id;
    char text[128];
    variant8_t var8;
    for (id = 0; id < EEPROM_VARCOUNT; id++) {
        var8 = eeprom_get_var(id);
        eeprom_var_format(text, 128, id, var8);
        _dbg("%s=%s", eeprom_var_name[id], text);
        variant8_done(&var8);
    }
}


#define ADDR_V2_FILAMENT_TYPE  0x0400
#define ADDR_V2_FILAMENT_COLOR EEPROM_ADDRESS + 3
#define ADDR_V2_RUN_SELFTEST   EEPROM_ADDRESS + 19
#define ADDR_V2_ZOFFSET        0x010e
#define ADDR_V2_PID_NOZ_P      0x019d
#define ADDR_V2_PID_BED_P      0x01af

// conversion function for old version 2 format (marlin eeprom)
int eeprom_convert_from_v2(void) {
    eeprom_vars_t vars = eeprom_var_defaults;
    // read FILAMENT_TYPE (uint8_t)
    st25dv64k_user_read_bytes(ADDR_V2_FILAMENT_TYPE, &(vars.FILAMENT_TYPE), sizeof(uint8_t));
    // initialize to zero, maybe not necessary
    if (vars.FILAMENT_TYPE == 0xff) vars.FILAMENT_TYPE = 0;
    // read FILAMENT_COLOR (uint32_t)
    st25dv64k_user_read_bytes(ADDR_V2_FILAMENT_COLOR, &(vars.FILAMENT_COLOR), sizeof(uint32_t));
    // read RUN_SELFTEST & RUN_XYZCALIB & RUN_FIRSTLAY & FSENSOR_ENABLED (4x uint8_t)
    st25dv64k_user_read_bytes(ADDR_V2_RUN_SELFTEST, &(vars.RUN_SELFTEST), 4 * sizeof(uint8_t));
    // read ZOFFSET (float)
    st25dv64k_user_read_bytes(ADDR_V2_ZOFFSET, &(vars.ZOFFSET), sizeof(float));
    // check ZOFFSET valid range, cancel conversion if not of valid range (defaults will be loaded)
    if ((vars.ZOFFSET < -2) || (vars.ZOFFSET > 0)) return 0;
    // read PID_NOZ_P & PID_NOZ_I & PID_NOZ_D (3x float)
    st25dv64k_user_read_bytes(ADDR_V2_PID_NOZ_P, &(vars.PID_NOZ_P), 3 * sizeof(float));
    // TODO: validate nozzle PID constants
    // read PID_BED_P & PID_BED_I & PID_BED_D (3x float)
    st25dv64k_user_read_bytes(ADDR_V2_PID_BED_P, &(vars.PID_BED_P), 3 * sizeof(float));
    // TODO: validate bed PID constants
    // calculate crc32
    vars.CRC32 = eeprom_calc_crc32((uint32_t*)(&vars), (EEPROM_DATASIZE - 4) / 4);
    // write data to eeprom
    st25dv64k_user_write_bytes(EEPROM_ADDRESS, (void*)&vars, EEPROM_DATASIZE);
    return 1;
}

// conversion function for new version format (features, firmware version/build)
int eeprom_convert_from(void) {
    return 0;
}

void eeprom_lock(void) {
    if (eeprom_sema == 0) {
        osSemaphoreDef(eepromSema);
        eeprom_sema = osSemaphoreCreate(osSemaphore(eepromSema), 1);
    }
    osSemaphoreWait(eeprom_sema, osWaitForever);
}

void eeprom_unlock(void) {
    osSemaphoreRelease(eeprom_sema);
}

// crc32 calculation function
uint32_t eeprom_calc_crc32(uint32_t* p, uint16_t n)
{
#if 0 // XOR32 instead of CRC32 (for debugging)
    uint32_t crc = 0;
    while (n--)
        crc ^= *(p++);
#else // hardware CRC32
    // TODO: semaphore and separate module for HW CRC32, because it will be shared  (in this time used just in QR codes)
    //
    TM_CRC_Init(); // it seems that is no problem to call this every time before using TM_CRC_Calculate32
    uint32_t crc = TM_CRC_Calculate32(p, n, 1);
#endif
    return crc;
}

// version independent crc32 check
int eeprom_check_crc32(void)
{
    uint16_t version;
    uint16_t datasize;
    uint32_t crc;
//    uint16_t addr;
//    uint16_t n;
    version = eeprom_get_var(EEVAR_VERSION).ui16;
    if (version <= 3)
        return 1; // version <= 3 is without crc, return 1 means crc OK, because import from v2 has additional check/validation
    datasize = eeprom_get_var(EEVAR_DATASIZE).ui16;
    st25dv64k_user_read_bytes(EEPROM_ADDRESS + EEPROM_DATASIZE - 4, &crc, 4);
#if 1 //simple method
    uint8_t data[256];
    uint32_t crc2;
    st25dv64k_user_read_bytes(EEPROM_ADDRESS, data, datasize);
    crc2 = eeprom_calc_crc32((uint32_t*)data, (datasize - 4) / 4);
    return (crc == crc2)?1:0;
#else //
    uint32_t data[2] = {0, crc};
    n = (datasize - 4) / 4;
    addr = EEPROM_ADDRESS;
    while (n--)
    {
        st25dv64k_user_read_bytes(addr, &(data[0]), 4);
        data[1] = eeprom_calc_crc32(data, 2);
        addr += 4;
    }
    return (data[1] == 0)?1:0;
#endif
}

void eeprom_update_crc32(uint16_t addr, uint16_t size)
{
#if 1 //simple method
    eeprom_vars_t vars;
    // read eeprom data
    st25dv64k_user_read_bytes(EEPROM_ADDRESS, (void*)&vars, EEPROM_DATASIZE);
    // calculate crc32
    vars.CRC32 = eeprom_calc_crc32((uint32_t*)(&vars), (EEPROM_DATASIZE - 4) / 4);
    // write crc to eeprom
    st25dv64k_user_write_bytes(EEPROM_ADDRESS + EEPROM_DATASIZE - 4, &(vars.CRC32), 4);
#else //
#endif
}

// public functions for load/save

int eeprom_load_bin_from_usb(const char* fn)
{
    FIL fil;
    uint8_t buff[128];
    uint16_t size = 0x800;
    uint16_t addr = 0x000;
    UINT bs = sizeof(buff);
    UINT br;
    if (f_open(&fil, fn, FA_READ) == FR_OK) {
        eeprom_lock();
        while (size)
        {
            if (size < bs)
                bs = size;
            if (f_read(&fil, buff, bs, &br) != FR_OK)
                break;
            if (br != bs)
                break;
            st25dv64k_user_write_bytes(addr, buff, bs);
            size -= bs;
            addr += bs;
        }
        f_close(&fil);
        eeprom_unlock();
        return (size == 0)?1:0;
    }
    return 0;
}

int eeprom_save_bin_to_usb(const char* fn)
{
    FIL fil;
    uint8_t buff[128];
    uint16_t size = 0x800;
    uint16_t addr = 0x000;
    UINT bs = sizeof(buff);
    UINT bw;
    if (f_open(&fil, fn, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
        eeprom_lock();
        while (size)
        {
            if (size < bs)
                bs = size;
            st25dv64k_user_read_bytes(addr, buff, bs);
            if (f_write(&fil, buff, bs, &bw) != FR_OK)
                break;
            if (bw != bs)
                break;
            size -= bs;
            addr += bs;
        }
        f_close(&fil);
        eeprom_unlock();
        return (size == 0)?1:0;
    }
    return 0;
}

int eeprom_load_xml_from_usb(const char* fn)
{
    return 0;
}

int eeprom_save_xml_to_usb(const char* fn)
{
    FIL fil;
    uint8_t id;
    char text[128];
    variant8_t var8;
    UINT bw;
    if (f_open(&fil, fn, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
        f_write(&fil, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n", 55, &bw);
        f_write(&fil, "<eeprom>\n", 9, &bw);
        for (id = 0; id < EEPROM_VARCOUNT; id++) {
            var8 = eeprom_get_var(id);
            *text = 0;
            eeprom_var_format(text, 128, id, var8);
            f_write(&fil, "  <variable id=\"", 16, &bw);
            f_write(&fil, eeprom_var_name[id], strlen(eeprom_var_name[id]), &bw);
            f_write(&fil, "\" value=\"", 9, &bw);
            f_write(&fil, text, strlen(text), &bw);
            f_write(&fil, "\"/>\n", 4, &bw);
            variant8_done(&var8);
        }
        f_write(&fil, "</eeprom>\n", 10, &bw);
        f_close(&fil);
        return 1;
    }
    return 0;
}



int8_t eeprom_test_PUT(const unsigned int bytes) {
    unsigned int i;
    char line[16] = "abcdefghijklmno";
    char line2[16];
    uint8_t size = sizeof(line);
    unsigned int count = bytes / 16;

    for (i = 0; i < count; i++) {
        st25dv64k_user_write_bytes(EEPROM_ADDRESS + i * size, &line, size);
    }

    int8_t res_flag = 1;

    for (i = 0; i < count; i++) {
        st25dv64k_user_read_bytes(EEPROM_ADDRESS + i * size, &line2, size);
        if (strcmp(line2, line))
            res_flag = 0;
    }
    return res_flag;
}


