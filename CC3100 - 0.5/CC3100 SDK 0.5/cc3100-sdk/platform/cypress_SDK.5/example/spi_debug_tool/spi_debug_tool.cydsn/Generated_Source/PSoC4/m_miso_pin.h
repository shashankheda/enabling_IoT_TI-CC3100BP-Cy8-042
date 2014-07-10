/*******************************************************************************
* File Name: m_miso_pin.h  
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

#if !defined(CY_PINS_m_miso_pin_H) /* Pins m_miso_pin_H */
#define CY_PINS_m_miso_pin_H

#include "cytypes.h"
#include "cyfitter.h"
#include "m_miso_pin_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    m_miso_pin_Write(uint8 value) ;
void    m_miso_pin_SetDriveMode(uint8 mode) ;
uint8   m_miso_pin_ReadDataReg(void) ;
uint8   m_miso_pin_Read(void) ;
uint8   m_miso_pin_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define m_miso_pin_DRIVE_MODE_BITS        (3)
#define m_miso_pin_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - m_miso_pin_DRIVE_MODE_BITS))
#define m_miso_pin_DRIVE_MODE_SHIFT       (0x00u)
#define m_miso_pin_DRIVE_MODE_MASK        (0x07u << m_miso_pin_DRIVE_MODE_SHIFT)

#define m_miso_pin_DM_ALG_HIZ         (0x00u << m_miso_pin_DRIVE_MODE_SHIFT)
#define m_miso_pin_DM_DIG_HIZ         (0x01u << m_miso_pin_DRIVE_MODE_SHIFT)
#define m_miso_pin_DM_RES_UP          (0x02u << m_miso_pin_DRIVE_MODE_SHIFT)
#define m_miso_pin_DM_RES_DWN         (0x03u << m_miso_pin_DRIVE_MODE_SHIFT)
#define m_miso_pin_DM_OD_LO           (0x04u << m_miso_pin_DRIVE_MODE_SHIFT)
#define m_miso_pin_DM_OD_HI           (0x05u << m_miso_pin_DRIVE_MODE_SHIFT)
#define m_miso_pin_DM_STRONG          (0x06u << m_miso_pin_DRIVE_MODE_SHIFT)
#define m_miso_pin_DM_RES_UPDWN       (0x07u << m_miso_pin_DRIVE_MODE_SHIFT)

/* Digital Port Constants */
#define m_miso_pin_MASK               m_miso_pin__MASK
#define m_miso_pin_SHIFT              m_miso_pin__SHIFT
#define m_miso_pin_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define m_miso_pin_PS                     (* (reg32 *) m_miso_pin__PS)
/* Port Configuration */
#define m_miso_pin_PC                     (* (reg32 *) m_miso_pin__PC)
/* Data Register */
#define m_miso_pin_DR                     (* (reg32 *) m_miso_pin__DR)
/* Input Buffer Disable Override */
#define m_miso_pin_INP_DIS                (* (reg32 *) m_miso_pin__PC2)


#if defined(m_miso_pin__INTSTAT)  /* Interrupt Registers */

    #define m_miso_pin_INTSTAT                (* (reg32 *) m_miso_pin__INTSTAT)

#endif /* Interrupt Registers */

#endif /* End Pins m_miso_pin_H */


/* [] END OF FILE */
