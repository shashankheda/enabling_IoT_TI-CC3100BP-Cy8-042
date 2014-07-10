/*******************************************************************************
* File Name: m_mosi_pin.c  
* Version 2.0
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "m_mosi_pin.h"

#define SetP4PinDriveMode(shift, mode)  \
    do { \
        m_mosi_pin_PC =   (m_mosi_pin_PC & \
                                (uint32)(~(uint32)(m_mosi_pin_DRIVE_MODE_IND_MASK << (m_mosi_pin_DRIVE_MODE_BITS * (shift))))) | \
                                (uint32)((uint32)(mode) << (m_mosi_pin_DRIVE_MODE_BITS * (shift))); \
    } while (0)


/*******************************************************************************
* Function Name: m_mosi_pin_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None 
*  
*******************************************************************************/
void m_mosi_pin_Write(uint8 value) 
{
    uint8 drVal = (uint8)(m_mosi_pin_DR & (uint8)(~m_mosi_pin_MASK));
    drVal = (drVal | ((uint8)(value << m_mosi_pin_SHIFT) & m_mosi_pin_MASK));
    m_mosi_pin_DR = (uint32)drVal;
}


/*******************************************************************************
* Function Name: m_mosi_pin_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  m_mosi_pin_DM_STRONG     Strong Drive 
*  m_mosi_pin_DM_OD_HI      Open Drain, Drives High 
*  m_mosi_pin_DM_OD_LO      Open Drain, Drives Low 
*  m_mosi_pin_DM_RES_UP     Resistive Pull Up 
*  m_mosi_pin_DM_RES_DWN    Resistive Pull Down 
*  m_mosi_pin_DM_RES_UPDWN  Resistive Pull Up/Down 
*  m_mosi_pin_DM_DIG_HIZ    High Impedance Digital 
*  m_mosi_pin_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void m_mosi_pin_SetDriveMode(uint8 mode) 
{
	SetP4PinDriveMode(m_mosi_pin__0__SHIFT, mode);
}


/*******************************************************************************
* Function Name: m_mosi_pin_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro m_mosi_pin_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 m_mosi_pin_Read(void) 
{
    return (uint8)((m_mosi_pin_PS & m_mosi_pin_MASK) >> m_mosi_pin_SHIFT);
}


/*******************************************************************************
* Function Name: m_mosi_pin_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 m_mosi_pin_ReadDataReg(void) 
{
    return (uint8)((m_mosi_pin_DR & m_mosi_pin_MASK) >> m_mosi_pin_SHIFT);
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(m_mosi_pin_INTSTAT) 

    /*******************************************************************************
    * Function Name: m_mosi_pin_ClearInterrupt
    ********************************************************************************
    *
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 m_mosi_pin_ClearInterrupt(void) 
    {
		uint8 maskedStatus = (uint8)(m_mosi_pin_INTSTAT & m_mosi_pin_MASK);
		m_mosi_pin_INTSTAT = maskedStatus;
        return maskedStatus >> m_mosi_pin_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 


/* [] END OF FILE */
