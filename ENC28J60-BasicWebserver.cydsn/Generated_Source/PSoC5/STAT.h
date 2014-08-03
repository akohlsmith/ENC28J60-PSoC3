/*******************************************************************************
* File Name: STAT.h  
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

#if !defined(CY_PINS_STAT_H) /* Pins STAT_H */
#define CY_PINS_STAT_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "STAT_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v2_0 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 STAT__PORT == 15 && ((STAT__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    STAT_Write(uint8 value) ;
void    STAT_SetDriveMode(uint8 mode) ;
uint8   STAT_ReadDataReg(void) ;
uint8   STAT_Read(void) ;
uint8   STAT_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define STAT_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define STAT_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define STAT_DM_RES_UP          PIN_DM_RES_UP
#define STAT_DM_RES_DWN         PIN_DM_RES_DWN
#define STAT_DM_OD_LO           PIN_DM_OD_LO
#define STAT_DM_OD_HI           PIN_DM_OD_HI
#define STAT_DM_STRONG          PIN_DM_STRONG
#define STAT_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define STAT_MASK               STAT__MASK
#define STAT_SHIFT              STAT__SHIFT
#define STAT_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define STAT_PS                     (* (reg8 *) STAT__PS)
/* Data Register */
#define STAT_DR                     (* (reg8 *) STAT__DR)
/* Port Number */
#define STAT_PRT_NUM                (* (reg8 *) STAT__PRT) 
/* Connect to Analog Globals */                                                  
#define STAT_AG                     (* (reg8 *) STAT__AG)                       
/* Analog MUX bux enable */
#define STAT_AMUX                   (* (reg8 *) STAT__AMUX) 
/* Bidirectional Enable */                                                        
#define STAT_BIE                    (* (reg8 *) STAT__BIE)
/* Bit-mask for Aliased Register Access */
#define STAT_BIT_MASK               (* (reg8 *) STAT__BIT_MASK)
/* Bypass Enable */
#define STAT_BYP                    (* (reg8 *) STAT__BYP)
/* Port wide control signals */                                                   
#define STAT_CTL                    (* (reg8 *) STAT__CTL)
/* Drive Modes */
#define STAT_DM0                    (* (reg8 *) STAT__DM0) 
#define STAT_DM1                    (* (reg8 *) STAT__DM1)
#define STAT_DM2                    (* (reg8 *) STAT__DM2) 
/* Input Buffer Disable Override */
#define STAT_INP_DIS                (* (reg8 *) STAT__INP_DIS)
/* LCD Common or Segment Drive */
#define STAT_LCD_COM_SEG            (* (reg8 *) STAT__LCD_COM_SEG)
/* Enable Segment LCD */
#define STAT_LCD_EN                 (* (reg8 *) STAT__LCD_EN)
/* Slew Rate Control */
#define STAT_SLW                    (* (reg8 *) STAT__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define STAT_PRTDSI__CAPS_SEL       (* (reg8 *) STAT__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define STAT_PRTDSI__DBL_SYNC_IN    (* (reg8 *) STAT__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define STAT_PRTDSI__OE_SEL0        (* (reg8 *) STAT__PRTDSI__OE_SEL0) 
#define STAT_PRTDSI__OE_SEL1        (* (reg8 *) STAT__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define STAT_PRTDSI__OUT_SEL0       (* (reg8 *) STAT__PRTDSI__OUT_SEL0) 
#define STAT_PRTDSI__OUT_SEL1       (* (reg8 *) STAT__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define STAT_PRTDSI__SYNC_OUT       (* (reg8 *) STAT__PRTDSI__SYNC_OUT) 


#if defined(STAT__INTSTAT)  /* Interrupt Registers */

    #define STAT_INTSTAT                (* (reg8 *) STAT__INTSTAT)
    #define STAT_SNAP                   (* (reg8 *) STAT__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_STAT_H */


/* [] END OF FILE */
