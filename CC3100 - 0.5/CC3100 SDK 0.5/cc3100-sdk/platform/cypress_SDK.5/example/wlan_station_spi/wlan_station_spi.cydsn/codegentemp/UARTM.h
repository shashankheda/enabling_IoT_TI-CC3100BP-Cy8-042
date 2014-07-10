/*******************************************************************************
* File Name: UARTM.h
* Version 2.30
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_UARTM_H)
#define CY_UART_UARTM_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define UARTM_RX_ENABLED                     (1u)
#define UARTM_TX_ENABLED                     (1u)
#define UARTM_HD_ENABLED                     (0u)
#define UARTM_RX_INTERRUPT_ENABLED           (0u)
#define UARTM_TX_INTERRUPT_ENABLED           (0u)
#define UARTM_INTERNAL_CLOCK_USED            (1u)
#define UARTM_RXHW_ADDRESS_ENABLED           (0u)
#define UARTM_OVER_SAMPLE_COUNT              (8u)
#define UARTM_PARITY_TYPE                    (0u)
#define UARTM_PARITY_TYPE_SW                 (0u)
#define UARTM_BREAK_DETECT                   (0u)
#define UARTM_BREAK_BITS_TX                  (13u)
#define UARTM_BREAK_BITS_RX                  (13u)
#define UARTM_TXCLKGEN_DP                    (1u)
#define UARTM_USE23POLLING                   (1u)
#define UARTM_FLOW_CONTROL                   (1u)
#define UARTM_CLK_FREQ                       (1u)
#define UARTM_TXBUFFERSIZE                   (4u)
#define UARTM_RXBUFFERSIZE                   (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_30 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#ifdef UARTM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define UARTM_CONTROL_REG_REMOVED            (0u)
#else
    #define UARTM_CONTROL_REG_REMOVED            (1u)
#endif /* End UARTM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct UARTM_backupStruct_
{
    uint8 enableState;

    #if(UARTM_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End UARTM_CONTROL_REG_REMOVED */
    #if( (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) )
        uint8 rx_period;
        #if (CY_UDB_V0)
            uint8 rx_mask;
            #if (UARTM_RXHW_ADDRESS_ENABLED)
                uint8 rx_addr1;
                uint8 rx_addr2;
            #endif /* End UARTM_RXHW_ADDRESS_ENABLED */
        #endif /* End CY_UDB_V0 */
    #endif  /* End (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED)*/

    #if(UARTM_TX_ENABLED)
        #if(UARTM_TXCLKGEN_DP)
            uint8 tx_clk_ctr;
            #if (CY_UDB_V0)
                uint8 tx_clk_compl;
            #endif  /* End CY_UDB_V0 */
        #else
            uint8 tx_period;
        #endif /*End UARTM_TXCLKGEN_DP */
        #if (CY_UDB_V0)
            uint8 tx_mask;
        #endif  /* End CY_UDB_V0 */
    #endif /*End UARTM_TX_ENABLED */
} UARTM_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void UARTM_Start(void) ;
void UARTM_Stop(void) ;
uint8 UARTM_ReadControlRegister(void) ;
void UARTM_WriteControlRegister(uint8 control) ;

void UARTM_Init(void) ;
void UARTM_Enable(void) ;
void UARTM_SaveConfig(void) ;
void UARTM_RestoreConfig(void) ;
void UARTM_Sleep(void) ;
void UARTM_Wakeup(void) ;

/* Only if RX is enabled */
#if( (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) )

    #if(UARTM_RX_INTERRUPT_ENABLED)
        void  UARTM_EnableRxInt(void) ;
        void  UARTM_DisableRxInt(void) ;
        CY_ISR_PROTO(UARTM_RXISR);
    #endif /* UARTM_RX_INTERRUPT_ENABLED */

    void UARTM_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void UARTM_SetRxAddress1(uint8 address) ;
    void UARTM_SetRxAddress2(uint8 address) ;

    void  UARTM_SetRxInterruptMode(uint8 intSrc) ;
    uint8 UARTM_ReadRxData(void) ;
    uint8 UARTM_ReadRxStatus(void) ;
    uint8 UARTM_GetChar(void) ;
    uint16 UARTM_GetByte(void) ;
    uint8 UARTM_GetRxBufferSize(void)
                                                            ;
    void UARTM_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define UARTM_GetRxInterruptSource   UARTM_ReadRxStatus

#endif /* End (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) */

/* Only if TX is enabled */
#if(UARTM_TX_ENABLED || UARTM_HD_ENABLED)

    #if(UARTM_TX_INTERRUPT_ENABLED)
        void UARTM_EnableTxInt(void) ;
        void UARTM_DisableTxInt(void) ;
        CY_ISR_PROTO(UARTM_TXISR);
    #endif /* UARTM_TX_INTERRUPT_ENABLED */

    void UARTM_SetTxInterruptMode(uint8 intSrc) ;
    void UARTM_WriteTxData(uint8 txDataByte) ;
    uint8 UARTM_ReadTxStatus(void) ;
    void UARTM_PutChar(uint8 txDataByte) ;
    void UARTM_PutString(const char8 string[]) ;
    void UARTM_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void UARTM_PutCRLF(uint8 txDataByte) ;
    void UARTM_ClearTxBuffer(void) ;
    void UARTM_SetTxAddressMode(uint8 addressMode) ;
    void UARTM_SendBreak(uint8 retMode) ;
    uint8 UARTM_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define UARTM_PutStringConst         UARTM_PutString
    #define UARTM_PutArrayConst          UARTM_PutArray
    #define UARTM_GetTxInterruptSource   UARTM_ReadTxStatus

#endif /* End UARTM_TX_ENABLED || UARTM_HD_ENABLED */

#if(UARTM_HD_ENABLED)
    void UARTM_LoadRxConfig(void) ;
    void UARTM_LoadTxConfig(void) ;
#endif /* End UARTM_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_UARTM) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    UARTM_CyBtldrCommStart(void) CYSMALL ;
    void    UARTM_CyBtldrCommStop(void) CYSMALL ;
    void    UARTM_CyBtldrCommReset(void) CYSMALL ;
    cystatus UARTM_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus UARTM_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_UARTM)
        #define CyBtldrCommStart    UARTM_CyBtldrCommStart
        #define CyBtldrCommStop     UARTM_CyBtldrCommStop
        #define CyBtldrCommReset    UARTM_CyBtldrCommReset
        #define CyBtldrCommWrite    UARTM_CyBtldrCommWrite
        #define CyBtldrCommRead     UARTM_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_UARTM) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define UARTM_BYTE2BYTE_TIME_OUT (25u)

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define UARTM_SET_SPACE                              (0x00u)
#define UARTM_SET_MARK                               (0x01u)

/* Status Register definitions */
#if( (UARTM_TX_ENABLED) || (UARTM_HD_ENABLED) )
    #if(UARTM_TX_INTERRUPT_ENABLED)
        #define UARTM_TX_VECT_NUM            (uint8)UARTM_TXInternalInterrupt__INTC_NUMBER
        #define UARTM_TX_PRIOR_NUM           (uint8)UARTM_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* UARTM_TX_INTERRUPT_ENABLED */
    #if(UARTM_TX_ENABLED)
        #define UARTM_TX_STS_COMPLETE_SHIFT          (0x00u)
        #define UARTM_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
        #define UARTM_TX_STS_FIFO_FULL_SHIFT         (0x02u)
        #define UARTM_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #endif /* UARTM_TX_ENABLED */
    #if(UARTM_HD_ENABLED)
        #define UARTM_TX_STS_COMPLETE_SHIFT          (0x00u)
        #define UARTM_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
        #define UARTM_TX_STS_FIFO_FULL_SHIFT         (0x05u)  /*needs MD=0*/
        #define UARTM_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #endif /* UARTM_HD_ENABLED */
    #define UARTM_TX_STS_COMPLETE            (uint8)(0x01u << UARTM_TX_STS_COMPLETE_SHIFT)
    #define UARTM_TX_STS_FIFO_EMPTY          (uint8)(0x01u << UARTM_TX_STS_FIFO_EMPTY_SHIFT)
    #define UARTM_TX_STS_FIFO_FULL           (uint8)(0x01u << UARTM_TX_STS_FIFO_FULL_SHIFT)
    #define UARTM_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << UARTM_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (UARTM_TX_ENABLED) || (UARTM_HD_ENABLED)*/

#if( (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) )
    #if(UARTM_RX_INTERRUPT_ENABLED)
        #define UARTM_RX_VECT_NUM            (uint8)UARTM_RXInternalInterrupt__INTC_NUMBER
        #define UARTM_RX_PRIOR_NUM           (uint8)UARTM_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* UARTM_RX_INTERRUPT_ENABLED */
    #define UARTM_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define UARTM_RX_STS_BREAK_SHIFT             (0x01u)
    #define UARTM_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define UARTM_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define UARTM_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define UARTM_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define UARTM_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define UARTM_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define UARTM_RX_STS_MRKSPC           (uint8)(0x01u << UARTM_RX_STS_MRKSPC_SHIFT)
    #define UARTM_RX_STS_BREAK            (uint8)(0x01u << UARTM_RX_STS_BREAK_SHIFT)
    #define UARTM_RX_STS_PAR_ERROR        (uint8)(0x01u << UARTM_RX_STS_PAR_ERROR_SHIFT)
    #define UARTM_RX_STS_STOP_ERROR       (uint8)(0x01u << UARTM_RX_STS_STOP_ERROR_SHIFT)
    #define UARTM_RX_STS_OVERRUN          (uint8)(0x01u << UARTM_RX_STS_OVERRUN_SHIFT)
    #define UARTM_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << UARTM_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define UARTM_RX_STS_ADDR_MATCH       (uint8)(0x01u << UARTM_RX_STS_ADDR_MATCH_SHIFT)
    #define UARTM_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << UARTM_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define UARTM_RX_HW_MASK                     (0x7Fu)
#endif /* End (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) */

/* Control Register definitions */
#define UARTM_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define UARTM_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define UARTM_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define UARTM_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define UARTM_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define UARTM_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define UARTM_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define UARTM_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define UARTM_CTRL_HD_SEND               (uint8)(0x01u << UARTM_CTRL_HD_SEND_SHIFT)
#define UARTM_CTRL_HD_SEND_BREAK         (uint8)(0x01u << UARTM_CTRL_HD_SEND_BREAK_SHIFT)
#define UARTM_CTRL_MARK                  (uint8)(0x01u << UARTM_CTRL_MARK_SHIFT)
#define UARTM_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << UARTM_CTRL_PARITY_TYPE0_SHIFT)
#define UARTM_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << UARTM_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define UARTM_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define UARTM_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define UARTM_SEND_BREAK                         (0x00u)
#define UARTM_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define UARTM_REINIT                             (0x02u)
#define UARTM_SEND_WAIT_REINIT                   (0x03u)

#define UARTM_OVER_SAMPLE_8                      (8u)
#define UARTM_OVER_SAMPLE_16                     (16u)

#define UARTM_BIT_CENTER                         (UARTM_OVER_SAMPLE_COUNT - 1u)

#define UARTM_FIFO_LENGTH                        (4u)
#define UARTM_NUMBER_OF_START_BIT                (1u)
#define UARTM_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation*/
#define UARTM_TXBITCTR_BREAKBITS8X   ((UARTM_BREAK_BITS_TX * UARTM_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation*/
#define UARTM_TXBITCTR_BREAKBITS ((UARTM_BREAK_BITS_TX * UARTM_OVER_SAMPLE_COUNT) - 1u)

#define UARTM_HALF_BIT_COUNT   \
                            (((UARTM_OVER_SAMPLE_COUNT / 2u) + (UARTM_USE23POLLING * 1u)) - 2u)
#if (UARTM_OVER_SAMPLE_COUNT == UARTM_OVER_SAMPLE_8)
    #define UARTM_HD_TXBITCTR_INIT   (((UARTM_BREAK_BITS_TX + \
                            UARTM_NUMBER_OF_START_BIT) * UARTM_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define UARTM_RXBITCTR_INIT  ((((UARTM_BREAK_BITS_RX + UARTM_NUMBER_OF_START_BIT) \
                            * UARTM_OVER_SAMPLE_COUNT) + UARTM_HALF_BIT_COUNT) - 1u)


#else /* UARTM_OVER_SAMPLE_COUNT == UARTM_OVER_SAMPLE_16 */
    #define UARTM_HD_TXBITCTR_INIT   ((8u * UARTM_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount=16 */
    #define UARTM_RXBITCTR_INIT      (((7u * UARTM_OVER_SAMPLE_COUNT) - 1u) + \
                                                      UARTM_HALF_BIT_COUNT)
#endif /* End UARTM_OVER_SAMPLE_COUNT */
#define UARTM_HD_RXBITCTR_INIT                   UARTM_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 UARTM_initVar;
#if( UARTM_TX_ENABLED && (UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH))
    extern volatile uint8 UARTM_txBuffer[UARTM_TXBUFFERSIZE];
    extern volatile uint8 UARTM_txBufferRead;
    extern uint8 UARTM_txBufferWrite;
#endif /* End UARTM_TX_ENABLED */
#if( ( UARTM_RX_ENABLED || UARTM_HD_ENABLED ) && \
     (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH) )
    extern volatile uint8 UARTM_rxBuffer[UARTM_RXBUFFERSIZE];
    extern volatile uint8 UARTM_rxBufferRead;
    extern volatile uint8 UARTM_rxBufferWrite;
    extern volatile uint8 UARTM_rxBufferLoopDetect;
    extern volatile uint8 UARTM_rxBufferOverflow;
    #if (UARTM_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 UARTM_rxAddressMode;
        extern volatile uint8 UARTM_rxAddressDetected;
    #endif /* End EnableHWAddress */
#endif /* End UARTM_RX_ENABLED */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define UARTM__B_UART__AM_SW_BYTE_BYTE 1
#define UARTM__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define UARTM__B_UART__AM_HW_BYTE_BY_BYTE 3
#define UARTM__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define UARTM__B_UART__AM_NONE 0

#define UARTM__B_UART__NONE_REVB 0
#define UARTM__B_UART__EVEN_REVB 1
#define UARTM__B_UART__ODD_REVB 2
#define UARTM__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define UARTM_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define UARTM_NUMBER_OF_STOP_BITS    (1u)

#if (UARTM_RXHW_ADDRESS_ENABLED)
    #define UARTM_RXADDRESSMODE      (0u)
    #define UARTM_RXHWADDRESS1       (0u)
    #define UARTM_RXHWADDRESS2       (0u)
    /* Backward compatible define */
    #define UARTM_RXAddressMode      UARTM_RXADDRESSMODE
#endif /* End EnableHWAddress */

#define UARTM_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << UARTM_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << UARTM_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << UARTM_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << UARTM_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << UARTM_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << UARTM_RX_STS_BREAK_SHIFT) \
                                        | (0 << UARTM_RX_STS_OVERRUN_SHIFT))

#define UARTM_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << UARTM_TX_STS_COMPLETE_SHIFT) \
                                        | (1 << UARTM_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << UARTM_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << UARTM_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef UARTM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define UARTM_CONTROL_REG \
                            (* (reg8 *) UARTM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define UARTM_CONTROL_PTR \
                            (  (reg8 *) UARTM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End UARTM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(UARTM_TX_ENABLED)
    #define UARTM_TXDATA_REG          (* (reg8 *) UARTM_BUART_sTX_TxShifter_u0__F0_REG)
    #define UARTM_TXDATA_PTR          (  (reg8 *) UARTM_BUART_sTX_TxShifter_u0__F0_REG)
    #define UARTM_TXDATA_AUX_CTL_REG  (* (reg8 *) UARTM_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define UARTM_TXDATA_AUX_CTL_PTR  (  (reg8 *) UARTM_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define UARTM_TXSTATUS_REG        (* (reg8 *) UARTM_BUART_sTX_TxSts__STATUS_REG)
    #define UARTM_TXSTATUS_PTR        (  (reg8 *) UARTM_BUART_sTX_TxSts__STATUS_REG)
    #define UARTM_TXSTATUS_MASK_REG   (* (reg8 *) UARTM_BUART_sTX_TxSts__MASK_REG)
    #define UARTM_TXSTATUS_MASK_PTR   (  (reg8 *) UARTM_BUART_sTX_TxSts__MASK_REG)
    #define UARTM_TXSTATUS_ACTL_REG   (* (reg8 *) UARTM_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define UARTM_TXSTATUS_ACTL_PTR   (  (reg8 *) UARTM_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(UARTM_TXCLKGEN_DP)
        #define UARTM_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define UARTM_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define UARTM_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define UARTM_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define UARTM_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define UARTM_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define UARTM_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define UARTM_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define UARTM_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define UARTM_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) UARTM_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* UARTM_TXCLKGEN_DP */

#endif /* End UARTM_TX_ENABLED */

#if(UARTM_HD_ENABLED)

    #define UARTM_TXDATA_REG             (* (reg8 *) UARTM_BUART_sRX_RxShifter_u0__F1_REG )
    #define UARTM_TXDATA_PTR             (  (reg8 *) UARTM_BUART_sRX_RxShifter_u0__F1_REG )
    #define UARTM_TXDATA_AUX_CTL_REG     (* (reg8 *) UARTM_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define UARTM_TXDATA_AUX_CTL_PTR     (  (reg8 *) UARTM_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define UARTM_TXSTATUS_REG           (* (reg8 *) UARTM_BUART_sRX_RxSts__STATUS_REG )
    #define UARTM_TXSTATUS_PTR           (  (reg8 *) UARTM_BUART_sRX_RxSts__STATUS_REG )
    #define UARTM_TXSTATUS_MASK_REG      (* (reg8 *) UARTM_BUART_sRX_RxSts__MASK_REG )
    #define UARTM_TXSTATUS_MASK_PTR      (  (reg8 *) UARTM_BUART_sRX_RxSts__MASK_REG )
    #define UARTM_TXSTATUS_ACTL_REG      (* (reg8 *) UARTM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define UARTM_TXSTATUS_ACTL_PTR      (  (reg8 *) UARTM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End UARTM_HD_ENABLED */

#if( (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) )
    #define UARTM_RXDATA_REG             (* (reg8 *) UARTM_BUART_sRX_RxShifter_u0__F0_REG )
    #define UARTM_RXDATA_PTR             (  (reg8 *) UARTM_BUART_sRX_RxShifter_u0__F0_REG )
    #define UARTM_RXADDRESS1_REG         (* (reg8 *) UARTM_BUART_sRX_RxShifter_u0__D0_REG )
    #define UARTM_RXADDRESS1_PTR         (  (reg8 *) UARTM_BUART_sRX_RxShifter_u0__D0_REG )
    #define UARTM_RXADDRESS2_REG         (* (reg8 *) UARTM_BUART_sRX_RxShifter_u0__D1_REG )
    #define UARTM_RXADDRESS2_PTR         (  (reg8 *) UARTM_BUART_sRX_RxShifter_u0__D1_REG )
    #define UARTM_RXDATA_AUX_CTL_REG     (* (reg8 *) UARTM_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define UARTM_RXBITCTR_PERIOD_REG    (* (reg8 *) UARTM_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define UARTM_RXBITCTR_PERIOD_PTR    (  (reg8 *) UARTM_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define UARTM_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) UARTM_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define UARTM_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) UARTM_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define UARTM_RXBITCTR_COUNTER_REG   (* (reg8 *) UARTM_BUART_sRX_RxBitCounter__COUNT_REG )
    #define UARTM_RXBITCTR_COUNTER_PTR   (  (reg8 *) UARTM_BUART_sRX_RxBitCounter__COUNT_REG )

    #define UARTM_RXSTATUS_REG           (* (reg8 *) UARTM_BUART_sRX_RxSts__STATUS_REG )
    #define UARTM_RXSTATUS_PTR           (  (reg8 *) UARTM_BUART_sRX_RxSts__STATUS_REG )
    #define UARTM_RXSTATUS_MASK_REG      (* (reg8 *) UARTM_BUART_sRX_RxSts__MASK_REG )
    #define UARTM_RXSTATUS_MASK_PTR      (  (reg8 *) UARTM_BUART_sRX_RxSts__MASK_REG )
    #define UARTM_RXSTATUS_ACTL_REG      (* (reg8 *) UARTM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define UARTM_RXSTATUS_ACTL_PTR      (  (reg8 *) UARTM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) */

#if(UARTM_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define UARTM_INTCLOCK_CLKEN_REG     (* (reg8 *) UARTM_IntClock__PM_ACT_CFG)
    #define UARTM_INTCLOCK_CLKEN_PTR     (  (reg8 *) UARTM_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define UARTM_INTCLOCK_CLKEN_MASK    UARTM_IntClock__PM_ACT_MSK
#endif /* End UARTM_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(UARTM_TX_ENABLED)
    #define UARTM_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End UARTM_TX_ENABLED */

#if(UARTM_HD_ENABLED)
    #define UARTM_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End UARTM_HD_ENABLED */

#if( (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) )
    #define UARTM_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) */


/***************************************
* Renamed global variables or defines
* for backward compatible
***************************************/

#define UARTM_initvar                    UARTM_initVar

#define UARTM_RX_Enabled                 UARTM_RX_ENABLED
#define UARTM_TX_Enabled                 UARTM_TX_ENABLED
#define UARTM_HD_Enabled                 UARTM_HD_ENABLED
#define UARTM_RX_IntInterruptEnabled     UARTM_RX_INTERRUPT_ENABLED
#define UARTM_TX_IntInterruptEnabled     UARTM_TX_INTERRUPT_ENABLED
#define UARTM_InternalClockUsed          UARTM_INTERNAL_CLOCK_USED
#define UARTM_RXHW_Address_Enabled       UARTM_RXHW_ADDRESS_ENABLED
#define UARTM_OverSampleCount            UARTM_OVER_SAMPLE_COUNT
#define UARTM_ParityType                 UARTM_PARITY_TYPE

#if( UARTM_TX_ENABLED && (UARTM_TXBUFFERSIZE > UARTM_FIFO_LENGTH))
    #define UARTM_TXBUFFER               UARTM_txBuffer
    #define UARTM_TXBUFFERREAD           UARTM_txBufferRead
    #define UARTM_TXBUFFERWRITE          UARTM_txBufferWrite
#endif /* End UARTM_TX_ENABLED */
#if( ( UARTM_RX_ENABLED || UARTM_HD_ENABLED ) && \
     (UARTM_RXBUFFERSIZE > UARTM_FIFO_LENGTH) )
    #define UARTM_RXBUFFER               UARTM_rxBuffer
    #define UARTM_RXBUFFERREAD           UARTM_rxBufferRead
    #define UARTM_RXBUFFERWRITE          UARTM_rxBufferWrite
    #define UARTM_RXBUFFERLOOPDETECT     UARTM_rxBufferLoopDetect
    #define UARTM_RXBUFFER_OVERFLOW      UARTM_rxBufferOverflow
#endif /* End UARTM_RX_ENABLED */

#ifdef UARTM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define UARTM_CONTROL                UARTM_CONTROL_REG
#endif /* End UARTM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(UARTM_TX_ENABLED)
    #define UARTM_TXDATA                 UARTM_TXDATA_REG
    #define UARTM_TXSTATUS               UARTM_TXSTATUS_REG
    #define UARTM_TXSTATUS_MASK          UARTM_TXSTATUS_MASK_REG
    #define UARTM_TXSTATUS_ACTL          UARTM_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(UARTM_TXCLKGEN_DP)
        #define UARTM_TXBITCLKGEN_CTR        UARTM_TXBITCLKGEN_CTR_REG
        #define UARTM_TXBITCLKTX_COMPLETE    UARTM_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define UARTM_TXBITCTR_PERIOD        UARTM_TXBITCTR_PERIOD_REG
        #define UARTM_TXBITCTR_CONTROL       UARTM_TXBITCTR_CONTROL_REG
        #define UARTM_TXBITCTR_COUNTER       UARTM_TXBITCTR_COUNTER_REG
    #endif /* UARTM_TXCLKGEN_DP */
#endif /* End UARTM_TX_ENABLED */

#if(UARTM_HD_ENABLED)
    #define UARTM_TXDATA                 UARTM_TXDATA_REG
    #define UARTM_TXSTATUS               UARTM_TXSTATUS_REG
    #define UARTM_TXSTATUS_MASK          UARTM_TXSTATUS_MASK_REG
    #define UARTM_TXSTATUS_ACTL          UARTM_TXSTATUS_ACTL_REG
#endif /* End UARTM_HD_ENABLED */

#if( (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) )
    #define UARTM_RXDATA                 UARTM_RXDATA_REG
    #define UARTM_RXADDRESS1             UARTM_RXADDRESS1_REG
    #define UARTM_RXADDRESS2             UARTM_RXADDRESS2_REG
    #define UARTM_RXBITCTR_PERIOD        UARTM_RXBITCTR_PERIOD_REG
    #define UARTM_RXBITCTR_CONTROL       UARTM_RXBITCTR_CONTROL_REG
    #define UARTM_RXBITCTR_COUNTER       UARTM_RXBITCTR_COUNTER_REG
    #define UARTM_RXSTATUS               UARTM_RXSTATUS_REG
    #define UARTM_RXSTATUS_MASK          UARTM_RXSTATUS_MASK_REG
    #define UARTM_RXSTATUS_ACTL          UARTM_RXSTATUS_ACTL_REG
#endif /* End  (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) */

#if(UARTM_INTERNAL_CLOCK_USED)
    #define UARTM_INTCLOCK_CLKEN         UARTM_INTCLOCK_CLKEN_REG
#endif /* End UARTM_INTERNAL_CLOCK_USED */

#define UARTM_WAIT_FOR_COMLETE_REINIT    UARTM_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_UARTM_H */


/* [] END OF FILE */
