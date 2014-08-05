#ifndef _DHCP_H_
#define _DHCP_H_

void dhcp_init(void);
int dhcp_discover(void);
int dhcp_request(void *packet);
int dhcp_process(DHCPhdr *dh);

#endif /* _DHCP_H_ */
