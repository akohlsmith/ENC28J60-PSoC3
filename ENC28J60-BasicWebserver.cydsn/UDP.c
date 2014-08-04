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

/*******************************************************************************
* Function Name: UDPReply
********************************************************************************
* Summary:
*   Generate and send a reply to a UDP packet RX'd.
*
* Parameters:
*   udppkt - pointer to the UDP packet recd.
*   datapayload - data to be sent in the UDP Reply.
*   payloadlen - length of the data payload to be sent.
*
* Returns:
*   TRUE(0)- if the UDP Reply was successfully sent.
*   FALSE(1) - if the UDP Reply was not successful in transmission.
*******************************************************************************/
int UDPReply(UDPPacket* udp, uint8_t *payload, uint16_t payloadlen)
{
    uint16 port;

    /*Swap the MAC Addresses in the ETH header*/
    memcpy( udp->udp.ip.eth.DestAddrs, udp->udp.ip.eth.SrcAddrs, 6);
    memcpy( udp->udp.ip.eth.SrcAddrs, deviceMAC,6 );

    /*Swap the IP Addresses in the IP header*/
    memcpy( udp->udp.ip.dest, udp->udp.ip.source,4);
    memcpy( udp->udp.ip.source, deviceIP,4);

    /*Swap the ports*/
    port=udp->udp.sourcePort;
    udp->udp.sourcePort=udp->udp.destPort;
    udp->udp.destPort=port;

    /*zero the checksums*/
    udp->udp.chksum=0x00;
    udp->udp.ip.chksum=0x00;

    /*write in the correct lengths*/
    udp->udp.len=(sizeof(UDPhdr)-sizeof(IPhdr))+payloadlen;
    udp->udp.ip.len=(sizeof(UDPhdr)+payloadlen)-sizeof(EtherNetII);

    /*clear the old payload*/
    memset(udp->Payload,0x00, (udp->udp.len)-8 );

    /*copy in the payload*/
    memcpy(udp->Payload,payload,payloadlen);

    /*do the checksums.*/
    udp->udp.ip.chksum=checksum(((unsigned char*) udp) + sizeof(EtherNetII),sizeof(IPhdr) - sizeof(EtherNetII),0);
    udp->udp.chksum=checksum((unsigned char*)udp->udp.ip.source,16+payloadlen,1);

    /*Send the packet.*/
    return MACWrite(udp, sizeof(UDPhdr) + payloadlen);
}

/*******************************************************************************
* Function Name: UDPSend
********************************************************************************
* Summary:
*   Generate and send a UDP packet with data.
*   You may edit the UDP Port number(that will be used as the source port)
*   in "globals.c".Default is 1200.
*
* Parameters:
*   targetIP - IP address to send the UDP packet to.
*   targetPort - Port to direct the UDP packet to.
*   datapayload - data to be sent in the UDP packet.
*   payloadlen - length of the data payload to be sent.
*
* Returns:
*   TRUE(0)- if the UDP packet was successfully sent.
*   FALSE(1) - if the UDP packet was not successful in transmission.
*******************************************************************************/
int UDPSend(ipaddr_t targetIP, uint16_t targetPort, uint8_t *payload, uint16_t payloadlen)
{
    UDPPacket udp;

    /*Setup the IP part*/
    SetupBasicIPPacket(&udp, PROTO_UDP, targetIP);
    udp.udp.ip.flags = 0x0;

    /*Setup the ports*/
    udp.udp.sourcePort=UDPPort;
    udp.udp.destPort=targetPort;

    /*Zero the checksums*/
    udp.udp.chksum=0x00;
    udp.udp.ip.chksum=0x00;

    /*Write in the correct lengths*/
    udp.udp.len=(sizeof(UDPhdr)-sizeof(IPhdr))+payloadlen;
    udp.udp.ip.len=(sizeof(UDPhdr)+payloadlen)-sizeof(EtherNetII);

    /*Copy in the payload*/
    memcpy(udp.Payload,payload,payloadlen);

    /*Do the checksums.*/
    udp.udp.ip.chksum=checksum((unsigned char*)&udp + sizeof(EtherNetII),sizeof(IPhdr) - sizeof(EtherNetII),0);
    udp.udp.chksum=checksum((unsigned char*)&udp.udp.ip.source,16+payloadlen,1);

    /*Send the packet!*/
    return(MACWrite((unsigned char*)&udp, sizeof(UDPhdr)+payloadlen));
}

/*******************************************************************************
* Function Name: UDP_ProcessIncoming
********************************************************************************
* Summary:
*   This will be called in GetPacket,when a UDP packet is recd.You may process the data
*   carried by that incoming UDP packet in this function,and then call UDPReply to respond.
*
* Parameters:
*   incomingpacket - packet whose data payload is to be processed and replied to.
* Returns:
*   Nothing.
*******************************************************************************/
void UDP_ProcessIncoming(UDPPacket* incomingpacket){
	unsigned char* datapointer=(unsigned char*)incomingpacket+sizeof(UDPhdr);

	if(strncmp(datapointer,"Invoke.",sizeof("Invoke."))==0){
		UDPReply(incomingpacket,"Hello World",sizeof("Hello World"));
	}else{
		UDPReply(incomingpacket,"Access Denied.",sizeof("Access Denied."));
	}

}

/* [] END OF FILE */
