/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : ENC28J60 driver functions
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file defines underlying ENC28J60 driver.
*/

/*
 Basic ENC28J60 Driver Code for PSoC3
 Author : Kartik Mankad
 Date : 17-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This file defines the functions,structures and registers
               essential while dealing with the ENC28J60 chip.
*/

#ifndef ENC28J60_H
#define ENC28J60_H



/*******************************************************************************
* Function Name: initMAC
********************************************************************************
* Summary:
*   Initializes the ENC28J60 Chip.
*
* Parameters:
*   deviceMAC - The MAC Address to be assigned to the ENC28J60
*
* Returns:
*   nothing.
*
*******************************************************************************/
void initMAC(unsigned char* deviceMAC);


/*******************************************************************************
* Function Name: MACWrite
********************************************************************************
* Summary:
*   This function writes a packet to ENC28J60's buffer,and sends it.
*
* Parameters:
*   packet - The buffer that contains the packet to be written.
*   len - The length of the packet present in the buffer 'packet'
*
* Returns:
*   TRUE(0)- if the Packet was successfully transmitted.
*   FALSE(1) - if the Packet was not successfully transmitted.
*
*******************************************************************************/
unsigned char MACWrite(void *packet, uint16_t len);

/*******************************************************************************
* Function Name: MACRead
********************************************************************************
* Summary:
*   This function read a packet from ENC28J60's buffer,if there is one.
*
* Parameters:
*   packet - a pointer to a buffer of data that will hold the packet read.
*	maxLen - Maximum length of the packet that will be read.
*
* Returns:
*   the length of the packet read into the buffer pointed to by packet.
*
*******************************************************************************/
unsigned int MACRead(void *dest, uint16_t maxLen);

/*******************************************************************************
* Function Name: ReadChipRev
********************************************************************************
* Summary:
*   This function reads the silicon Revision of the chip.
*
* Parameters:
*   none.
*
* Returns:
*   The silicon revision of the chip.Could be either of
*   __________________________________________________________
*   Part Number| B1         | B4       | B5       | B7       |
*   ___________|____________|__________|__________|__________|
*   ENC28J60   | 0000 0010  | 0000 0100| 0000 0101| 0000 0110|
*   ___________|____________|__________|__________|__________|
*   See http://ww1.microchip.com/downloads/en/DeviceDoc/80349c.pdf
*
*******************************************************************************/
unsigned char ReadChipRev(void);

/*******************************************************************************
* Function Name: IsLinkUp
********************************************************************************
* Summary:
*   Returns the status of the PHY link,from PHSTAT2 register.
*   Do not call this before initMAC,since SPIM is started in InitMAC function.
*
* Parameters:
*   none.
*
* Returns:
*   0x01 - If link is up.
*   0x00 - If link is not up.
*
*******************************************************************************/
unsigned char IsLinkUp(void);



/*Structure defined to hold
bits from of the TX Status Vectors
See ENC28J60 datasheet Page 43,Table 7-1
*/
typedef union {
	unsigned char v[7];
	struct {
		unsigned int	ByteCount;
		unsigned char	CollisionCount:4;
		unsigned char	CRCError:1;
		unsigned char	LengthCheckError:1;
		unsigned char	LengthOutOfRange:1;
		unsigned char	Done:1;
		unsigned char	Multicast:1;
		unsigned char	Broadcast:1;
		unsigned char	PacketDefer:1;
		unsigned char	ExcessiveDefer:1;
		unsigned char	MaximumCollisions:1;
		unsigned char	LateCollision:1;
		unsigned char	Giant:1;
		unsigned char	Underrun:1;
		unsigned int	BytesTransmittedOnWire;
		unsigned char	ControlFrame:1;
		unsigned char	PAUSEControlFrame:1;
		unsigned char	BackpressureApplied:1;
		unsigned char	VLANTaggedFrame:1;
		unsigned char	Zeros:4;
	} bits;
} TXSTATUS;

/*Structure defined to hold
bits from of the RX Status Vectors
See ENC28J60 datasheet Page 46,Table 7-3
*/
typedef union {
	unsigned char v[6];
	struct {
		unsigned int    NextPacket;
        unsigned int	ByteCount;
		unsigned char	LongEvent:1;
		unsigned char	Reserved:1;
		unsigned char	CarrierEvent:1;
		unsigned char	Reserved2:1;
		unsigned char	CRCError:1;
		unsigned char	LenChkError:1;
		unsigned char	LenOutofRange:1;
		unsigned char	RxOk:1;
		unsigned char	RxMultiCast:1;
		unsigned char	RxBroadCast:1;
		unsigned char	DribbleNibble:1;
		unsigned char	RxCntrlFrame:1;
		unsigned char	RxPauseFrame:1;
		unsigned char	RxUkwnOpcode:1;
        unsigned char   RxVlan:1;
		unsigned char	Zeros:1;
	} bits;
} RXSTATUS;

/*Memory Organization of the
ENC28J60's 8kb circular buffer
See ENC28J60 datasheet Page 20,Figure 3-2
*/
#define RXSTART        0x0000
#define RXEND          0x0fff
#define TXSTART        0x1000
#define TXEND          0x1fff
#define RXMAXBUFLEN    RXEND - RXSTART
#define TXMAXBUFLEN    TXEND - TXSTART

/*Maximum length of a packet it can RX.*/
#define MAXFRAMELEN     1518

/*SPI Opcodes for the ENC28J60
See ENC28J60 datasheet Page 28,Table 4-1
*/
#define RBM_OP           0x3a //Read Buffer Memory.
#define WCR_OP           0x40 //Write Control Register.
#define WBM_OP           0x7a //Write Buffer Memory.
#define BFS_OP           0x80 //Bit Field Set.
#define BFC_OP           0xa0 //Bit Field Clear.
#define RESET_OP         0xff //Soft Reset.

// Bank 0 registers --------
#define ERDPTL		0x00
#define ERDPTH		0x01
#define EWRPTL		0x02
#define EWRPTH		0x03
#define ETXSTL		0x04
#define ETXSTH		0x05
#define ETXNDL		0x06
#define ETXNDH		0x07
#define ERXSTL		0x08
#define ERXSTH		0x09
#define ERXNDL		0x0A
#define ERXNDH		0x0B
#define ERXRDPTL	0x0C
#define ERXRDPTH	0x0D
#define ERXWRPTL	0x0E
#define ERXWRPTH	0x0F
#define EDMASTL		0x10
#define EDMASTH		0x11
#define EDMANDL		0x12
#define EDMANDH		0x13
#define EDMADSTL	0x14
#define EDMADSTH	0x15
#define EDMACSL		0x16
#define EDMACSH		0x17
#define EIE         0x1B
#define EIR         0x1C
#define ESTAT		0x1D
#define ECON2		0x1E
#define ECON1		0x1F


#define EHT0		0x00
#define EHT1		0x01
#define EHT2		0x02
#define EHT3		0x03
#define EHT4		0x04
#define EHT5		0x05
#define EHT6		0x06
#define EHT7		0x07
#define EPMM0		0x08
#define EPMM1		0x09
#define EPMM2		0x0A
#define EPMM3		0x0B
#define EPMM4		0x0C
#define EPMM5		0x0D
#define EPMM6		0x0E
#define EPMM7		0x0F
#define EPMCSL		0x10
#define EPMCSH		0x11
#define EPMOL		0x14
#define EPMOH		0x15
#define ERXFCON		0x18
#define EPKTCNT		0x19

// Bank 2 registers -----
#define MACON1		0x00
#define MACON3		0x02
#define MACON4		0x03
#define MABBIPG		0x04
#define MAIPGL		0x06
#define MAIPGH		0x07
#define MACLCON1	0x08
#define MACLCON2	0x09
#define MAMXFLL		0x0A
#define MAMXFLH		0x0B
#define MICMD		0x12
#define MIREGADR	0x14
#define MIWRL		0x16
#define MIWRH		0x17
#define MIRDL		0x18
#define MIRDH		0x19

// Bank 3 registers -----
#define MAADR5		0x00
#define MAADR6		0x01
#define MAADR3		0x02
#define MAADR4		0x03
#define MAADR1		0x04
#define MAADR2		0x05
#define EBSTSD		0x06
#define EBSTCON		0x307
#define EBSTCSL		0x08
#define EBSTCSH		0x09
#define MISTAT		0x0A
#define EREVID		0x12
#define ECOCON		0x15
#define EFLOCON		0x17
#define EPAUSL		0x18
#define EPAUSH		0x19

/******************************************************************************
* PHY Register Locations
******************************************************************************/
#define PHCON1	0x00
#define PHSTAT1	0x01
#define PHID1	0x02
#define PHID2	0x03
#define PHCON2	0x10
#define PHSTAT2	0x11
#define PHIE	0x12
#define PHIR	0x13
#define PHLCON	0x14

/******************************************************************************
* Individual Register Bits
******************************************************************************/
// ETH/MAC/MII bits

// EIE bits ----------
#define	EIE_INTIE		(1<<7)
#define	EIE_PKTIE		(1<<6)
#define	EIE_DMAIE		(1<<5)
#define	EIE_LINKIE		(1<<4)
#define	EIE_TXIE		(1<<3)
#define	EIE_TXERIE		(1<<1)
#define	EIE_RXERIE		(1)

// EIR bits ----------
#define	EIR_PKTIF		(1<<6)
#define	EIR_DMAIF		(1<<5)
#define	EIR_LINKIF		(1<<4)
#define	EIR_TXIF		(1<<3)
#define	EIR_TXERIF		(1<<1)
#define	EIR_RXERIF		(1)

// ESTAT bits ---------
#define	ESTAT_INT		(1<<7)
#define ESTAT_BUFER		(1<<6)
#define	ESTAT_LATECOL	(1<<4)
#define	ESTAT_RXBUSY	(1<<2)
#define	ESTAT_TXABRT	(1<<1)
#define	ESTAT_CLKRDY	(1)

// ECON2 bits --------
#define	ECON2_AUTOINC	(1<<7)
#define	ECON2_PKTDEC	(1<<6)
#define	ECON2_PWRSV		(1<<5)
#define	ECON2_VRPS		(1<<3)

// ECON1 bits --------
#define	ECON1_TXRST		(1<<7)
#define	ECON1_RXRST		(1<<6)
#define	ECON1_DMAST		(1<<5)
#define	ECON1_CSUMEN	(1<<4)
#define	ECON1_TXRTS		(1<<3)
#define	ECON1_RXEN		(1<<2)
#define	ECON1_BSEL1		(1<<1)
#define	ECON1_BSEL0		(1)
#define ECON1_BSEL      (3)

// ERXFCON bits ------
#define	ERXFCON_UCEN	(unsigned char)(1<<7)
#define	ERXFCON_ANDOR	(unsigned char)(1<<6)
#define	ERXFCON_CRCEN	(unsigned char)(1<<5)
#define	ERXFCON_PMEN	(unsigned char)(1<<4)
#define	ERXFCON_MPEN	(unsigned char)(1<<3)
#define	ERXFCON_HTEN	(unsigned char)(1<<2)
#define	ERXFCON_MCEN	(unsigned char)(1<<1)
#define	ERXFCON_BCEN	(unsigned char)(1)

// MACON1 bits --------
#define	MACON1_TXPAUS	(1<<3)
#define	MACON1_RXPAUS	(1<<2)
#define	MACON1_PASSALL	(1<<1)
#define	MACON1_MARXEN	(unsigned char)(1)

// MACON3 bits --------
#define	MACON3_PADCFG2	(1<<7)
#define	MACON3_PADCFG1	(1<<6)
#define	MACON3_PADCFG0	(1<<5)
#define	MACON3_TXCRCEN	(1<<4)
#define	MACON3_PHDREN	(1<<3)
#define	MACON3_HFRMEN	(1<<2)
#define	MACON3_FRMLNEN	(1<<1)
#define	MACON3_FULDPX	(1)

// MACON4 bits --------
#define	MACON4_DEFER	(1<<6)
#define	MACON4_BPEN		(1<<5)
#define	MACON4_NOBKOFF	(1<<4)

// MICMD bits ---------
#define	MICMD_MIISCAN	(1<<1)
#define	MICMD_MIIRD		(1)

// EBSTCON bits -----
#define	EBSTCON_PSV2	(1<<7)
#define	EBSTCON_PSV1	(1<<6)
#define	EBSTCON_PSV0	(1<<5)
#define	EBSTCON_PSEL	(1<<4)
#define	EBSTCON_TMSEL1	(1<<3)
#define	EBSTCON_TMSEL0	(1<<2)
#define	EBSTCON_TME		(1<<1)
#define	EBSTCON_BISTST	(1)

// MISTAT bits --------
#define	MISTAT_NVALID	(1<<2)
#define	MISTAT_SCAN		(1<<1)
#define	MISTAT_BUSY		(1)

// ECOCON bits -------
#define	ECOCON_COCON2	(1<<2)
#define	ECOCON_COCON1	(1<<1)
#define	ECOCON_COCON0	(1)

// EFLOCON bits -----
#define	EFLOCON_FULDPXS	(1<<2)
#define	EFLOCON_FCEN1	(1<<1)
#define	EFLOCON_FCEN0	(1)

// PHY bits

// PHCON1 bits ----------
#define	PHCON1_PRST	(1u<<15)
#define	PHCON1_PLOOPBK	(1u<<14)
#define	PHCON1_PPWRSV	(1u<<11)
#define	PHCON1_PDPXMD	(1u<<8)

// PHSTAT1 bits --------
#define	PHSTAT1_PFDPX	(1u<<12)
#define	PHSTAT1_PHDPX	(1u<<11)
#define	PHSTAT1_LLSTAT	(1u<<2)
#define	PHSTAT1_JBSTAT	(1u<<1)

// PHID2 bits --------
#define	PHID2_PID24		(1u<<15)
#define	PHID2_PID23		(1u<<14)
#define	PHID2_PID22		(1u<<13)
#define	PHID2_PID21		(1u<<12)
#define	PHID2_PID20		(1u<<11)
#define	PHID2_PID19		(1u<<10)
#define	PHID2_PPN5		(1u<<9)
#define	PHID2_PPN4		(1u<<8)
#define	PHID2_PPN3		(1u<<7)
#define	PHID2_PPN2		(1u<<6)
#define	PHID2_PPN1		(1u<<5)
#define	PHID2_PPN0		(1u<<4)
#define	PHID2_PREV3		(1u<<3)
#define	PHID2_PREV2		(1u<<2)
#define	PHID2_PREV1		(1u<<1)
#define	PHID2_PREV0		(1u)

// PHCON2 bits ----------
#define	PHCON2_FRCLNK	(1u<<14)
#define	PHCON2_TXDIS	(1u<<13)
#define	PHCON2_JABBER	(1u<<10)
#define	PHCON2_HDLDIS	(1u<<8)

// PHSTAT2 bits --------
#define	PHSTAT2_TXSTAT	(1u<<13)
#define	PHSTAT2_RXSTAT	(1u<<12)
#define	PHSTAT2_COLSTAT	(1u<<11)
#define	PHSTAT2_LSTAT	(1u<<10)
#define	PHSTAT2_DPXSTAT	(1u<<9)
#define	PHSTAT2_PLRITY	(1u<<5)

// PHIE bits -----------
#define	PHIE_PLNKIE		(1u<<4)
#define	PHIE_PGEIE		(1u<<1)

// PHIR bits -----------
#define	PHIR_PLNKIF		(1u<<4)
#define	PHIR_PGIF		(1u<<2)

// PHLCON bits -------
#define	PHLCON_LACFG3	(1u<<11)
#define	PHLCON_LACFG2	(1u<<10)
#define	PHLCON_LACFG1	(1u<<9)
#define	PHLCON_LACFG0	(1u<<8)
#define	PHLCON_LBCFG3	(1u<<7)
#define	PHLCON_LBCFG2	(1u<<6)
#define	PHLCON_LBCFG1	(1u<<5)
#define	PHLCON_LBCFG0	(1u<<4)
#define	PHLCON_LFRQ1	(1u<<3)
#define	PHLCON_LFRQ0	(1u<<2)
#define	PHLCON_STRCH	(1u<<1)

#endif

/*[END OF FILE]*/