/*
 * board.c - msp430f5529 launchpad configuration
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

#include "simplelink.h"
#include "board.h"
#include<project.h>

#ifdef SL_IF_TYPE_UART
#include "UARTM.h"
#endif

#define PMM_STATUS_ERROR  1
#define PMM_STATUS_OK     0
#define XT1HFOFFG   0

P_EVENT_HANDLER                pIrqEventHandler = 0;

BOOLEAN IntIsMasked;

#ifdef SL_IF_TYPE_UART
#define ASSERT_UART(expr) {  if (!(expr)) { while(1) ;}}

unsigned char error_overrun = FALSE;
_uartFlowctrl uartFlowctrl;
_uartFlowctrl *puartFlowctrl = &uartFlowctrl;
#endif

int registerInterruptHandler(P_EVENT_HANDLER InterruptHdl , void* pValue)
{
    pIrqEventHandler = InterruptHdl;

    return 0;
}

void CC3100_disable()
{
    nHIB_Write(0);
}


void CC3100_enable()
{
    nHIB_Write(1);
}

void CC3100_InterruptEnable(void)
{
    /* Configure and enable interrupt */
#ifndef SL_IF_TYPE_UART
    isr_1_Start();
#endif

#ifdef SL_IF_TYPE_UART
    Rx_ISR_Start();
#endif
    
#if(UARTM_RX_ENABLED || UARTM_HD_ENABLED)
    #if(UARTM_RX_INTERRUPT_ENABLED)
       UARTM_EnableRxInt();
    #endif
#endif

    CyGlobalIntEnable;
}

void CC3100_InterruptDisable()
{
     /* Configure and enable interrupt */
#ifndef SL_IF_TYPE_UART
    isr_1_Disable();
#endif

#ifdef SL_IF_TYPE_UART
    Rx_ISR_Disable();
#endif
    
#if(UARTM_RX_ENABLED || UARTM_HD_ENABLED)
    #if(UARTM_RX_INTERRUPT_ENABLED)
        UARTM_DisableRxInt();
    #endif
#endif

    CyGlobalIntDisable;
}

void MaskIntHdlr()
{
    IntIsMasked = TRUE;
}

void UnMaskIntHdlr()
{
    IntIsMasked = FALSE;
}

void initClk(void)
{
    CySysClkImoStart();
    CySysClkWriteImoFreq(24u);
        
    /* Globally enable interrupts */
    CyGlobalIntEnable;
}

void stopWDT()
{
    CySysWdtDisable(1u);
    CySysWdtDisable(2u);
    CySysWdtDisable(3u);
}

void initLEDs()
{
    turnLedOff(LED1);
    turnLedOff(LED2);
}

/* BLUE LED - LED1
   GREEN LED - LED2 */

void turnLedOn(char ledNum)
{
    switch(ledNum)
    {
      case LED1:
        LED_BLUE_Write(0);
        break;
      case LED2:
        LED_GREEN_Write(0);
        break;
    }

}

void turnLedOff(char ledNum)
{
    switch(ledNum)
    {
      case LED1:
        LED_BLUE_Write(1);
        break;
      case LED2:
        LED_GREEN_Write(1);
        break;
    }  
}

void toggleLed(char ledNum)
{
    switch(ledNum)
    {
      case LED1:
        LED_BLUE_Write(~LED_BLUE_Read());
        break;
      case LED2:
        LED_GREEN_Write(~LED_GREEN_Read());
        break;
    }

}

unsigned char GetLEDStatus()
{
  unsigned char status = 0;
    
  if(LED_BLUE_Read() && LED_GREEN_Read())
    status = 4;
  if(LED_BLUE_Read())
    status = 1;
    //status |= (1 << 0);
  if(LED_GREEN_Read())
    status = 2;
    //status |= (1 << 1);

  return status;
}

void initAntSelGPIO()
{
}

void SelAntenna(int antenna)
{
}

void Delay(unsigned long interval)
{
    while(interval > 0)
    {
        CyDelay(1000u);
        interval--;
    }
}