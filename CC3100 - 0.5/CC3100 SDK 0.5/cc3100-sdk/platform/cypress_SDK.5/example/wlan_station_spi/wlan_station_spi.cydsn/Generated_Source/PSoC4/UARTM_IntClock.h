/*******************************************************************************
* File Name: UARTM_IntClock.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_UARTM_IntClock_H)
#define CY_CLOCK_UARTM_IntClock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void UARTM_IntClock_StartEx(uint32 alignClkDiv);
#define UARTM_IntClock_Start() \
    UARTM_IntClock_StartEx(UARTM_IntClock__PA_DIV_ID)

#else

void UARTM_IntClock_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void UARTM_IntClock_Stop(void);

void UARTM_IntClock_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 UARTM_IntClock_GetDividerRegister(void);
uint8  UARTM_IntClock_GetFractionalDividerRegister(void);

#define UARTM_IntClock_Enable()                         UARTM_IntClock_Start()
#define UARTM_IntClock_Disable()                        UARTM_IntClock_Stop()
#define UARTM_IntClock_SetDividerRegister(clkDivider, reset)  \
    UARTM_IntClock_SetFractionalDividerRegister((clkDivider), 0u)
#define UARTM_IntClock_SetDivider(clkDivider)           UARTM_IntClock_SetDividerRegister((clkDivider), 1u)
#define UARTM_IntClock_SetDividerValue(clkDivider)      UARTM_IntClock_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define UARTM_IntClock_DIV_ID     UARTM_IntClock__DIV_ID

#define UARTM_IntClock_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define UARTM_IntClock_CTRL_REG   (*(reg32 *)UARTM_IntClock__CTRL_REGISTER)
#define UARTM_IntClock_DIV_REG    (*(reg32 *)UARTM_IntClock__DIV_REGISTER)

#define UARTM_IntClock_CMD_DIV_SHIFT          (0u)
#define UARTM_IntClock_CMD_PA_DIV_SHIFT       (8u)
#define UARTM_IntClock_CMD_DISABLE_SHIFT      (30u)
#define UARTM_IntClock_CMD_ENABLE_SHIFT       (31u)

#define UARTM_IntClock_CMD_DISABLE_MASK       ((uint32)((uint32)1u << UARTM_IntClock_CMD_DISABLE_SHIFT))
#define UARTM_IntClock_CMD_ENABLE_MASK        ((uint32)((uint32)1u << UARTM_IntClock_CMD_ENABLE_SHIFT))

#define UARTM_IntClock_DIV_FRAC_MASK  (0x000000F8u)
#define UARTM_IntClock_DIV_FRAC_SHIFT (3u)
#define UARTM_IntClock_DIV_INT_MASK   (0xFFFFFF00u)
#define UARTM_IntClock_DIV_INT_SHIFT  (8u)

#else 

#define UARTM_IntClock_DIV_REG        (*(reg32 *)UARTM_IntClock__REGISTER)
#define UARTM_IntClock_ENABLE_REG     UARTM_IntClock_DIV_REG
#define UARTM_IntClock_DIV_FRAC_MASK  UARTM_IntClock__FRAC_MASK
#define UARTM_IntClock_DIV_FRAC_SHIFT (16u)
#define UARTM_IntClock_DIV_INT_MASK   UARTM_IntClock__DIVIDER_MASK
#define UARTM_IntClock_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_UARTM_IntClock_H) */

/* [] END OF FILE */
