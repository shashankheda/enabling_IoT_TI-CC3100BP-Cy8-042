/*******************************************************************************
* File Name: IRQ_Pin.h  
* Version 2.0
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_IRQ_Pin_H) /* Pins IRQ_Pin_H */
#define CY_PINS_IRQ_Pin_H

#include "cytypes.h"
#include "cyfitter.h"
#include "IRQ_Pin_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    IRQ_Pin_Write(uint8 value) ;
void    IRQ_Pin_SetDriveMode(uint8 mode) ;
uint8   IRQ_Pin_ReadDataReg(void) ;
uint8   IRQ_Pin_Read(void) ;
uint8   IRQ_Pin_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define IRQ_Pin_DRIVE_MODE_BITS        (3)
#define IRQ_Pin_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - IRQ_Pin_DRIVE_MODE_BITS))
#define IRQ_Pin_DRIVE_MODE_SHIFT       (0x00u)
#define IRQ_Pin_DRIVE_MODE_MASK        (0x07u << IRQ_Pin_DRIVE_MODE_SHIFT)

#define IRQ_Pin_DM_ALG_HIZ         (0x00u << IRQ_Pin_DRIVE_MODE_SHIFT)
#define IRQ_Pin_DM_DIG_HIZ         (0x01u << IRQ_Pin_DRIVE_MODE_SHIFT)
#define IRQ_Pin_DM_RES_UP          (0x02u << IRQ_Pin_DRIVE_MODE_SHIFT)
#define IRQ_Pin_DM_RES_DWN         (0x03u << IRQ_Pin_DRIVE_MODE_SHIFT)
#define IRQ_Pin_DM_OD_LO           (0x04u << IRQ_Pin_DRIVE_MODE_SHIFT)
#define IRQ_Pin_DM_OD_HI           (0x05u << IRQ_Pin_DRIVE_MODE_SHIFT)
#define IRQ_Pin_DM_STRONG          (0x06u << IRQ_Pin_DRIVE_MODE_SHIFT)
#define IRQ_Pin_DM_RES_UPDWN       (0x07u << IRQ_Pin_DRIVE_MODE_SHIFT)

/* Digital Port Constants */
#define IRQ_Pin_MASK               IRQ_Pin__MASK
#define IRQ_Pin_SHIFT              IRQ_Pin__SHIFT
#define IRQ_Pin_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define IRQ_Pin_PS                     (* (reg32 *) IRQ_Pin__PS)
/* Port Configuration */
#define IRQ_Pin_PC                     (* (reg32 *) IRQ_Pin__PC)
/* Data Register */
#define IRQ_Pin_DR                     (* (reg32 *) IRQ_Pin__DR)
/* Input Buffer Disable Override */
#define IRQ_Pin_INP_DIS                (* (reg32 *) IRQ_Pin__PC2)


#if defined(IRQ_Pin__INTSTAT)  /* Interrupt Registers */

    #define IRQ_Pin_INTSTAT                (* (reg32 *) IRQ_Pin__INTSTAT)

#endif /* Interrupt Registers */

#endif /* End Pins IRQ_Pin_H */


/* [] END OF FILE */
