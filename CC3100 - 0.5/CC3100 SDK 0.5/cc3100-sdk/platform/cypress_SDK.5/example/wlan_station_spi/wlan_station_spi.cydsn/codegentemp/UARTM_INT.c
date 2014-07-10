/*******************************************************************************
* File Name: UARTM_INT.c
* Version 2.30
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
* Note:
*  Any unusual or non-standard behavior should be noted here. Other-
*  wise, this section should remain blank.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UARTM.h"
#include "CyLib.h"


/***************************************
* Custom Declratations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if( (UARTM_RX_ENABLED || UARTM_HD_ENABLED) && \
     (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH))


    /*******************************************************************************
    * Function Name: UARTM_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_rxBuffer - RAM buffer pointer for save received data.
    *  UARTM_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  UARTM_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  UARTM_rxBufferOverflow - software overflow flag. Set to one
    *     when UARTM_rxBufferWrite index overtakes
    *     UARTM_rxBufferRead index.
    *  UARTM_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when UARTM_rxBufferWrite is equal to
    *    UARTM_rxBufferRead
    *  UARTM_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  UARTM_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(UARTM_RXISR)
    {
        uint8 readData;
        uint8 increment_pointer = 0u;
        #if(CY_PSOC3)
            uint8 int_en;
        #endif /* CY_PSOC3 */

        /* User code required at start of ISR */
        /* `#START UARTM_RXISR_START` */

        /* `#END` */

        #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
            int_en = EA;
            CyGlobalIntEnable;
        #endif /* CY_PSOC3 */

        readData = UARTM_RXSTATUS_REG;

        if((readData & (UARTM_RX_STS_BREAK | UARTM_RX_STS_PAR_ERROR |
                        UARTM_RX_STS_STOP_ERROR | UARTM_RX_STS_OVERRUN)) != 0u)
        {
            /* ERROR handling. */
            /* `#START UARTM_RXISR_ERROR` */

            /* `#END` */
        }

        while((readData & UARTM_RX_STS_FIFO_NOTEMPTY) != 0u)
        {

            #if (UARTM_RXHW_ADDRESS_ENABLED)
                if(UARTM_rxAddressMode == (uint8)UARTM__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readData & UARTM_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readData & UARTM_RX_STS_ADDR_MATCH) != 0u)
                        {
                            UARTM_rxAddressDetected = 1u;
                        }
                        else
                        {
                            UARTM_rxAddressDetected = 0u;
                        }
                    }

                    readData = UARTM_RXDATA_REG;
                    if(UARTM_rxAddressDetected != 0u)
                    {   /* store only addressed data */
                        UARTM_rxBuffer[UARTM_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* without software addressing */
                {
                    UARTM_rxBuffer[UARTM_rxBufferWrite] = UARTM_RXDATA_REG;
                    increment_pointer = 1u;
                }
            #else  /* without addressing */
                UARTM_rxBuffer[UARTM_rxBufferWrite] = UARTM_RXDATA_REG;
                increment_pointer = 1u;
            #endif /* End SW_DETECT_TO_BUFFER */

            /* do not increment buffer pointer when skip not adderessed data */
            if( increment_pointer != 0u )
            {
                if(UARTM_rxBufferLoopDetect != 0u)
                {   /* Set Software Buffer status Overflow */
                    UARTM_rxBufferOverflow = 1u;
                }
                /* Set next pointer. */
                UARTM_rxBufferWrite++;

                /* Check pointer for a loop condition */
                if(UARTM_rxBufferWrite >= UARTM_RXBUFFERSIZE)
                {
                    UARTM_rxBufferWrite = 0u;
                }
                /* Detect pre-overload condition and set flag */
                if(UARTM_rxBufferWrite == UARTM_rxBufferRead)
                {
                    UARTM_rxBufferLoopDetect = 1u;
                    /* When Hardware Flow Control selected */
                    #if(UARTM_FLOW_CONTROL != 0u)
                    /* Disable RX interrupt mask, it will be enabled when user read data from the buffer using APIs */
                        UARTM_RXSTATUS_MASK_REG  &= (uint8)~UARTM_RX_STS_FIFO_NOTEMPTY;
                        CyIntClearPending(UARTM_RX_VECT_NUM);
                        break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                    #endif /* End UARTM_FLOW_CONTROL != 0 */
                }
            }

            /* Check again if there is data. */
            readData = UARTM_RXSTATUS_REG;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START UARTM_RXISR_END` */

        /* `#END` */

        #if(CY_PSOC3)
            EA = int_en;
        #endif /* CY_PSOC3 */

    }

#endif /* End UARTM_RX_ENABLED && (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH) */


#if(UARTM_TX_ENABLED && (UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH))


    /*******************************************************************************
    * Function Name: UARTM_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_txBuffer - RAM buffer pointer for transmit data from.
    *  UARTM_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmited byte.
    *  UARTM_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(UARTM_TXISR)
    {

        #if(CY_PSOC3)
            uint8 int_en;
        #endif /* CY_PSOC3 */

        /* User code required at start of ISR */
        /* `#START UARTM_TXISR_START` */

        /* `#END` */

        #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
            int_en = EA;
            CyGlobalIntEnable;
        #endif /* CY_PSOC3 */

        while((UARTM_txBufferRead != UARTM_txBufferWrite) &&
             ((UARTM_TXSTATUS_REG & UARTM_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer. */
            if(UARTM_txBufferRead >= UARTM_TXBUFFERSIZE)
            {
                UARTM_txBufferRead = 0u;
            }

            UARTM_TXDATA_REG = UARTM_txBuffer[UARTM_txBufferRead];

            /* Set next pointer. */
            UARTM_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START UARTM_TXISR_END` */

        /* `#END` */

        #if(CY_PSOC3)
            EA = int_en;
        #endif /* CY_PSOC3 */

    }

#endif /* End UARTM_TX_ENABLED && (UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH) */


/* [] END OF FILE */
