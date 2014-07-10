/*******************************************************************************
* File Name: UARTM.c
* Version 2.30
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UARTM.h"
#include "CyLib.h"
#if(UARTM_INTERNAL_CLOCK_USED)
    #include "UARTM_IntClock.h"
#endif /* End UARTM_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 UARTM_initVar = 0u;
#if( UARTM_TX_ENABLED && (UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH))
    volatile uint8 UARTM_txBuffer[UARTM_TXBUFFERSIZE];
    volatile uint8 UARTM_txBufferRead = 0u;
    uint8 UARTM_txBufferWrite = 0u;
#endif /* End UARTM_TX_ENABLED */
#if( ( UARTM_RX_ENABLED || UARTM_HD_ENABLED ) && \
     (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH) )
    volatile uint8 UARTM_rxBuffer[UARTM_RXBUFFERSIZE];
    volatile uint8 UARTM_rxBufferRead = 0u;
    volatile uint8 UARTM_rxBufferWrite = 0u;
    volatile uint8 UARTM_rxBufferLoopDetect = 0u;
    volatile uint8 UARTM_rxBufferOverflow = 0u;
    #if (UARTM_RXHW_ADDRESS_ENABLED)
        volatile uint8 UARTM_rxAddressMode = UARTM_RXADDRESSMODE;
        volatile uint8 UARTM_rxAddressDetected = 0u;
    #endif /* End EnableHWAddress */
#endif /* End UARTM_RX_ENABLED */


/*******************************************************************************
* Function Name: UARTM_Start
********************************************************************************
*
* Summary:
*  Initialize and Enable the UART component.
*  Enable the clock input to enable operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The UARTM_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time UART_Start() is called. This allows for
*  component initialization without re-initialization in all subsequent calls
*  to the UARTM_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UARTM_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(UARTM_initVar == 0u)
    {
        UARTM_Init();
        UARTM_initVar = 1u;
    }
    UARTM_Enable();
}


/*******************************************************************************
* Function Name: UARTM_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the
*  customizer of the component placed onto schematic. Usually called in
*  UARTM_Start().
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void UARTM_Init(void) 
{
    #if(UARTM_RX_ENABLED || UARTM_HD_ENABLED)

        #if(UARTM_RX_INTERRUPT_ENABLED && (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH))
            /* Set the RX Interrupt. */
            (void)CyIntSetVector(UARTM_RX_VECT_NUM, &UARTM_RXISR);
            CyIntSetPriority(UARTM_RX_VECT_NUM, UARTM_RX_PRIOR_NUM);
        #endif /* End UARTM_RX_INTERRUPT_ENABLED */

        #if (UARTM_RXHW_ADDRESS_ENABLED)
            UARTM_SetRxAddressMode(UARTM_RXAddressMode);
            UARTM_SetRxAddress1(UARTM_RXHWADDRESS1);
            UARTM_SetRxAddress2(UARTM_RXHWADDRESS2);
        #endif /* End UARTM_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        UARTM_RXBITCTR_PERIOD_REG = UARTM_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        UARTM_RXSTATUS_MASK_REG  = UARTM_INIT_RX_INTERRUPTS_MASK;
    #endif /* End UARTM_RX_ENABLED || UARTM_HD_ENABLED*/

    #if(UARTM_TX_ENABLED)
        #if(UARTM_TX_INTERRUPT_ENABLED && (UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH))
            /* Set the TX Interrupt. */
            (void)CyIntSetVector(UARTM_TX_VECT_NUM, &UARTM_TXISR);
            CyIntSetPriority(UARTM_TX_VECT_NUM, UARTM_TX_PRIOR_NUM);
        #endif /* End UARTM_TX_INTERRUPT_ENABLED */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if(UARTM_TXCLKGEN_DP)
            UARTM_TXBITCLKGEN_CTR_REG = UARTM_BIT_CENTER;
            UARTM_TXBITCLKTX_COMPLETE_REG = (UARTM_NUMBER_OF_DATA_BITS +
                        UARTM_NUMBER_OF_START_BIT) * UARTM_OVER_SAMPLE_COUNT;
        #else
            UARTM_TXBITCTR_PERIOD_REG = ((UARTM_NUMBER_OF_DATA_BITS +
                        UARTM_NUMBER_OF_START_BIT) * UARTM_OVER_SAMPLE_8) - 1u;
        #endif /* End UARTM_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if(UARTM_TX_INTERRUPT_ENABLED && (UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH))
            UARTM_TXSTATUS_MASK_REG = UARTM_TX_STS_FIFO_EMPTY;
        #else
            UARTM_TXSTATUS_MASK_REG = UARTM_INIT_TX_INTERRUPTS_MASK;
        #endif /*End UARTM_TX_INTERRUPT_ENABLED*/

    #endif /* End UARTM_TX_ENABLED */

    #if(UARTM_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        UARTM_WriteControlRegister( \
            (UARTM_ReadControlRegister() & (uint8)~UARTM_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(UARTM_PARITY_TYPE << UARTM_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End UARTM_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: UARTM_Enable
********************************************************************************
*
* Summary:
*  Enables the UART block operation
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UARTM_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void UARTM_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if(UARTM_RX_ENABLED || UARTM_HD_ENABLED)
        /*RX Counter (Count7) Enable */
        UARTM_RXBITCTR_CONTROL_REG |= UARTM_CNTR_ENABLE;
        /* Enable the RX Interrupt. */
        UARTM_RXSTATUS_ACTL_REG  |= UARTM_INT_ENABLE;
        #if(UARTM_RX_INTERRUPT_ENABLED && (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH))
            CyIntEnable(UARTM_RX_VECT_NUM);
            #if (UARTM_RXHW_ADDRESS_ENABLED)
                UARTM_rxAddressDetected = 0u;
            #endif /* End UARTM_RXHW_ADDRESS_ENABLED */
        #endif /* End UARTM_RX_INTERRUPT_ENABLED */
    #endif /* End UARTM_RX_ENABLED || UARTM_HD_ENABLED*/

    #if(UARTM_TX_ENABLED)
        /*TX Counter (DP/Count7) Enable */
        #if(!UARTM_TXCLKGEN_DP)
            UARTM_TXBITCTR_CONTROL_REG |= UARTM_CNTR_ENABLE;
        #endif /* End UARTM_TXCLKGEN_DP */
        /* Enable the TX Interrupt. */
        UARTM_TXSTATUS_ACTL_REG |= UARTM_INT_ENABLE;
        #if(UARTM_TX_INTERRUPT_ENABLED && (UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH))
            CyIntEnable(UARTM_TX_VECT_NUM);
        #endif /* End UARTM_TX_INTERRUPT_ENABLED*/
     #endif /* End UARTM_TX_ENABLED */

    #if(UARTM_INTERNAL_CLOCK_USED)
        /* Enable the clock. */
        UARTM_IntClock_Start();
    #endif /* End UARTM_INTERNAL_CLOCK_USED */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: UARTM_Stop
********************************************************************************
*
* Summary:
*  Disable the UART component
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void UARTM_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if(UARTM_RX_ENABLED || UARTM_HD_ENABLED)
        UARTM_RXBITCTR_CONTROL_REG &= (uint8)~UARTM_CNTR_ENABLE;
    #endif /* End UARTM_RX_ENABLED */

    #if(UARTM_TX_ENABLED)
        #if(!UARTM_TXCLKGEN_DP)
            UARTM_TXBITCTR_CONTROL_REG &= (uint8)~UARTM_CNTR_ENABLE;
        #endif /* End UARTM_TXCLKGEN_DP */
    #endif /* UARTM_TX_ENABLED */

    #if(UARTM_INTERNAL_CLOCK_USED)
        /* Disable the clock. */
        UARTM_IntClock_Stop();
    #endif /* End UARTM_INTERNAL_CLOCK_USED */

    /* Disable internal interrupt component */
    #if(UARTM_RX_ENABLED || UARTM_HD_ENABLED)
        UARTM_RXSTATUS_ACTL_REG  &= (uint8)~UARTM_INT_ENABLE;
        #if(UARTM_RX_INTERRUPT_ENABLED && (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH))
            UARTM_DisableRxInt();
        #endif /* End UARTM_RX_INTERRUPT_ENABLED */
    #endif /* End UARTM_RX_ENABLED */

    #if(UARTM_TX_ENABLED)
        UARTM_TXSTATUS_ACTL_REG &= (uint8)~UARTM_INT_ENABLE;
        #if(UARTM_TX_INTERRUPT_ENABLED && (UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH))
            UARTM_DisableTxInt();
        #endif /* End UARTM_TX_INTERRUPT_ENABLED */
    #endif /* End UARTM_TX_ENABLED */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: UARTM_ReadControlRegister
********************************************************************************
*
* Summary:
*  Read the current state of the control register
*
* Parameters:
*  None.
*
* Return:
*  Current state of the control register.
*
*******************************************************************************/
uint8 UARTM_ReadControlRegister(void) 
{
    #if( UARTM_CONTROL_REG_REMOVED )
        return(0u);
    #else
        return(UARTM_CONTROL_REG);
    #endif /* End UARTM_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: UARTM_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  UARTM_WriteControlRegister(uint8 control) 
{
    #if( UARTM_CONTROL_REG_REMOVED )
        if(control != 0u) { }      /* release compiler warning */
    #else
       UARTM_CONTROL_REG = control;
    #endif /* End UARTM_CONTROL_REG_REMOVED */
}


#if(UARTM_RX_ENABLED || UARTM_HD_ENABLED)

    #if(UARTM_RX_INTERRUPT_ENABLED)

        /*******************************************************************************
        * Function Name: UARTM_EnableRxInt
        ********************************************************************************
        *
        * Summary:
        *  Enable RX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Enable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void UARTM_EnableRxInt(void) 
        {
            CyIntEnable(UARTM_RX_VECT_NUM);
        }


        /*******************************************************************************
        * Function Name: UARTM_DisableRxInt
        ********************************************************************************
        *
        * Summary:
        *  Disable RX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Disable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void UARTM_DisableRxInt(void) 
        {
            CyIntDisable(UARTM_RX_VECT_NUM);
        }

    #endif /* UARTM_RX_INTERRUPT_ENABLED */


    /*******************************************************************************
    * Function Name: UARTM_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configure which status bits trigger an interrupt event
    *
    * Parameters:
    *  IntSrc:  An or'd combination of the desired status bit masks (defined in
    *           the header file)
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void UARTM_SetRxInterruptMode(uint8 intSrc) 
    {
        UARTM_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: UARTM_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns data in RX Data register without checking status register to
    *  determine if data is valid
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  UARTM_rxBuffer - RAM buffer pointer for save received data.
    *  UARTM_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  UARTM_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  UARTM_rxBufferLoopDetect - creared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 UARTM_ReadRxData(void) 
    {
        uint8 rxData;

        #if(UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH)
            uint8 loc_rxBufferRead;
            uint8 loc_rxBufferWrite;
            /* Protect variables that could change on interrupt. */
            /* Disable Rx interrupt. */
            #if(UARTM_RX_INTERRUPT_ENABLED)
                UARTM_DisableRxInt();
            #endif /* UARTM_RX_INTERRUPT_ENABLED */
            loc_rxBufferRead = UARTM_rxBufferRead;
            loc_rxBufferWrite = UARTM_rxBufferWrite;

            if( (UARTM_rxBufferLoopDetect != 0u) || (loc_rxBufferRead != loc_rxBufferWrite) )
            {
                rxData = UARTM_rxBuffer[loc_rxBufferRead];
                loc_rxBufferRead++;

                if(loc_rxBufferRead >= UARTM_RXBUFFERSIZE)
                {
                    loc_rxBufferRead = 0u;
                }
                /* Update the real pointer */
                UARTM_rxBufferRead = loc_rxBufferRead;

                if(UARTM_rxBufferLoopDetect != 0u )
                {
                    UARTM_rxBufferLoopDetect = 0u;
                    #if( (UARTM_RX_INTERRUPT_ENABLED) && (UARTM_FLOW_CONTROL != 0u) && \
                         (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH) )
                        /* When Hardware Flow Control selected - return RX mask */
                        #if( UARTM_HD_ENABLED )
                            if((UARTM_CONTROL_REG & UARTM_CTRL_HD_SEND) == 0u)
                            {   /* In Half duplex mode return RX mask only in RX
                                *  configuration set, otherwise
                                *  mask will be returned in LoadRxConfig() API.
                                */
                                UARTM_RXSTATUS_MASK_REG  |= UARTM_RX_STS_FIFO_NOTEMPTY;
                            }
                        #else
                            UARTM_RXSTATUS_MASK_REG  |= UARTM_RX_STS_FIFO_NOTEMPTY;
                        #endif /* end UARTM_HD_ENABLED */
                    #endif /* UARTM_RX_INTERRUPT_ENABLED and Hardware flow control*/
                }
            }
            else
            {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit*/
                rxData = UARTM_RXDATA_REG;
            }

            /* Enable Rx interrupt. */
            #if(UARTM_RX_INTERRUPT_ENABLED)
                UARTM_EnableRxInt();
            #endif /* End UARTM_RX_INTERRUPT_ENABLED */

        #else /* UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH */

            /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit*/
            rxData = UARTM_RXDATA_REG;

        #endif /* UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: UARTM_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Read the current state of the status register
    *  And detect software buffer overflow.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Global Variables:
    *  UARTM_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn?t free space in
    *   UARTM_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   UARTM_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 UARTM_ReadRxStatus(void) 
    {
        uint8 status;

        status = UARTM_RXSTATUS_REG & UARTM_RX_HW_MASK;

        #if(UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH)
            if( UARTM_rxBufferOverflow != 0u )
            {
                status |= UARTM_RX_STS_SOFT_BUFF_OVER;
                UARTM_rxBufferOverflow = 0u;
            }
        #endif /* UARTM_RXBUFFERSIZE */

        return(status);
    }


    /*******************************************************************************
    * Function Name: UARTM_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, if data is not available or an error
    *  condition exists, zero is returned; otherwise, character is read and
    *  returned.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  UARTM_rxBuffer - RAM buffer pointer for save received data.
    *  UARTM_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  UARTM_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  UARTM_rxBufferLoopDetect - creared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 UARTM_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

        #if(UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH)
            uint8 loc_rxBufferRead;
            uint8 loc_rxBufferWrite;
            /* Protect variables that could change on interrupt. */
            /* Disable Rx interrupt. */
            #if(UARTM_RX_INTERRUPT_ENABLED)
                UARTM_DisableRxInt();
            #endif /* UARTM_RX_INTERRUPT_ENABLED */
            loc_rxBufferRead = UARTM_rxBufferRead;
            loc_rxBufferWrite = UARTM_rxBufferWrite;

            if( (UARTM_rxBufferLoopDetect != 0u) || (loc_rxBufferRead != loc_rxBufferWrite) )
            {
                rxData = UARTM_rxBuffer[loc_rxBufferRead];
                loc_rxBufferRead++;
                if(loc_rxBufferRead >= UARTM_RXBUFFERSIZE)
                {
                    loc_rxBufferRead = 0u;
                }
                /* Update the real pointer */
                UARTM_rxBufferRead = loc_rxBufferRead;

                if(UARTM_rxBufferLoopDetect > 0u )
                {
                    UARTM_rxBufferLoopDetect = 0u;
                    #if( (UARTM_RX_INTERRUPT_ENABLED) && (UARTM_FLOW_CONTROL != 0u) )
                        /* When Hardware Flow Control selected - return RX mask */
                        #if( UARTM_HD_ENABLED )
                            if((UARTM_CONTROL_REG & UARTM_CTRL_HD_SEND) == 0u)
                            {   /* In Half duplex mode return RX mask only if
                                *  RX configuration set, otherwise
                                *  mask will be returned in LoadRxConfig() API.
                                */
                                UARTM_RXSTATUS_MASK_REG  |= UARTM_RX_STS_FIFO_NOTEMPTY;
                            }
                        #else
                            UARTM_RXSTATUS_MASK_REG  |= UARTM_RX_STS_FIFO_NOTEMPTY;
                        #endif /* end UARTM_HD_ENABLED */
                    #endif /* UARTM_RX_INTERRUPT_ENABLED and Hardware flow control*/
                }

            }
            else
            {   rxStatus = UARTM_RXSTATUS_REG;
                if((rxStatus & UARTM_RX_STS_FIFO_NOTEMPTY) != 0u)
                {   /* Read received data from FIFO*/
                    rxData = UARTM_RXDATA_REG;
                    /*Check status on error*/
                    if((rxStatus & (UARTM_RX_STS_BREAK | UARTM_RX_STS_PAR_ERROR |
                                   UARTM_RX_STS_STOP_ERROR | UARTM_RX_STS_OVERRUN)) != 0u)
                    {
                        rxData = 0u;
                    }
                }
            }

            /* Enable Rx interrupt. */
            #if(UARTM_RX_INTERRUPT_ENABLED)
                UARTM_EnableRxInt();
            #endif /* UARTM_RX_INTERRUPT_ENABLED */

        #else /* UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH */

            rxStatus =UARTM_RXSTATUS_REG;
            if((rxStatus & UARTM_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO*/
                rxData = UARTM_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (UARTM_RX_STS_BREAK | UARTM_RX_STS_PAR_ERROR |
                               UARTM_RX_STS_STOP_ERROR | UARTM_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        #endif /* UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: UARTM_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Grab the next available byte of data from the recieve FIFO
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains Status Register and LSB contains UART RX data
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 UARTM_GetByte(void) 
    {
        return ( ((uint16)UARTM_ReadRxStatus() << 8u) | UARTM_ReadRxData() );
    }


    /*******************************************************************************
    * Function Name: UARTM_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Determine the amount of bytes left in the RX buffer and return the count in
    *  bytes
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Integer count of the number of bytes left
    *  in the RX buffer
    *
    * Global Variables:
    *  UARTM_rxBufferWrite - used to calculate left bytes.
    *  UARTM_rxBufferRead - used to calculate left bytes.
    *  UARTM_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 UARTM_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

        #if(UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH)

            /* Disable Rx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(UARTM_RX_INTERRUPT_ENABLED)
                UARTM_DisableRxInt();
            #endif /* UARTM_RX_INTERRUPT_ENABLED */

            if(UARTM_rxBufferRead == UARTM_rxBufferWrite)
            {
                if(UARTM_rxBufferLoopDetect > 0u)
                {
                    size = UARTM_RXBUFFERSIZE;
                }
                else
                {
                    size = 0u;
                }
            }
            else if(UARTM_rxBufferRead < UARTM_rxBufferWrite)
            {
                size = (UARTM_rxBufferWrite - UARTM_rxBufferRead);
            }
            else
            {
                size = (UARTM_RXBUFFERSIZE - UARTM_rxBufferRead) + UARTM_rxBufferWrite;
            }

            /* Enable Rx interrupt. */
            #if(UARTM_RX_INTERRUPT_ENABLED)
                UARTM_EnableRxInt();
            #endif /* End UARTM_RX_INTERRUPT_ENABLED */

        #else /* UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH */

            /* We can only know if there is data in the fifo. */
            size = ((UARTM_RXSTATUS_REG & UARTM_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

        #endif /* End UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH */

        return(size);
    }


    /*******************************************************************************
    * Function Name: UARTM_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the RX RAM buffer by setting the read and write pointers both to zero.
    *  Clears hardware RX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_rxBufferWrite - cleared to zero.
    *  UARTM_rxBufferRead - cleared to zero.
    *  UARTM_rxBufferLoopDetect - cleared to zero.
    *  UARTM_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *******************************************************************************/
    void UARTM_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* clear the HW FIFO */
        /* Enter critical section */
        enableInterrupts = CyEnterCriticalSection();
        UARTM_RXDATA_AUX_CTL_REG |=  UARTM_RX_FIFO_CLR;
        UARTM_RXDATA_AUX_CTL_REG &= (uint8)~UARTM_RX_FIFO_CLR;
        /* Exit critical section */
        CyExitCriticalSection(enableInterrupts);

        #if(UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH)
            /* Disable Rx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(UARTM_RX_INTERRUPT_ENABLED)
                UARTM_DisableRxInt();
            #endif /* End UARTM_RX_INTERRUPT_ENABLED */

            UARTM_rxBufferRead = 0u;
            UARTM_rxBufferWrite = 0u;
            UARTM_rxBufferLoopDetect = 0u;
            UARTM_rxBufferOverflow = 0u;

            /* Enable Rx interrupt. */
            #if(UARTM_RX_INTERRUPT_ENABLED)
                UARTM_EnableRxInt();
            #endif /* End UARTM_RX_INTERRUPT_ENABLED */
        #endif /* End UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH */

    }


    /*******************************************************************************
    * Function Name: UARTM_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Set the receive addressing mode
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  UARTM__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  UARTM__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  UARTM__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  UARTM__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  UARTM__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  UARTM_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void UARTM_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(UARTM_RXHW_ADDRESS_ENABLED)
            #if(UARTM_CONTROL_REG_REMOVED)
                if(addressMode != 0u) { }     /* release compiler warning */
            #else /* UARTM_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = UARTM_CONTROL_REG & (uint8)~UARTM_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << UARTM_CTRL_RXADDR_MODE0_SHIFT);
                UARTM_CONTROL_REG = tmpCtrl;
                #if(UARTM_RX_INTERRUPT_ENABLED && \
                   (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH) )
                    UARTM_rxAddressMode = addressMode;
                    UARTM_rxAddressDetected = 0u;
                #endif /* End UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH*/
            #endif /* End UARTM_CONTROL_REG_REMOVED */
        #else /* UARTM_RXHW_ADDRESS_ENABLED */
            if(addressMode != 0u) { }     /* release compiler warning */
        #endif /* End UARTM_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: UARTM_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Set the first hardware address compare value
    *
    * Parameters:
    *  address
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void UARTM_SetRxAddress1(uint8 address) 

    {
        UARTM_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: UARTM_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Set the second hardware address compare value
    *
    * Parameters:
    *  address
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void UARTM_SetRxAddress2(uint8 address) 
    {
        UARTM_RXADDRESS2_REG = address;
    }

#endif  /* UARTM_RX_ENABLED || UARTM_HD_ENABLED*/


#if( (UARTM_TX_ENABLED) || (UARTM_HD_ENABLED) )

    #if(UARTM_TX_INTERRUPT_ENABLED)

        /*******************************************************************************
        * Function Name: UARTM_EnableTxInt
        ********************************************************************************
        *
        * Summary:
        *  Enable TX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Enable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void UARTM_EnableTxInt(void) 
        {
            CyIntEnable(UARTM_TX_VECT_NUM);
        }


        /*******************************************************************************
        * Function Name: UARTM_DisableTxInt
        ********************************************************************************
        *
        * Summary:
        *  Disable TX interrupt generation
        *
        * Parameters:
        *  None.
        *
        * Return:
        *  None.
        *
        * Theory:
        *  Disable the interrupt output -or- the interrupt component itself
        *
        *******************************************************************************/
        void UARTM_DisableTxInt(void) 
        {
            CyIntDisable(UARTM_TX_VECT_NUM);
        }

    #endif /* UARTM_TX_INTERRUPT_ENABLED */


    /*******************************************************************************
    * Function Name: UARTM_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configure which status bits trigger an interrupt event
    *
    * Parameters:
    *  intSrc: An or'd combination of the desired status bit masks (defined in
    *          the header file)
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void UARTM_SetTxInterruptMode(uint8 intSrc) 
    {
        UARTM_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: UARTM_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Write a byte of data to the Transmit FIFO or TX buffer to be sent when the
    *  bus is available. WriteTxData sends a byte without checking for buffer room
    *  or status. It is up to the user to separately check status.
    *
    * Parameters:
    *  TXDataByte: byte of data to place in the transmit FIFO
    *
    * Return:
    * void
    *
    * Global Variables:
    *  UARTM_txBuffer - RAM buffer pointer for save data for transmission
    *  UARTM_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  UARTM_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  UARTM_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void UARTM_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(UARTM_initVar != 0u)
        {
            #if(UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH)

                /* Disable Tx interrupt. */
                /* Protect variables that could change on interrupt. */
                #if(UARTM_TX_INTERRUPT_ENABLED)
                    UARTM_DisableTxInt();
                #endif /* End UARTM_TX_INTERRUPT_ENABLED */

                if( (UARTM_txBufferRead == UARTM_txBufferWrite) &&
                    ((UARTM_TXSTATUS_REG & UARTM_TX_STS_FIFO_FULL) == 0u) )
                {
                    /* Add directly to the FIFO. */
                    UARTM_TXDATA_REG = txDataByte;
                }
                else
                {
                    if(UARTM_txBufferWrite >= UARTM_TXBUFFERSIZE)
                    {
                        UARTM_txBufferWrite = 0u;
                    }

                    UARTM_txBuffer[UARTM_txBufferWrite] = txDataByte;

                    /* Add to the software buffer. */
                    UARTM_txBufferWrite++;

                }

                /* Enable Tx interrupt. */
                #if(UARTM_TX_INTERRUPT_ENABLED)
                    UARTM_EnableTxInt();
                #endif /* End UARTM_TX_INTERRUPT_ENABLED */

            #else /* UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH */

                /* Add directly to the FIFO. */
                UARTM_TXDATA_REG = txDataByte;

            #endif /* End UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH */
        }
    }


    /*******************************************************************************
    * Function Name: UARTM_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Read the status register for the component
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the status register which is clear on read. It is up to
    *  the user to handle all bits in this return value accordingly, even if the bit
    *  was not enabled as an interrupt source the event happened and must be handled
    *  accordingly.
    *
    *******************************************************************************/
    uint8 UARTM_ReadTxStatus(void) 
    {
        return(UARTM_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: UARTM_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Wait to send byte until TX register or buffer has room.
    *
    * Parameters:
    *  txDataByte: The 8-bit data value to send across the UART.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_txBuffer - RAM buffer pointer for save data for transmission
    *  UARTM_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  UARTM_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  UARTM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void UARTM_PutChar(uint8 txDataByte) 
    {
            #if(UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH)
                /* The temporary output pointer is used since it takes two instructions
                *  to increment with a wrap, and we can't risk doing that with the real
                *  pointer and getting an interrupt in between instructions.
                */
                uint8 loc_txBufferWrite;
                uint8 loc_txBufferRead;

                do{
                    /* Block if software buffer is full, so we don't overwrite. */
                    #if ((UARTM_TXBUFFERSIZE > UARTM_MAX_BYTE_VALUE) && (CY_PSOC3))
                        /* Disable TX interrupt to protect variables that could change on interrupt */
                        CyIntDisable(UARTM_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                    loc_txBufferWrite = UARTM_txBufferWrite;
                    loc_txBufferRead = UARTM_txBufferRead;
                    #if ((UARTM_TXBUFFERSIZE > UARTM_MAX_BYTE_VALUE) && (CY_PSOC3))
                        /* Enable interrupt to continue transmission */
                        CyIntEnable(UARTM_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                }while( (loc_txBufferWrite < loc_txBufferRead) ? (loc_txBufferWrite == (loc_txBufferRead - 1u)) :
                                        ((loc_txBufferWrite - loc_txBufferRead) ==
                                        (uint8)(UARTM_TXBUFFERSIZE - 1u)) );

                if( (loc_txBufferRead == loc_txBufferWrite) &&
                    ((UARTM_TXSTATUS_REG & UARTM_TX_STS_FIFO_FULL) == 0u) )
                {
                    /* Add directly to the FIFO. */
                    UARTM_TXDATA_REG = txDataByte;
                }
                else
                {
                    if(loc_txBufferWrite >= UARTM_TXBUFFERSIZE)
                    {
                        loc_txBufferWrite = 0u;
                    }
                    /* Add to the software buffer. */
                    UARTM_txBuffer[loc_txBufferWrite] = txDataByte;
                    loc_txBufferWrite++;

                    /* Finally, update the real output pointer */
                    #if ((UARTM_TXBUFFERSIZE > UARTM_MAX_BYTE_VALUE) && (CY_PSOC3))
                        CyIntDisable(UARTM_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                    UARTM_txBufferWrite = loc_txBufferWrite;
                    #if ((UARTM_TXBUFFERSIZE > UARTM_MAX_BYTE_VALUE) && (CY_PSOC3))
                        CyIntEnable(UARTM_TX_VECT_NUM);
                    #endif /* End TXBUFFERSIZE > 255 */
                }

            #else /* UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH */

                while((UARTM_TXSTATUS_REG & UARTM_TX_STS_FIFO_FULL) != 0u)
                {
                    ; /* Wait for room in the FIFO. */
                }

                /* Add directly to the FIFO. */
                UARTM_TXDATA_REG = txDataByte;

            #endif /* End UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH */
    }


    /*******************************************************************************
    * Function Name: UARTM_PutString
    ********************************************************************************
    *
    * Summary:
    *  Write a Sequence of bytes on the Transmit line. Data comes from RAM or ROM.
    *
    * Parameters:
    *  string: char pointer to character string of Data to Send.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  This function will block if there is not enough memory to place the whole
    *  string, it will block until the entire string has been written to the
    *  transmit buffer.
    *
    *******************************************************************************/
    void UARTM_PutString(const char8 string[]) 
    {
        uint16 buf_index = 0u;
        /* If not Initialized then skip this function*/
        if(UARTM_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent*/
            while(string[buf_index] != (char8)0)
            {
                UARTM_PutChar((uint8)string[buf_index]);
                buf_index++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: UARTM_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Write a Sequence of bytes on the Transmit line. Data comes from RAM or ROM.
    *
    * Parameters:
    *  string: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of Bytes to be transmitted.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void UARTM_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 buf_index = 0u;
        /* If not Initialized then skip this function*/
        if(UARTM_initVar != 0u)
        {
            do
            {
                UARTM_PutChar(string[buf_index]);
                buf_index++;
            }while(buf_index < byteCount);
        }
    }


    /*******************************************************************************
    * Function Name: UARTM_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Write a character and then carriage return and line feed.
    *
    * Parameters:
    *  txDataByte: uint8 Character to send.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void UARTM_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(UARTM_initVar != 0u)
        {
            UARTM_PutChar(txDataByte);
            UARTM_PutChar(0x0Du);
            UARTM_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: UARTM_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Determine the amount of space left in the TX buffer and return the count in
    *  bytes
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Integer count of the number of bytes left in the TX buffer
    *
    * Global Variables:
    *  UARTM_txBufferWrite - used to calculate left space.
    *  UARTM_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 UARTM_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

        #if(UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH)

            /* Disable Tx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(UARTM_TX_INTERRUPT_ENABLED)
                UARTM_DisableTxInt();
            #endif /* End UARTM_TX_INTERRUPT_ENABLED */

            if(UARTM_txBufferRead == UARTM_txBufferWrite)
            {
                size = 0u;
            }
            else if(UARTM_txBufferRead < UARTM_txBufferWrite)
            {
                size = (UARTM_txBufferWrite - UARTM_txBufferRead);
            }
            else
            {
                size = (UARTM_TXBUFFERSIZE - UARTM_txBufferRead) + UARTM_txBufferWrite;
            }

            /* Enable Tx interrupt. */
            #if(UARTM_TX_INTERRUPT_ENABLED)
                UARTM_EnableTxInt();
            #endif /* End UARTM_TX_INTERRUPT_ENABLED */

        #else /* UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH */

            size = UARTM_TXSTATUS_REG;

            /* Is the fifo is full. */
            if((size & UARTM_TX_STS_FIFO_FULL) != 0u)
            {
                size = UARTM_FIFO_LENGTH;
            }
            else if((size & UARTM_TX_STS_FIFO_EMPTY) != 0u)
            {
                size = 0u;
            }
            else
            {
                /* We only know there is data in the fifo. */
                size = 1u;
            }

        #endif /* End UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH */

        return(size);
    }


    /*******************************************************************************
    * Function Name: UARTM_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the TX RAM buffer by setting the read and write pointers both to zero.
    *  Clears the hardware TX FIFO.  Any data present in the FIFO will not be sent.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_txBufferWrite - cleared to zero.
    *  UARTM_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM buffer will be lost when overwritten.
    *
    *******************************************************************************/
    void UARTM_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Enter critical section */
        enableInterrupts = CyEnterCriticalSection();
        /* clear the HW FIFO */
        UARTM_TXDATA_AUX_CTL_REG |=  UARTM_TX_FIFO_CLR;
        UARTM_TXDATA_AUX_CTL_REG &= (uint8)~UARTM_TX_FIFO_CLR;
        /* Exit critical section */
        CyExitCriticalSection(enableInterrupts);

        #if(UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH)

            /* Disable Tx interrupt. */
            /* Protect variables that could change on interrupt. */
            #if(UARTM_TX_INTERRUPT_ENABLED)
                UARTM_DisableTxInt();
            #endif /* End UARTM_TX_INTERRUPT_ENABLED */

            UARTM_txBufferRead = 0u;
            UARTM_txBufferWrite = 0u;

            /* Enable Tx interrupt. */
            #if(UARTM_TX_INTERRUPT_ENABLED)
                UARTM_EnableTxInt();
            #endif /* End UARTM_TX_INTERRUPT_ENABLED */

        #endif /* End UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH */
    }


    /*******************************************************************************
    * Function Name: UARTM_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Write a Break command to the UART
    *
    * Parameters:
    *  uint8 retMode:  Wait mode,
    *   0 - Initialize registers for Break, sends the Break signal and return
    *       imediately.
    *   1 - Wait until Break sending is complete, reinitialize registers to normal
    *       transmission mode then return.
    *   2 - Reinitialize registers to normal transmission mode then return.
    *   3 - both steps: 0 and 1
    *       init registers for Break, send Break signal
    *       wait until Break sending is complete, reinit registers to normal
    *       transmission mode then return.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UARTM_initVar - checked to identify that the component has been
    *     initialized.
    *  tx_period - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  Trere are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Funcition will block CPU untill transmition
    *     complete.
    *  2) User may want to use bloking time if UART configured to the low speed
    *     operation
    *     Emample for this case:
    *     SendBreak(0);     - init Break signal transmition
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to init and use the interrupt for complete
    *     break operation.
    *     Example for this case:
    *     Init TX interrupt whith "TX - On TX Complete" parameter
    *     SendBreak(0);     - init Break signal transmition
    *         Add your code here to use CPU time
    *     When interrupt appear with UART_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *   Uses static variable to keep registers configuration.
    *
    *******************************************************************************/
    void UARTM_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(UARTM_initVar != 0u)
        {
            /*Set the Counter to 13-bits and transmit a 00 byte*/
            /*When that is done then reset the counter value back*/
            uint8 tmpStat;

            #if(UARTM_HD_ENABLED) /* Half Duplex mode*/

                if( (retMode == UARTM_SEND_BREAK) ||
                    (retMode == UARTM_SEND_WAIT_REINIT ) )
                {
                    /* CTRL_HD_SEND_BREAK - sends break bits in HD mode*/
                    UARTM_WriteControlRegister(UARTM_ReadControlRegister() |
                                                          UARTM_CTRL_HD_SEND_BREAK);
                    /* Send zeros*/
                    UARTM_TXDATA_REG = 0u;

                    do /*wait until transmit starts*/
                    {
                        tmpStat = UARTM_TXSTATUS_REG;
                    }while((tmpStat & UARTM_TX_STS_FIFO_EMPTY) != 0u);
                }

                if( (retMode == UARTM_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == UARTM_SEND_WAIT_REINIT) )
                {
                    do /*wait until transmit complete*/
                    {
                        tmpStat = UARTM_TXSTATUS_REG;
                    }while(((uint8)~tmpStat & UARTM_TX_STS_COMPLETE) != 0u);
                }

                if( (retMode == UARTM_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == UARTM_REINIT) ||
                    (retMode == UARTM_SEND_WAIT_REINIT) )
                {
                    UARTM_WriteControlRegister(UARTM_ReadControlRegister() &
                                                  (uint8)~UARTM_CTRL_HD_SEND_BREAK);
                }

            #else /* UARTM_HD_ENABLED Full Duplex mode */

                static uint8 tx_period;

                if( (retMode == UARTM_SEND_BREAK) ||
                    (retMode == UARTM_SEND_WAIT_REINIT) )
                {
                    /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode*/
                    #if( (UARTM_PARITY_TYPE != UARTM__B_UART__NONE_REVB) || \
                                        (UARTM_PARITY_TYPE_SW != 0u) )
                        UARTM_WriteControlRegister(UARTM_ReadControlRegister() |
                                                              UARTM_CTRL_HD_SEND_BREAK);
                    #endif /* End UARTM_PARITY_TYPE != UARTM__B_UART__NONE_REVB  */

                    #if(UARTM_TXCLKGEN_DP)
                        tx_period = UARTM_TXBITCLKTX_COMPLETE_REG;
                        UARTM_TXBITCLKTX_COMPLETE_REG = UARTM_TXBITCTR_BREAKBITS;
                    #else
                        tx_period = UARTM_TXBITCTR_PERIOD_REG;
                        UARTM_TXBITCTR_PERIOD_REG = UARTM_TXBITCTR_BREAKBITS8X;
                    #endif /* End UARTM_TXCLKGEN_DP */

                    /* Send zeros*/
                    UARTM_TXDATA_REG = 0u;

                    do /* wait until transmit starts */
                    {
                        tmpStat = UARTM_TXSTATUS_REG;
                    }while((tmpStat & UARTM_TX_STS_FIFO_EMPTY) != 0u);
                }

                if( (retMode == UARTM_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == UARTM_SEND_WAIT_REINIT) )
                {
                    do /*wait until transmit complete*/
                    {
                        tmpStat = UARTM_TXSTATUS_REG;
                    }while(((uint8)~tmpStat & UARTM_TX_STS_COMPLETE) != 0u);
                }

                if( (retMode == UARTM_WAIT_FOR_COMPLETE_REINIT) ||
                    (retMode == UARTM_REINIT) ||
                    (retMode == UARTM_SEND_WAIT_REINIT) )
                {

                    #if(UARTM_TXCLKGEN_DP)
                        UARTM_TXBITCLKTX_COMPLETE_REG = tx_period;
                    #else
                        UARTM_TXBITCTR_PERIOD_REG = tx_period;
                    #endif /* End UARTM_TXCLKGEN_DP */

                    #if( (UARTM_PARITY_TYPE != UARTM__B_UART__NONE_REVB) || \
                         (UARTM_PARITY_TYPE_SW != 0u) )
                        UARTM_WriteControlRegister(UARTM_ReadControlRegister() &
                                                      (uint8)~UARTM_CTRL_HD_SEND_BREAK);
                    #endif /* End UARTM_PARITY_TYPE != NONE */
                }
            #endif    /* End UARTM_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: UARTM_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Set the transmit addressing mode
    *
    * Parameters:
    *  addressMode: 0 -> Space
    *               1 -> Mark
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void UARTM_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable*/
        if(addressMode != 0u)
        {
            #if( UARTM_CONTROL_REG_REMOVED == 0u )
                UARTM_WriteControlRegister(UARTM_ReadControlRegister() |
                                                      UARTM_CTRL_MARK);
            #endif /* End UARTM_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
            #if( UARTM_CONTROL_REG_REMOVED == 0u )
                UARTM_WriteControlRegister(UARTM_ReadControlRegister() &
                                                    (uint8)~UARTM_CTRL_MARK);
            #endif /* End UARTM_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndUARTM_TX_ENABLED */

#if(UARTM_HD_ENABLED)


    /*******************************************************************************
    * Function Name: UARTM_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Unloads the Rx configuration if required and loads the
    *  Tx configuration. It is the users responsibility to ensure that any
    *  transaction is complete and it is safe to unload the Tx
    *  configuration.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Valid only for half duplex UART.
    *
    * Side Effects:
    *  Disable RX interrupt mask, when software buffer has been used.
    *
    *******************************************************************************/
    void UARTM_LoadTxConfig(void) 
    {
        #if((UARTM_RX_INTERRUPT_ENABLED) && (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH))
            /* Disable RX interrupts before set TX configuration */
            UARTM_SetRxInterruptMode(0u);
        #endif /* UARTM_RX_INTERRUPT_ENABLED */

        UARTM_WriteControlRegister(UARTM_ReadControlRegister() | UARTM_CTRL_HD_SEND);
        UARTM_RXBITCTR_PERIOD_REG = UARTM_HD_TXBITCTR_INIT;
        #if(CY_UDB_V0) /* Manually clear status register when mode has been changed */
            /* Clear status register */
            CY_GET_REG8(UARTM_RXSTATUS_PTR);
        #endif /* CY_UDB_V0 */
    }


    /*******************************************************************************
    * Function Name: UARTM_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Unloads the Tx configuration if required and loads the
    *  Rx configuration. It is the users responsibility to ensure that any
    *  transaction is complete and it is safe to unload the Rx
    *  configuration.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Valid only for half duplex UART
    *
    * Side Effects:
    *  Set RX interrupt mask based on customizer settings, when software buffer
    *  has been used.
    *
    *******************************************************************************/
    void UARTM_LoadRxConfig(void) 
    {
        UARTM_WriteControlRegister(UARTM_ReadControlRegister() &
                                                (uint8)~UARTM_CTRL_HD_SEND);
        UARTM_RXBITCTR_PERIOD_REG = UARTM_HD_RXBITCTR_INIT;
        #if(CY_UDB_V0) /* Manually clear status register when mode has been changed */
            /* Clear status register */
            CY_GET_REG8(UARTM_RXSTATUS_PTR);
        #endif /* CY_UDB_V0 */

        #if((UARTM_RX_INTERRUPT_ENABLED) && (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH))
            /* Enable RX interrupt after set RX configuration */
            UARTM_SetRxInterruptMode(UARTM_INIT_RX_INTERRUPTS_MASK);
        #endif /* UARTM_RX_INTERRUPT_ENABLED */
    }

#endif  /* UARTM_HD_ENABLED */


/* [] END OF FILE */
