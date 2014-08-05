/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : DNS Handling functions
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file defines the function required for
			   carrying out DNS Lookups.
*/

#include "IPStackMain.h"

/*
 * attempts to resolve the specified name into an IP address.
 * writes the address to *addr if successful
 * returns 0 for success
 */
int gethostbyname_simple(const uint8_t *name, ipaddr_t *addr)
{
	uint8_t packet[MAXPACKETLEN];
	uint16_t len, noChars = 0;
	const uint8_t *c;
	uint8_t *dnsq;
	int timeout = 9000;
	DNShdr *dns;


	memset(packet, 0, sizeof(packet));
	SetupBasicIPPacket(&packet, PROTO_UDP, dnsIP);

	dns = (DNShdr *)packet;
	dns->udp.sourcePort = htons(0xABCD);	/* chosen at random */
	dns->udp.destPort = htons(53);

	dns->id = htons(0xbaab);		/* chosen at random */
	dns->flags = htons(0x0100);
	dns->qdCount = htons(1);

	/* Format the URL into a proper DNS Query */
	dnsq = (char *)(packet + sizeof(DNShdr) + 1);
	for (c = name; *c != '\0' && *c !='\\'; ++c, ++dnsq) {
		*dnsq = *c;
		if (*c == '.' ){
			*(dnsq - (noChars + 1)) = noChars;
			noChars = 0;
		}

		else ++noChars;
	}

	*(dnsq - (noChars + 1)) = noChars;
	*dnsq++ = 0;

	/* Define the host type and class */
	*dnsq++ = 0;
	*dnsq++ = 1;
	*dnsq++ = 0;
	*dnsq++ = 1;

	/*Length of the Packet*/
	len = (unsigned char*)dnsq-packet;

	/*Set the IP and UDP length fields*/
	dns->udp.len = htons(len - sizeof(IPhdr));
	dns->udp.ip.len = htons(len - sizeof(EtherNetII));

	/*Calculate the UDP and IP Checksums*/
	dns->udp.ip.chksum = htons(checksum((uint8_t *)dns + sizeof(EtherNetII), sizeof(IPhdr) - sizeof(EtherNetII), CK_IP));
	dns->udp.chksum = htons(checksum(&dns->udp.ip.source, len + 8 - sizeof(IPhdr), CK_UDP));
	/* NOTE: len + 8 because Source IP and DestIP, which are part of the pseduoheader, are 4 bytes each. */

	/*Send the DNS Query packet*/
	tx_packet(packet, len);

	/* Now that we have sent the query, we wait for the reply and then process it. */

	while (timeout--) {
		UDPhdr *udp = (UDPhdr *)packet;

		if (! GetPacket(PROTO_UDP, packet)) {
			continue;
		}

		if (ntohs(udp->sourcePort) == DNSUDPPORT) {
			dns = (DNShdr*)packet;

			/* does the ID match and there were no DNS errors? */
			if ((ntohs(dns->id) == 0xbaab) && ((ntohs(dns->flags) & 0x008F) != 0x0080)) {
				dnsq = packet + len;

				/* Lets go into a loop to browse through the returned resources. */
				for(;;) {
					if (*dnsq == 0xC0) {	/* PTR? */
						dnsq += 2;
					} else {
						/*
						 * we just search for the first, zero=root domain
						 * all other octets must be non zero
						 */
						while (++dnsq < packet + len) {
							if (*dnsq == 0) {
								++dnsq;
								break;
							}
						};
					}

					/*
					 * There might be multipe records in the answer.
					 * We are searching for an 'A' record (contains IP Address).
					 */
					if (dnsq[1] == 1 && dnsq[9] == 4) { /* Check if type "A" and IPv4 */
						/* found an IPv4 address for the name */
						memcpy(addr, dnsq + 10, sizeof(ipaddr_t));

						if(addr[0] == 0) {
							return FALSE;
						} else {
							return TRUE;
						}

						break;
					}

					/*
					 * Advance pointer to browse the remaining records
					 * since we havent got the right one with an IP
					 */
					dnsq += dnsq[9] + 10;
				} //for loop to browse records

				break;

			/* this is either not a DNS response for our query, or it's a bad response */
			} else {
				return FALSE;
			}
		}
	}; /* while(timeout) */

	return FALSE;
}

/* [] END OF FILE */
