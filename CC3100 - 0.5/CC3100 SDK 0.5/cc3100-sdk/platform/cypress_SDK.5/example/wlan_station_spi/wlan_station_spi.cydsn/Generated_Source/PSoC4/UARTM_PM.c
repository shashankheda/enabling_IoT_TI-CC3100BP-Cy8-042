/*******************************************************************************
* File Name: UARTM_PM.c
* Version 2.30
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
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


/***************************************
* Local data allocation
***************************************/

static UARTM_BACKUP_STRUCT  UARTM_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: UARTM_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UARTM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UARTM_SaveConfig(void)
{
    #if (CY_UDB_V0)

        #if(UARTM_CONTROL_REG_REMOVED == 0u)
            UARTM_backup.cr = UARTM_CONTROL_REG;
        #endif /* End UARTM_CONTROL_REG_REMOVED */

        #if( (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) )
            UARTM_backup.rx_period = UARTM_RXBITCTR_PERIOD_REG;
            UARTM_backup.rx_mask = UARTM_RXSTATUS_MASK_REG;
            #if (UARTM_RXHW_ADDRESS_ENABLED)
                UARTM_backup.rx_addr1 = UARTM_RXADDRESS1_REG;
                UARTM_backup.rx_addr2 = UARTM_RXADDRESS2_REG;
            #endif /* End UARTM_RXHW_ADDRESS_ENABLED */
        #endif /* End UARTM_RX_ENABLED | UARTM_HD_ENABLED*/

        #if(UARTM_TX_ENABLED)
            #if(UARTM_TXCLKGEN_DP)
                UARTM_backup.tx_clk_ctr = UARTM_TXBITCLKGEN_CTR_REG;
                UARTM_backup.tx_clk_compl = UARTM_TXBITCLKTX_COMPLETE_REG;
            #else
                UARTM_backup.tx_period = UARTM_TXBITCTR_PERIOD_REG;
            #endif /*End UARTM_TXCLKGEN_DP */
            UARTM_backup.tx_mask = UARTM_TXSTATUS_MASK_REG;
        #endif /*End UARTM_TX_ENABLED */


    #else /* CY_UDB_V1 */

        #if(UARTM_CONTROL_REG_REMOVED == 0u)
            UARTM_backup.cr = UARTM_CONTROL_REG;
        #endif /* End UARTM_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: UARTM_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UARTM_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UARTM_RestoreConfig(void)
{

    #if (CY_UDB_V0)

        #if(UARTM_CONTROL_REG_REMOVED == 0u)
            UARTM_CONTROL_REG = UARTM_backup.cr;
        #endif /* End UARTM_CONTROL_REG_REMOVED */

        #if( (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) )
            UARTM_RXBITCTR_PERIOD_REG = UARTM_backup.rx_period;
            UARTM_RXSTATUS_MASK_REG = UARTM_backup.rx_mask;
            #if (UARTM_RXHW_ADDRESS_ENABLED)
                UARTM_RXADDRESS1_REG = UARTM_backup.rx_addr1;
                UARTM_RXADDRESS2_REG = UARTM_backup.rx_addr2;
            #endif /* End UARTM_RXHW_ADDRESS_ENABLED */
        #endif  /* End (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) */

        #if(UARTM_TX_ENABLED)
            #if(UARTM_TXCLKGEN_DP)
                UARTM_TXBITCLKGEN_CTR_REG = UARTM_backup.tx_clk_ctr;
                UARTM_TXBITCLKTX_COMPLETE_REG = UARTM_backup.tx_clk_compl;
            #else
                UARTM_TXBITCTR_PERIOD_REG = UARTM_backup.tx_period;
            #endif /*End UARTM_TXCLKGEN_DP */
            UARTM_TXSTATUS_MASK_REG = UARTM_backup.tx_mask;
        #endif /*End UARTM_TX_ENABLED */

    #else /* CY_UDB_V1 */

        #if(UARTM_CONTROL_REG_REMOVED == 0u)
            UARTM_CONTROL_REG = UARTM_backup.cr;
        #endif /* End UARTM_CONTROL_REG_REMOVED */

    #endif  /* End CY_UDB_V0 */
}


/*******************************************************************************
* Function Name: UARTM_Sleep
********************************************************************************
*
* Summary:
*  Stops and saves the user configuration. Should be called
*  just prior to entering sleep.
*
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UARTM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UARTM_Sleep(void)
{

    #if(UARTM_RX_ENABLED || UARTM_HD_ENABLED)
        if((UARTM_RXSTATUS_ACTL_REG  & UARTM_INT_ENABLE) != 0u)
        {
            UARTM_backup.enableState = 1u;
        }
        else
        {
            UARTM_backup.enableState = 0u;
        }
    #else
        if((UARTM_TXSTATUS_ACTL_REG  & UARTM_INT_ENABLE) !=0u)
        {
            UARTM_backup.enableState = 1u;
        }
        else
        {
            UARTM_backup.enableState = 0u;
        }
    #endif /* End UARTM_RX_ENABLED || UARTM_HD_ENABLED*/

    UARTM_Stop();
    UARTM_SaveConfig();
}


/*******************************************************************************
* Function Name: UARTM_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called
*  just after awaking from sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  UARTM_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void UARTM_Wakeup(void)
{
    UARTM_RestoreConfig();
    #if( (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) )
        UARTM_ClearRxBuffer();
    #endif /* End (UARTM_RX_ENABLED) || (UARTM_HD_ENABLED) */
    #if(UARTM_TX_ENABLED || UARTM_HD_ENABLED)
        UARTM_ClearTxBuffer();
    #endif /* End UARTM_TX_ENABLED || UARTM_HD_ENABLED */

    if(UARTM_backup.enableState != 0u)
    {
        UARTM_Enable();
    }
}


/* [] END OF FILE */
