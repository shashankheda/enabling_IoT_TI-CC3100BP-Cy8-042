/*******************************************************************************
* File Name: nHIB.h  
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

#if !defined(CY_PINS_nHIB_H) /* Pins nHIB_H */
#define CY_PINS_nHIB_H

#include "cytypes.h"
#include "cyfitter.h"
#include "nHIB_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    nHIB_Write(uint8 value) ;
void    nHIB_SetDriveMode(uint8 mode) ;
uint8   nHIB_ReadDataReg(void) ;
uint8   nHIB_Read(void) ;
uint8   nHIB_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define nHIB_DRIVE_MODE_BITS        (3)
#define nHIB_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - nHIB_DRIVE_MODE_BITS))
#define nHIB_DRIVE_MODE_SHIFT       (0x00u)
#define nHIB_DRIVE_MODE_MASK        (0x07u << nHIB_DRIVE_MODE_SHIFT)

#define nHIB_DM_ALG_HIZ         (0x00u << nHIB_DRIVE_MODE_SHIFT)
#define nHIB_DM_DIG_HIZ         (0x01u << nHIB_DRIVE_MODE_SHIFT)
#define nHIB_DM_RES_UP          (0x02u << nHIB_DRIVE_MODE_SHIFT)
#define nHIB_DM_RES_DWN         (0x03u << nHIB_DRIVE_MODE_SHIFT)
#define nHIB_DM_OD_LO           (0x04u << nHIB_DRIVE_MODE_SHIFT)
#define nHIB_DM_OD_HI           (0x05u << nHIB_DRIVE_MODE_SHIFT)
#define nHIB_DM_STRONG          (0x06u << nHIB_DRIVE_MODE_SHIFT)
#define nHIB_DM_RES_UPDWN       (0x07u << nHIB_DRIVE_MODE_SHIFT)

/* Digital Port Constants */
#define nHIB_MASK               nHIB__MASK
#define nHIB_SHIFT              nHIB__SHIFT
#define nHIB_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define nHIB_PS                     (* (reg32 *) nHIB__PS)
/* Port Configuration */
#define nHIB_PC                     (* (reg32 *) nHIB__PC)
/* Data Register */
#define nHIB_DR                     (* (reg32 *) nHIB__DR)
/* Input Buffer Disable Override */
#define nHIB_INP_DIS                (* (reg32 *) nHIB__PC2)


#if defined(nHIB__INTSTAT)  /* Interrupt Registers */

    #define nHIB_INTSTAT                (* (reg32 *) nHIB__INTSTAT)

#endif /* Interrupt Registers */

#endif /* End Pins nHIB_H */


/* [] END OF FILE */
