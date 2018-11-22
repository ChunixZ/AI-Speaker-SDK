/*************************************************************************

    > File Name: zigbeezap.h
    > Author: lunan
    > Mail: 6616@shuncom.com 
    > Created Time: 2016年02月27日 星期六 13时11分38秒
 ************************************************************************/

#ifndef _ZIGBEEZAP_H_
#define _ZIGBEEZAP_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern int zigbeezap(char *uartpath);

extern uint16_t zb_uart_send(uint8_t *data,uint16_t len);

#ifdef __cplusplus
}
#endif
#endif
