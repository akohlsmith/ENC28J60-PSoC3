/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : ARP Packet Handling functions
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file contains the functions required for
			   ARP Packet handling.
*/

#include "IPStackMain.h"

/*******************************************************************************
* Function Name: SendArpRequest
********************************************************************************
* Summary:
*   Sends an ARP Request for the IP Address specified by targetIP.
*
* Parameters:
*   targetIP - An array that holds the IP,whose hardware address is required.
*              example: unsigned char routerIP={192,168,1,10};
*
* Returns:
*   TRUE(0)- if the request was successfully transmitted.
*   FALSE(1) - if the request was not successfully transmitted.
*******************************************************************************/
unsigned int SendArpRequest(unsigned char * targetIP)
{
	ARP a;

	/* Setup EtherNetII Header */
	memcpy(a.eth.SrcAddrs, deviceMAC, 6);
	memset(a.eth.DestAddrs, 0xff, 6);
	a.eth.type = htons(PKT_ARP);

	/* Setup ARP Header */
	a.hardware = htons(HW_ETHER);
	a.protocol = htons(PKT_IP);
	a.hardwareSize = 0x06;
	a.protocolSize = 0x04;
	a.opCode = htons(ARPREQUEST);

	/* Target MAC is set to 0 as it is unknown. */
	memset(a.targetMAC, 0, 6);

	/* Sender MAC is the ENC28J60's MAC address. */
	memcpy(a.senderMAC, deviceMAC, 6);

	/* The target IP is the IP address we want resolved. */
	memcpy(a.targetIP, targetIP, 4);

	/* Sender IP will be the device IP */
	memcpy(a.senderIP, deviceIP, 4);

	return tx_packet((unsigned char *)&a, sizeof(a));
}

/*******************************************************************************
* Function Name: ReplyArpRequest
********************************************************************************
* Summary:
*   Replies to an ARP Request made to the ENC28J60's IP Address.
*   This function creates an ARP Answer from the ARP request packet recd.
*   This function must be called with an appropriate pointer to the same.
*
* Parameters:
*   a - A pointer to the received ARP Request packet.
*
* Returns:
*   TRUE(0)- if the reply was successfully transmitted.
*   FALSE(1) - if the reply was not successfully transmitted.
*******************************************************************************/
unsigned int ReplyArpRequest(ARP *a)
{
	if (!memcmp(a->targetIP, deviceIP, sizeof(deviceIP))) {
		/* Swap the MAC Addresses in the ETH header */
		memcpy(a->eth.DestAddrs, a->eth.SrcAddrs, sizeof(deviceMAC));
		memcpy(a->eth.SrcAddrs, deviceMAC, sizeof(deviceMAC));

		/* Swap the MAC Addresses in the ARP packet */
		memcpy(a->targetMAC, a->senderMAC, sizeof(deviceMAC));
		memcpy(a->senderMAC, deviceMAC, sizeof(deviceMAC));

		/* Swap the IP Addresses in the ARP packet */
		memcpy(a->targetIP, a->senderIP, sizeof(deviceIP));
		memcpy(a->senderIP, deviceIP, sizeof(deviceIP));

		/*Set the opCode for an ARP Reply*/
		a->opCode = htons(ARPREPLY);

		/*Send the Packet!*/
		return tx_packet((unsigned char *)a, sizeof(*a));

	}

	return FALSE;
}

/* [] END OF FILE */
