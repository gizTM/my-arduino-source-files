/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2015/1/23, v1.0 create this file.
*******************************************************************************/

#include "osapi.h"
#include "at_custom.h"
#include "user_interface.h"

#include "ip_addr.h"
#include "sntp.h"

#include "time.h"

char * ntp_servers[] = {
    "1.europe.pool.ntp.org",
    "2.europe.pool.ntp.org",
    "time-c.nist.gov"
};
#define NTP_OFFSET 0 // GMT

char * weekday[] = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"
};
//AT+NTP - Init NTP
//AT+NTP? - Ask NTP Time

void ICACHE_FLASH_ATTR
at_exeCmdNTP(uint8_t id)
{
    uint8 buffer[255] = {0};
    sntp_stop();
    int i;
    uint32 tm = 0;
    int ntp_servers_len = sizeof(ntp_servers) / sizeof(ntp_servers[0]);
    for (i = 0; i < 3; i++) {
	if (i >= ntp_servers_len) break;
	sntp_setservername(i, ntp_servers[i]);
    }
    sntp_set_timezone(NTP_OFFSET);
    sntp_init();
    os_sprintf(buffer, "NTP init\r\n");
    at_port_print(buffer);
    at_response_ok();
}



void ICACHE_FLASH_ATTR
at_queryCmdNTP(uint8_t id)
{
    uint8 buffer[255] = {0};
    time_t tim = (time_t)sntp_get_current_timestamp();
    struct tm *t;
    if (tim > 0) {
	t = localtime(&tim);
	os_sprintf(buffer, "NTP Time: %02d:%02d:%02d %02d.%02d.%04d %s (%d)sec\r\n", t->tm_hour,t->tm_min, t->tm_sec, t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, weekday[t->tm_wday], tim);
	at_port_print(buffer);
	at_response_ok();
    } else {
	os_sprintf(buffer, "NTP Error: (0)\r\n");
	at_port_print(buffer);
	at_response_error();
    }
}

extern void at_exeCmdCiupdate(uint8_t id);
at_funcationType at_custom_cmd[] = {
    {"+NTP", 4, NULL, at_queryCmdNTP, NULL, at_exeCmdNTP},
#ifdef AT_UPGRADE_SUPPORT
    {"+CIUPDATE", 9,       NULL,            NULL,            NULL, at_exeCmdCiupdate}
#endif
};

void user_rf_pre_init(void)
{
}

void user_init(void)
{
    char buf[64] = {0};
    at_customLinkMax = 5;
    at_init();
    os_sprintf(buf,"eMax' NTP Version:%s %s",__DATE__,__TIME__);
    at_set_custom_info(buf);
    at_port_print("\r\nready\r\n");
    at_cmd_array_regist(&at_custom_cmd[0], sizeof(at_custom_cmd)/sizeof(at_custom_cmd[0]));
}
