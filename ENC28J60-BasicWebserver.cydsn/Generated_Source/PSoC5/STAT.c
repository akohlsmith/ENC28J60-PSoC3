/*******************************************************************************
* File Name: STAT.c  
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
#include "STAT.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 STAT__PORT == 15 && ((STAT__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: STAT_Write
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
void STAT_Write(uint8 value) 
{
    uint8 staticBits = (STAT_DR & (uint8)(~STAT_MASK));
    STAT_DR = staticBits | ((uint8)(value << STAT_SHIFT) & STAT_MASK);
}


/*******************************************************************************
* Function Name: STAT_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  STAT_DM_STRONG     Strong Drive 
*  STAT_DM_OD_HI      Open Drain, Drives High 
*  STAT_DM_OD_LO      Open Drain, Drives Low 
*  STAT_DM_RES_UP     Resistive Pull Up 
*  STAT_DM_RES_DWN    Resistive Pull Down 
*  STAT_DM_RES_UPDWN  Resistive Pull Up/Down 
*  STAT_DM_DIG_HIZ    High Impedance Digital 
*  STAT_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void STAT_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(STAT_0, mode);
}


/*******************************************************************************
* Function Name: STAT_Read
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
*  Macro STAT_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 STAT_Read(void) 
{
    return (STAT_PS & STAT_MASK) >> STAT_SHIFT;
}


/*******************************************************************************
* Function Name: STAT_ReadDataReg
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
uint8 STAT_ReadDataReg(void) 
{
    return (STAT_DR & STAT_MASK) >> STAT_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(STAT_INTSTAT) 

    /*******************************************************************************
    * Function Name: STAT_ClearInterrupt
    ********************************************************************************
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
    uint8 STAT_ClearInterrupt(void) 
    {
        return (STAT_INTSTAT & STAT_MASK) >> STAT_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
