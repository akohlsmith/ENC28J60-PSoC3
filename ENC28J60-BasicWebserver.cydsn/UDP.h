/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : UDP Transceiver functions.
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : Functions to send and receive generic UDP packets.
*/
#ifndef UDP_H
#define UDP_H

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
int UDPReply(UDPPacket* udp, uint8_t *payload, uint16_t payloadlen);

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
int UDPSend(ipaddr_t targetIP, uint16_t targetPort, uint8_t *payload, uint16_t payloadlen);


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
void UDP_ProcessIncoming(UDPPacket* incomingpacket);

#endif

/* [] END OF FILE */
