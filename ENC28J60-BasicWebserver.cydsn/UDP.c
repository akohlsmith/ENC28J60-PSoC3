/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : UDP Transceiver functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Functions to send and receive generic UDP packets.

*/

#include "IPStackMain.h"

/* respond to a UDP packet. udp is the rx'd packet, data/len are the payload to send back */
int UDPReply(UDPPacket* udp, uint8_t *data, uint16_t len)
{
	uint16 port;

	/* Swap the MAC/IP Addresses */
	memcpy(udp->udp.ip.eth.DestAddrs, udp->udp.ip.eth.SrcAddrs, sizeof(macaddr_t));
	memcpy(udp->udp.ip.eth.SrcAddrs, deviceMAC, sizeof(macaddr_t));
	memcpy(udp->udp.ip.dest, udp->udp.ip.source, sizeof(ipaddr_t));
	memcpy(udp->udp.ip.source, deviceIP, sizeof(ipaddr_t));

	/* Swap the ports */
	port = udp->udp.sourcePort;
	udp->udp.sourcePort = udp->udp.destPort;
	udp->udp.destPort = port;

	/* zero the checksums */
	udp->udp.chksum = 0;
	udp->udp.ip.chksum = 0;

	/* write in the correct lengths */
	udp->udp.len = htons(sizeof(UDPhdr) - sizeof(IPhdr) + len);
	udp->udp.ip.len = htons(sizeof(UDPhdr) + len - sizeof(EtherNetII));

	/* clear the old payload and copy in the new */
	memset(udp->Payload, 0, ntohs(udp->udp.len) - 8);
	memcpy(udp->Payload, data, len);

	/* calculate the checksums */
	udp->udp.ip.chksum = htons(checksum(((uint8_t *)udp) + sizeof(EtherNetII), sizeof(IPhdr) - sizeof(EtherNetII), CK_IP));
	udp->udp.chksum = htons(checksum(&udp->udp.ip.source, 16 + len, CK_UDP));

	return tx_packet(udp, sizeof(UDPhdr) + len);
}


/* send a UDP packet */
int UDPSend(ipaddr_t dst, uint16_t dport, uint8_t *data, uint16_t len)
{
	UDPPacket udp;

	memset(&udp, 0, sizeof(udp));
	SetupBasicIPPacket(&udp, PROTO_UDP, dst);

	udp.udp.sourcePort = htons(UDPPort);
	udp.udp.destPort = htons(dport);

	/*Write in the correct lengths*/
	udp.udp.len = htons(sizeof(UDPhdr) - sizeof(IPhdr) + len);
	udp.udp.ip.len = htons(sizeof(UDPhdr) + len - sizeof(EtherNetII));

	memcpy(udp.Payload, data, len);

	/* calculate the checksums */
	udp.udp.ip.chksum = htons(checksum(((uint8_t *)&udp) + sizeof(EtherNetII), sizeof(IPhdr) - sizeof(EtherNetII), CK_IP));
	udp.udp.chksum = htons(checksum(&udp.udp.ip.source, 16 + len, CK_UDP));

	return tx_packet(&udp, sizeof(UDPhdr) + len);
}

/* called by GetPacket, we can spin off and process different UDP packets in here */
void UDP_ProcessIncoming(UDPPacket *udp)
{
	uint8_t *data;

	data = (uint8_t *)udp + sizeof(UDPhdr);

	if (ntohs(udp->udp.destPort) == 68) {
		dhcp_process((DHCPhdr *)udp);

	} else {
		if(strncmp(data,"Invoke.",sizeof("Invoke."))==0){
			UDPReply(udp,"Hello World",sizeof("Hello World"));
		}else{
			UDPReply(udp,"Access Denied.",sizeof("Access Denied."));
		}
	}
}

/* [] END OF FILE */
