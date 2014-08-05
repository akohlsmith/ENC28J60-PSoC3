/*
 * DHCP packet handler
 * Andrew Kohlsmith <andrew@mixdown.ca>
 * License: CC-BY-SA 3.0
 */

#include "IPStackMain.h"

/* message op codes from RFC2131 */
#define BOOTREQUEST	(1)
#define BOOTREPLY	(2)

static uint32_t last_xid;
static ipaddr_t my_ip;
static ipaddr_t next_server;

static enum { DHS_INITBOOT=0, DHS_REBOOTING, DHS_BOUND, DHS_RENEWING, DHS_INIT, DHS_REBINDING, DHS_SELECTING, DHS_REQUESTING } dhcp_state;

/* shared buffer for DHCP requests and responses */
static uint8_t dhcpbuffer[576];


void dhcp_init(void)
{
	memset(my_ip, 0, sizeof(ipaddr_t));
	memset(next_server, 0, sizeof(ipaddr_t));
	last_xid = 0;
	dhcp_state = DHS_INITBOOT;
}


/* sends a DHCP discover packet. All UDP/DHCP fields are initialized. */
int dhcp_discover(void)
{
	DHCPhdr *dh = (DHCPhdr *)&dhcpbuffer;
	ipaddr_t host255 = { 255, 255, 255, 255 };

	memset(dh, 0, sizeof(dh));
	SetupBasicIPPacket(dh, PROTO_UDP, host255);

	/* override the destination MAC address, we don't know what it is */
	memset(dh->udp.ip.eth.DestAddrs, 0xff, sizeof(macaddr_t));

	/* override the source IP address, should be all zeroes */
	memset(dh->udp.ip.source, 0, sizeof(ipaddr_t));

	/* source and dest ports */
	dh->udp.sourcePort = ntohs(68);
	dh->udp.destPort = ntohs(67);

	dh->op = BOOTREQUEST;
	dh->htype = HW_ETHER;
	dh->hlen = sizeof(macaddr_t);
	memcpy(dh->chaddr, deviceMAC, sizeof(macaddr_t));

	last_xid = 0xaa55aa55;
	dh->xid = htonl(last_xid);

	/* TODO: fill in secs */
	/* TODO: fill in options requests for NTP, DNS, etc. */

	/* fill in the length and checksum fields */
	dh->udp.len = htons(sizeof(UDPhdr) - sizeof(IPhdr) + sizeof(*dh));
	dh->udp.ip.len = htons(sizeof(UDPhdr) - sizeof(EtherNetII) + sizeof(*dh));
	dh->udp.ip.chksum = htons(checksum(((uint8_t *)dh) + sizeof(EtherNetII), sizeof(IPhdr) - sizeof(EtherNetII), CK_IP));
	dh->udp.chksum = htons(checksum(&dh->udp.ip.source, 16 + sizeof(*dh), CK_UDP));

	/* move the state machine as necessary */
	if (dhcp_state == DHS_INITBOOT) {
		dhcp_state = DHS_REBOOTING;
	} else if (dhcp_state == DHS_INIT) {
		dhcp_state = DHS_SELECTING;
	} else {
		/* TODO: debug point? */
	}

	return tx_packet(dh, sizeof(dh));
}


/* sends a DHCP request packet. Minimal field updates */
int dhcp_request(void *packet)
{
	DHCPhdr *dh = (DHCPhdr *)packet;
	IPhdr *ip = (IPhdr *)packet;

	/* Swap the MAC Addresses in the ETH header */
	memcpy(ip->eth.DestAddrs, ip->eth.SrcAddrs, sizeof(macaddr_t));
	memcpy(ip->eth.SrcAddrs, deviceMAC, sizeof(macaddr_t));

	/* copy the source IP to the destination, and make the source the new IP address from the server */
	memcpy(ip->dest, ip->source, sizeof(ipaddr_t));
	memcpy(ip->source, dh->yiaddr, sizeof(ipaddr_t));

	dh->udp.sourcePort = ntohs(68);
	dh->udp.destPort = ntohs(67);

	dh->op = BOOTREQUEST;
	last_xid = ntohl(dh->xid);

	/* move the state machine as necessary */
	if (dhcp_state == DHS_INITBOOT) {
		dhcp_state = DHS_REBOOTING;
	} else if (dhcp_state == DHS_INIT) {
		dhcp_state = DHS_SELECTING;
	} else {
		/* TODO: debug point? */
	}

	return tx_packet(dh, sizeof(dh));
}


/* if we get here we've verified that the destination port is the DHCP client port (68) */
int dhcp_process(DHCPhdr *dh)
{
	enum { RX_UNKNOWN=0, RX_OFFER, RX_ACK, RX_NAK } rxtype;

	rxtype = RX_UNKNOWN;

	/* is this a DHCP response to our active request? */
	if (dh->op != BOOTREPLY || ntohl(dh->xid) != last_xid) {
		return 0;
	}

	/* is this a DHCPNAK? */
	if ((dh->yiaddr[0] | dh->yiaddr[1] | dh->yiaddr[2] | dh->yiaddr[3]) == 0) {
		rxtype = RX_NAK;

	/* not a DHCPNAK. It's either a DHCPACK or DHCPOFFER. We don't really care which. */
	} else {
		rxtype = RX_ACK;
	}

	switch (dhcp_state) {

	/*
	 * rx packet in REBOOTING/REQUESTING/REBINDING/RENEWING:
	 * if it's an ACK, grab the info and jump to BOUND state
	 * if it's a NAK, jump to INIT state (and the main loop will re-request)
	 */
	case DHS_REBOOTING:
	case DHS_REQUESTING:
	case DHS_REBINDING:
	case DHS_RENEWING:
		/* if we got an ACK it means we have an IP. */
		if (rxtype == RX_ACK) {
			memcpy(my_ip, dh->yiaddr, sizeof(ipaddr_t));
			dhcp_state = DHS_BOUND;

		/* we got a NAK; request an IP. */
		} else if (rxtype == RX_NAK) {
			dhcp_state = DHS_INIT;
		}
		break;

	/* BOUND state: ignore any DHCP packets */
	case DHS_BOUND:
		break;

	case DHS_SELECTING:
		if (rxtype == RX_ACK) {
			dhcp_request(dh);
		}
		break;

	default:
		break;
	};

	return 0;
}
