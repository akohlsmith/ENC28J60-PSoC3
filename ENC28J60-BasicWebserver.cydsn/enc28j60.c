/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : ENC28J60 driver function
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This file contains the underlying ENC28J60 driver.
*/

/*
 Basic ENC28J60 Driver Code for PSoC3
 Author : Kartik Mankad
 Date : 17-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This file contains the functions,structures and registers
	 essential while dealing with the ENC28J60 chip.
*/
#include <stdint.h>
#include "IPStack.h"
#include "globals.h"
#include "enc28j60.h"
#include "spi.h"

/*
 * Macro for Silicon Errata to do with Transmit Logic Reset.
 * Silicon Errata No.12 as per Latest Errata doc for ENC28J60
 * See http://ww1.microchip.com/downloads/en/DeviceDoc/80349c.pdf
 */
#define ERRATAFIX do { 					\
	enc_setbits(ECON1, ECON1_TXRST); 		\
	enc_clearbits(ECON1, ECON1_TXRST); 		\
	enc_clearbits(EIR, EIR_TXERIF | EIR_TXIF);	\
} while(0)


/* status vectors from the device */
TXSTATUS txstat;
RXSTATUS rxstat;


#define enc_writectrl(a, v) enc_write(WCR_OP, a, v)
#define enc_setbits(a, v) enc_write(BFS_OP, a, v)
#define enc_clearbits(a, v) enc_write(BFC_OP, a, v)


/* writes to a control register. Assumes the correct bank has been selected */
static void enc_write(uint8_t op, uint8_t addr, uint8_t val)
{
	if (addr > 0x1f) {
		return;
	}

	addr |= op;		/* append the desired opcode */
	SPI_SEL(TRUE);
	spiTxBuffer(&addr, 1);
	spiTxBuffer(&val, 1);
	SPI_SEL(FALSE);
}

/* returns an 8-bit value from the specified register. Assumes the correct bank has been selected */
static uint8_t enc_read(uint8_t addr)
{
	uint8_t val;

	SPI_SEL(TRUE);
	spiTxBuffer(&addr, 1);
	spiRxBuffer(&val, 1);
	SPI_SEL(FALSE);

	return val;
}


/* returns an 8-bit value from the specified MAC register. Assumes the correct bank has been selected */
static uint8_t enc_readmac(uint8_t addr)
{
	uint8_t val;

	SPI_SEL(TRUE);
	spiTxBuffer(&addr, 1);
	spiRxBuffer(&val, 1);	/* read the dummy byte */
	spiRxBuffer(&val, 1);	/* and now the actual value */
	SPI_SEL(FALSE);

	return val;
}


/* selects a register bank on the ENC28J60 */
static void enc_banksel(uint8_t bank)
{
	uint8_t val;
	if (bank > 3) {
		return;
	}

	val = enc_read(ECON1);
	val &= ~ECON1_BSEL;
	val |= bank;
	enc_writectrl(ECON1, val);
}


/* reads a PHY register as detailed in 3.3.1 on page 21 of the datasheet */
static uint16_t enc_readphy(uint8_t addr)
{
	uint16_t val;
	uint8_t stat;

	enc_banksel(2);
	enc_writectrl(MIREGADR, addr);
	enc_setbits(MICMD, MICMD_MIIRD);	/* request a read */

	do {
		stat = enc_readmac(MISTAT);
	} while(stat & MISTAT_BUSY);

	enc_clearbits(MICMD, MICMD_MIIRD);	/* no longer reading */

	val = enc_readmac(MIRDL);
	val |= (enc_readmac(MIRDH) << 8);

	return val;
}


/* writes to a PHY register as detailed in 3.3.2 on page 21 of the datasheet */
static void enc_writephy(uint8_t addr, uint16_t val)
{
	if (addr > 0x14) {
		return;
	}

	enc_banksel(2);
	enc_writectrl(MIREGADR, addr);
	enc_writectrl(MIWRL, val);
	enc_writectrl(MIWRH, val >> 8);
}


/* reads from the 8kB data buffer on the ENC28J60. */
static uint16_t enc_readbuf(uint8_t *dest, uint16_t len)
{
	uint8_t op;
	uint16_t rx_count;

	op = RBM_OP;
	SPI_SEL(TRUE);
	spiTxBuffer(&op, 1);
	rx_count = spiRxBuffer(dest, len);
	SPI_SEL(FALSE);

	return len;
}


/* writes to the 8kB data buffer on the ENC28J60. */
static uint16_t enc_writebuf(uint8_t *src, uint16_t len)
{
	uint8_t op;
	uint16_t tx_count;

	op = WBM_OP;
	SPI_SEL(TRUE);
	spiTxBuffer(&op, 1);
	tx_count = spiTxBuffer(src, len);
	SPI_SEL(FALSE);

	return tx_count;
}


/* issues a soft-reset of the ENC28J60 over SPI */
static void enc_reset(void)
{
	uint8_t op = RESET_OP;

	SPI_SEL(TRUE);
	spiTxBuffer(&op, 1);
	SPI_SEL(FALSE);

	CyDelay(1000);
}


/* returns nonzero if the link is up. */
uint8_t enc_link_up(void)
{
	return ((enc_readphy(PHSTAT2) & 0x0400) == 0x0400);
}


/* returns the EREVID register contents */
uint8_t enc_revision(void)
{
	enc_banksel(3);
	return enc_read(EREVID);
}


void enc_init(const macaddr_t deviceMAC)
{
	spiInit();
	enc_reset();

	/* Setup the 8kb Memory space on the ENC28J60 by defining ERXST and ERXND */
	enc_banksel(0);
	enc_writectrl(ERXSTL, (unsigned char)(RXSTART & 0x00ff));
	enc_writectrl(ERXSTH, (unsigned char)((RXSTART & 0xff00) >> 8));
	enc_writectrl(ERXNDL, (unsigned char)(RXEND & 0x00ff));
	enc_writectrl(ERXNDH, (unsigned char)((RXEND & 0xff00) >>8));

	/* Set RX Read pointer to start of RX Buffer */
	enc_writectrl(ERXRDPTL, (unsigned char)(RXSTART & 0x00ff));
	enc_writectrl(ERXRDPTH, (unsigned char)((RXSTART & 0xff00) >> 8));

	/* Setup Transmit Buffer */
	enc_writectrl(ETXSTL, (unsigned char)(TXSTART & 0x00ff));
	enc_writectrl(ETXSTH, (unsigned char)((TXSTART & 0xff00) >> 8));
	/* End of buffer will depend on packets,so no point hardcoding it */

	/* Set the RX Filters */
	enc_banksel(1);
	enc_writectrl(ERXFCON, (ERXFCON_UCEN + ERXFCON_CRCEN + ERXFCON_PMEN));

	/*
	 * Set up the packet filter
	 *
	 * This part is taken from Guido Socher's AVR enc28j60 driver.Great Work, that.
	 * For broadcast packets we allow only ARP packtets
	 * All other packets should be unicast only for our mac (MAADR)
	 *
	 * The pattern to match on is therefore
	 * Type     ETH.DST
	 * ARP      BROADCAST
	 * 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
	 * in binary these poitions are:11 0000 0011 1111
	 * This is hex 303F -> EPMM0 = 0x3f, EPMM1 = 0x30
	 */
	enc_writectrl(EPMM0, 0x3f);
	enc_writectrl(EPMM1, 0x30);
	enc_writectrl(EPMCSL, 0x39);
	enc_writectrl(EPMCSH, 0xf7);

	/*
	 * Initialize the MAC Registers.
	 * enable frame reception
	 * type/length fields will be checked, MAC will append a valid CRC and all small packets will be padded
	 */
	enc_banksel(2);
	enc_setbits(MACON1, MACON1_MARXEN);
	enc_writectrl(MACON3, MACON3_FRMLNEN + MACON3_TXCRCEN + MACON3_PADCFG0);    // All small packets will be padded

	enc_writectrl(MAMXFLL, (unsigned char)(MAXFRAMELEN & 0x00ff));
	enc_writectrl(MAMXFLH, (unsigned char)((MAXFRAMELEN & 0xff00) >> 8));

	/* set up back-to-back interpacket gap and non-back-to-back interpacket gap as per the data sheet */
	enc_writectrl(MABBIPG, 0x12);
	enc_writectrl(MAIPGL, 0x12);
	enc_writectrl(MAIPGH, 0x0C);

	/* Assign the MAC Address to the chip. */
	enc_banksel(3);
	enc_writectrl(MAADR1, deviceMAC[0]);
	enc_writectrl(MAADR2, deviceMAC[1]);
	enc_writectrl(MAADR3, deviceMAC[2]);
	enc_writectrl(MAADR4, deviceMAC[3]);
	enc_writectrl(MAADR5, deviceMAC[4]);
	enc_writectrl(MAADR6, deviceMAC[5]);

	/* Initialise the PHY registers as per table 11-3 PHCON1 on page 65 of the datasheet */
	enc_writephy(PHCON1, 0x000);

	/*
	 * "If using half duplex, the host controller may wish to set the PHCON2.HDLDIS bit
	 * to prevent automatic loopback of the data which is transmitted."
	 *     -- Section 6.6 on Page 40
	 */
	enc_writephy(PHCON2, PHCON2_HDLDIS);

	/* Enable reception of packets */
	enc_writectrl(ECON1, ECON1_RXEN);
}


int tx_packet(void *packet, uint16_t len)
{
	uint8_t *buf = packet;
	unsigned char bytControl = 0x00;

	if (! enc_link_up()) {
		return FALSE;
	}

	/* Configure TX Buffer Pointers */
	enc_banksel(0);

	/*Buffer write ptr to start of Tx packet*/
	enc_writectrl(ETXSTL, (unsigned char)(TXSTART & 0x00ff));
	enc_writectrl(ETXSTH, (unsigned char)((TXSTART & 0xff00) >> 8));

	/*Set write buffer pointer to point to start of Tx Buffer*/
	enc_writectrl(EWRPTL, (unsigned char)(TXSTART & 0x00ff));
	enc_writectrl(EWRPTH, (unsigned char)((TXSTART & 0xff00) >> 8));

	/*
	 * Write the Per Packet Control Byte
	 * see table 7-1: FORMAT FOR PER PACKET CONTROL BYTES on page 41
	 */
	enc_writebuf(&bytControl, 1);

	/* Write the packet into the ENC's buffer */
	enc_writebuf(buf, len);

	/* Tell MAC when the end of the packet is */
	enc_writectrl(ETXNDL, (unsigned char)((len+TXSTART+1) & 0x00ff));
	enc_writectrl(ETXNDH, (unsigned char)(((len+TXSTART+1) & 0xff00) >> 8));

	/* We would like to enable Interrupts on Packet TX complete. */
	enc_clearbits(EIR,EIR_TXIF);
	enc_setbits(EIE, EIE_TXIE | EIE_INTIE);

	ERRATAFIX;	/* see definition at top of this file */

	/* Send that Packet! */
	enc_setbits(ECON1, ECON1_TXRTS);

	/* Wait for the Chip to finish the TX,and read the TX interrrupt bit to check the same. */
	while ((enc_read(EIR) & EIR_TXIF) == 0) {
		/* TODO: do something useful? */
	};

	/* Clear TXRTS, since the packet has been TX'd. */
	enc_clearbits(ECON1, ECON1_TXRTS);

	/*
	 * We will now attempt to read the TX Status Vectors.
	 * See TABLE 7-1: TRANSMIT STATUS VECTORS on Page 43 of the datasheet.
	 */
	len++;		/* because of the control byte */
	enc_banksel(0);

	/*Configure the buffer read ptr to read status structure*/
	enc_writectrl(ERDPTL, (unsigned char)(len & 0x00ff));
	enc_writectrl(ERDPTH, (unsigned char)((len & 0xff00) >> 8));

	/* Read In the TX Status Vectors */
	/* Note: Use these for debugging. Really useful. */
	enc_readbuf(&txstat.v[0], 7);

	/*Read TX status vectors to see if TX was interrupted.*/
	if (enc_read(ESTAT) & ESTAT_TXABRT) {
		if (txstat.bits.LateCollision) {
			enc_clearbits(ECON1, ECON1_TXRTS);			/* Toggle TXRTS */
			enc_setbits(ECON1, ECON1_TXRTS);
			enc_clearbits(ESTAT, ESTAT_TXABRT | ESTAT_LATECOL);	/* Clear the Late Collision Bit. */
		}

		/* clear the interrupt and abort flags */
		enc_clearbits(EIR, EIR_TXERIF | EIR_TXIF);
		enc_clearbits(ESTAT,ESTAT_TXABRT);
		return FALSE;
  	}

	return TRUE;
}


int rx_packet(void *dest, uint16_t maxLen)
{
	uint8_t *packet = dest;
	uint16_t pckLen;
	static uint16_t nextpckptr = RXSTART;

	if (! enc_link_up()) {
		return FALSE;
	}

	/* Read EPKTCNT to see if there are any packets for us */
	enc_banksel(1);
	if (enc_read(EPKTCNT) == 0) {
		return 0;
	}

	/* Setup memory pointers to Read in this RX'd packet. */
	enc_banksel(0);
	enc_writectrl(ERDPTL, (unsigned char)(nextpckptr & 0x00ff));
	enc_writectrl(ERDPTH, (unsigned char)((nextpckptr & 0xff00) >> 8));

	/* Read in the Next Packet Pointer and the 32-bit status vector. See Figure 7-3 on page 45. */
	enc_readbuf((unsigned char *)&rxstat.v[0], 6);

	/*Because, Little Endian.*/
	nextpckptr = CYSWAP_ENDIAN16(rxstat.bits.NextPacket);

	/*Compute actual length of the RX'd Packet.*/
	pckLen = CYSWAP_ENDIAN16(rxstat.bits.ByteCount) - 4;	/* -4 to remove the CRC */

	/* limit the amount of data we pull in */
	if (pckLen > (maxLen - 1)) {
		pckLen = maxLen;
	}

	/*
	 * Read the packet only if it was RX'd Okay.
	 * We should be checking other flags too,like Length Out of Range,
	 * but that one doesnt seem reliable.
	 * We need more work and testing here.
	 */
	if (rxstat.bits.RxOk) {
		enc_readbuf(packet, pckLen);
	}

	/* Ensure that ERXRDPT is Always ODD! Else Buffer gets corrupted. See Errata point 5 */
	enc_banksel(0);
	if (((nextpckptr - 1) < RXSTART) || ((nextpckptr-1) > RXEND)) {
		/* Free up memory in the device by adjusting the RX read pointer since we're done with the packet. */
		enc_writectrl(ERXRDPTL, (RXEND & 0x00ff));
		enc_writectrl(ERXRDPTH, ((RXEND & 0xff00) >> 8));
	} else {
		enc_writectrl(ERXRDPTL, ((nextpckptr - 1) & 0x00ff));
		enc_writectrl(ERXRDPTH, (((nextpckptr - 1) & 0xff00) >> 8));
	}

	/* To signal that we are done with the packet, decrement EPKTCNT */
	enc_setbits(ECON2, ECON2_PKTDEC);
	return pckLen;
}
