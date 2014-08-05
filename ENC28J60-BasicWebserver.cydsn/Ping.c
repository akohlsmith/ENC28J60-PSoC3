/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Ping Handling functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Functions to send and receive a ping
	reply and request respectively,are contained here.
*/

#include "IPStackMain.h"

/*******************************************************************************
* Function Name: PingReply
********************************************************************************
* Summary:
*   Generate and send a Ping reply from a received request.
*
* Parameters:
*   ping - A pointer of type ICMPhdr,to the received request's packet.
*   len - length of the recd. Ping request.
* Returns:
*   TRUE(0)- if the Ping Reply was successfully sent.
*   FALSE(1) - if the Ping Reply was not successful in transmission.
*******************************************************************************/
void PingReply(ICMPhdr* ping, uint16_t len)
{
	if (ping->type == ICMPREQUEST) {
		ping->type = ICMPREPLY;
		ping->chksum = 0;
		ping->ip.chksum = 0;

		/* Swap the MAC and IP addresses in the headers */
		memcpy(ping->ip.eth.DestAddrs, ping->ip.eth.SrcAddrs, sizeof(macaddr_t));
		memcpy(ping->ip.eth.SrcAddrs, deviceMAC, sizeof(macaddr_t));
		memcpy(ping->ip.dest, ping->ip.source, sizeof(ipaddr_t));
		memcpy(ping->ip.source, deviceIP, sizeof(ipaddr_t));

		/* Compute the checksums */
		ping->chksum = htons(checksum(((uint8_t *)ping) + sizeof(IPhdr), len - sizeof(IPhdr), CK_ICMP));
		ping->ip.chksum = htons(checksum(((uint8_t *)ping) + sizeof(EtherNetII), sizeof(IPhdr) - sizeof(EtherNetII), CK_IP));

		return tx_packet(ping, len);
	}
}

/*******************************************************************************
* Function Name: SendPing
********************************************************************************
* Summary:
*   Generate and send a Ping request to an IP specified by targetIP
*
* Parameters:
*   targetIP - IP Address to which the ping request will be directed.
*   example: unsigned char LabPC_IP[]={192,168,1,15}; then
*               ..
*              SendPing(LabPC_IP);
* Returns:
*   TRUE(0)- if the Ping request was successfully sent.
*   FALSE(1) - if the Ping request was not successful in transmission.
*******************************************************************************/
int SendPing(ipaddr_t targetIP)
{
	unsigned int i;

	/*declare an ICMP header for our ping request packet*/
	ICMPhdr ping;

	memset(&ping, 0, sizeof(ping));

	/*Setup the IP header part of it*/
	SetupBasicIPPacket(&ping, PROTO_ICMP, targetIP);

	ping.ip.flags = 0x0;
	ping.type = 0x8;
	ping.iden = htons(1);
	ping.seqNum = htons(76);

	/*Fill in the dummy data*/
	for (i=0; i<18; i++) {
		*((unsigned char *)&ping + sizeof(ICMPhdr) + i) = 'A' + i;
	}

	/* Write the length and checksum fields */
	ping.ip.len = htons(60 - sizeof(EtherNetII));
	ping.chksum = htons(checksum(((uint8_t *)&ping) + sizeof(IPhdr), (sizeof(ICMPhdr) - sizeof(IPhdr)) + 18, CK_ICMP));
	ping.ip.chksum = htons(checksum(((uint8_t *)&ping) + sizeof(EtherNetII), sizeof(IPhdr) - sizeof(EtherNetII), CK_IP));

	return tx_packet(&ping, sizeof(ICMPhdr) + 18);
}


/* [] END OF FILE */
