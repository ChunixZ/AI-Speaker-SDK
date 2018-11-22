/*********************************************************************
 * Filename:       uart.h
 * Description:    This file is the uart driver header file.
 ********************************************************************/

#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------
 *							Includes
 * -------------------------------------------------------------------
 */
#include <stdint.h>
#ifndef ARM_ANDROID
#include <sys/shm.h>
#else
#include <stddef.h>
#include <linux/shm.h>
#endif


//#define dbg_print   printf
//#define debug_print   printf

/* -------------------------------------------------------------------
 *							Constants
 * -------------------------------------------------------------------
 */
/* Message Command IDs */

/* -------------------------------------------------------------------
 *							Typedefs
 * -------------------------------------------------------------------
 */

extern sem_t zap_shm_sem;
/* -------------------------------------------------------------------
 *							Global Variables
 * -------------------------------------------------------------------
 */

/* -------------------------------------------------------------------
 *							Functions
 * -------------------------------------------------------------------
 */

/*********************************************************************
 * @fn      uartOpen
 *
 * @brief   opens the serial port to the CC253x.
 *
 * input parameters
 *
 * @param   devicePath - path to the UART device
 *
 * output parameters
 *
 * None.
 *
 * @return  serialPortFd - serial port fd.
 *********************************************************************
 */
int32_t uartOpen(int8_t *devicePath);

/*********************************************************************
 * @fn      uartClose
 *
 * @brief   close the serial port to the CC253x.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return  None.
 *********************************************************************
 */
void uartClose(void);

/**************************************************************************************************
 * @fn          uartSend
 *
 * @brief       This function sends an RPC message buffer created with zap_msg_allocate().
 *
 * input parameters
 *.
 * @param       pBuf - The buffer pointer returned by zap_msg_allocate().
 *
 * output parameters
 *
 * None.
 *
 * @return      SUCCESS or FAILURE.
 **************************************************************************************************
 */
uint16_t uartSend(uint8_t  *pBuf,uint16_t len);

int uartB115200(void);

int uartB38400(void);

#ifdef __cplusplus
}
#endif

#endif /* _UART_H_ */
