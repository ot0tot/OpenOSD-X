
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "main.h"
#include "uart_dma.h"
#include "vtx.h"
#include "cli.h"


typedef void cliCommandFn(const char* name, char *cmdline);
typedef struct {
    const char *name;
    const char *description;
    const char *args;
    cliCommandFn *cliCommand;
} clicmd_t;

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))
#define CLI_COMMAND_DEF(name, description, args, cliCommand) \
{ \
    name , \
    description , \
    args , \
    cliCommand \
}

const char climsg_prompt[] = "\r\n> ";

uint8_t cli_buff[256];
uint16_t cli_buff_len = 0;


bool cliisspace(uint8_t c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

int strncasecmp(const char * s1, const char * s2, size_t n)
{
    const unsigned char * ucs1 = (const unsigned char *) s1;
    const unsigned char * ucs2 = (const unsigned char *) s2;

    int d = 0;

    for ( ; n != 0; n--) {
        const int c1 = tolower(*ucs1++);
        const int c2 = tolower(*ucs2++);
        if (((d = c1 - c2) != 0) || (c2 == '\0')) {
            break;
        }
    }

    return d;
}
char *strcasestr(const char *haystack, const char *needle)
{
    int nLen = strlen(needle);
    do {
        if (!strncasecmp(haystack, needle, nLen)) {
            return (char *)haystack;
        }
        haystack++;
    } while (*haystack);
    return NULL;
}

static bool isEmpty(const char *string)
{
    return (string == NULL || *string == '\0') ? true : false;
}

static void cliPrintf(const char *format, ...)
{
    va_list va;
    va_start(va, format);

    static char buff[256];
    uint16_t len = vsnprintf(buff, sizeof(buff), format, va);
    len = len < sizeof(buff) ? len : sizeof(buff);
    uart_send((uint8_t*)buff,len);

    va_end(va);
}


static void cliStatus(const char *cmdName, char *cmdline)
{
    UNUSED(cmdName);
    UNUSED(cmdline);
    cliPrintf("version x.xx.xx\r\n");
}

static void cliVtxtest(const char *cmdName, char *cmdline)
{
    UNUSED(cmdName);

    int freq, vpd;
    if (sscanf(cmdline, "%d %d", &freq, &vpd) == 2){
        setVtx_vpd(freq, vpd);
        cliPrintf("freq:%d vpd:%d\r\n", freq, vpd);
    }else{
        cliPrintf("ERROR\r\n");
    }
}


static void cliOsdEnable(const char *cmdName, char *cmdline)
{
    UNUSED(cmdName);

    int en;
    if (sscanf(cmdline, "%d", &en) == 1){
        if (en == 0 || en == 1){
            osd_enable((uint8_t)en);
            cliPrintf("osd:%d\r\n", en);
        }else{
            cliPrintf("ERROR\r\n");
        }
    }else{
        cliPrintf("ERROR\r\n");
    }
}


static void cliRebootDfu(const char *cmdName, char *cmdline)
{
    UNUSED(cmdName);
    UNUSED(cmdline);

    rebootDfu();
}


static void cliHelp(const char *cmdName, char *cmdline);
const clicmd_t cmdTable[] = {
    CLI_COMMAND_DEF("status", "show status", NULL, cliStatus),
    CLI_COMMAND_DEF("vtx_set", "set vtx params", "<freq-MHz> <vpd-mV>", cliVtxtest),
    CLI_COMMAND_DEF("osd_en", "osd enable", "<0:off/1:on>", cliOsdEnable),
    CLI_COMMAND_DEF("reboot_dfu", "reboot and dfu", "", cliRebootDfu),
    CLI_COMMAND_DEF("help", "display command help", "[search string]", cliHelp)
};

static void cliHelp(const char *cmdName, char *cmdline)
{
    UNUSED(cmdName);
    bool anyMatches = false;

    for (uint32_t i = 0; i < ARRAYLEN(cmdTable); i++) {
        bool printEntry = false;
        if (isEmpty(cmdline)) {
            printEntry = true;
        } else {
            if (strcasestr(cmdTable[i].name, cmdline)
                || strcasestr(cmdTable[i].description, cmdline)
               ) {
                printEntry = true;
            }
        }

        if (printEntry) {
            anyMatches = true;
            uart_send((uint8_t*)cmdTable[i].name, strlen(cmdTable[i].name));
            if (cmdTable[i].description) {
                uart_send( (uint8_t*)" - ", 3);
                uart_send( (uint8_t*)cmdTable[i].description, strlen(cmdTable[i].description));
            }
            if (cmdTable[i].args) {
                uart_send((uint8_t*)"\r\n\t", 3);
                uart_send((uint8_t*)cmdTable[i].args, strlen(cmdTable[i].args));
            }
            uart_send((uint8_t*)"\r\n", 2);
        }
    }
    if (!isEmpty(cmdline) && !anyMatches) {
        cliPrintf("ERROR NO MATCHES FOR '%s'\r\n", cmdline);
    }
}


int32_t startCli(void)
{
    cliPrintf("\r\n\r\nOpenOSD-X CLI\r\n");
    cliPrintf(climsg_prompt);
    cli_buff_len = 0;
    return 0;
}

int32_t dataCli(uint8_t rxdata)
{
    if ( rxdata == '\r' || rxdata == '\n'){
        uart_send((uint8_t*)"\r\n", 2);
        cli_buff[cli_buff_len] = 0;
        const clicmd_t *cmd;
        char *options=NULL;
        for (cmd = cmdTable; cmd < cmdTable + ARRAYLEN(cmdTable); cmd++) {
            if (!strncasecmp((char*)cli_buff, cmd->name, strlen(cmd->name))   // command names match
                && (cliisspace(cli_buff[strlen(cmd->name)]) || cli_buff[strlen(cmd->name)] == 0)) {
                options = (char*)&cli_buff[strlen(cmd->name)+1];
                break;
            }
        }
        if (cmd < cmdTable + ARRAYLEN(cmdTable)) {
            cmd->cliCommand(cmd->name, options);
        } else {
            cliPrintf("ERROR\r\n");
        }
        cliPrintf(climsg_prompt);
        cli_buff_len = 0;
    }else if( rxdata == 0x8){
        // bs
        if (cli_buff_len > 0){
            cli_buff_len--;
            cliPrintf("\010 \010");
        }
    }else if( rxdata >= 0x20 && rxdata <= 0x7e){
        cli_buff[cli_buff_len++] = rxdata;
        uart_send(&rxdata, 1);
        if (cli_buff_len  >= sizeof(cli_buff)){
            cliPrintf("ERROR\r\n");
            cliPrintf(climsg_prompt);
            cli_buff_len = 0;
        }
    }
    return 0;
}



