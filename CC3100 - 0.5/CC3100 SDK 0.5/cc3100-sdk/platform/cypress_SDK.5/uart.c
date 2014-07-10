/*
 * uart.h - msp430f5529 launchpad uart interface implementation
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#ifdef SL_IF_TYPE_UART
  
#include "datatypes.h"
#include "simplelink.h"
#include "board.h"
#include "UARTM.h"
#include <project.h>
    
#if(UARTM_INTERNAL_CLOCK_USED)
    #include "UARTM_IntClock.h"
#endif /* End UARTM_INTERNAL_CLOCK_USED */

extern BOOLEAN IntIsMasked;

int uart_Close(Fd_t fd)
{
    UARTM_Stop();
    
    /* Disable WLAN Interrupt ... */
    CC3100_InterruptDisable();

    return 0;
}


Fd_t uart_Open(char *ifName, unsigned long flags)
{
    UARTM_Start();
    
    /* 1 ms delay */
    CyDelay(1u);

    /* Enable WLAN interrupt */
    CC3100_InterruptEnable();
    
    /* Clear Tx Rx Buffers */
    UARTM_ClearTxBuffer();
    UARTM_ClearRxBuffer();

    return NONOS_RET_OK;
}

int uart_Read(Fd_t fd, unsigned char *pBuff, int len)
{
    int i=0;
    uint8 ch;

    /* Disable Rx Interrupt */
    Rx_ISR_Disable();
    
    /* wait till all remaining bytes are received */
    for(i=0; i < len; i++ )
    {
       while(UARTM_ReadRxStatus() == 0);
    
       ch = UARTM_GetByte();
       pBuff[i] = ch;
    }
    
    /* Enable Rx Interrupt */
    Rx_ISR_Enable();

    return len;
}


int uart_Write(Fd_t fd, unsigned char *pBuff, int len)
{
    int len_to_return = len;
    CyDelay(10u);

    while (len)
    {
        UARTM_PutChar(*pBuff);
        len--;
        pBuff++;
    }
    return len_to_return;
}
#endif /* SL_IF_TYPE_UART */
