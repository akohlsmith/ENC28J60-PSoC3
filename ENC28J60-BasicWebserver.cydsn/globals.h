/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Global Variables Header file
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This header file defines(extern) the global variables used.

 Do not edit variables here.Edit them in "globals.c" if you want.
*/

extern ipaddr_t deviceIP;
extern macaddr_t deviceMAC;

extern ipaddr_t routerIP;
extern macaddr_t routerMAC;

extern ipaddr_t serverIP;
extern ipaddr_t dnsIP;

extern uint16_t WWWPort;
extern uint16_t WClientPort;
extern uint16_t UDPPort;

extern int WebClientStatus;

extern uint8_t WebClientQuery[250];

#define TRUE  0
#define FALSE 1

/* [] END OF FILE */
