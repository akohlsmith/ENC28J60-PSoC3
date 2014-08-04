/*
 Network Stack for PSoC3-ENC28J60 hardware
 -----------------------------------------
 Title  : Global Variables declaration file
 Author : Kartik Mankad
 Date : 30-06-12
 This code is licensed as CC-BY-SA 3.0
 Description : This file contains declarations of
               the global variables used.

 You may edit Router IP,DNS Server IP and Webserver,Webclient Port here.
*/
#include <stdint.h>
#include "IPStack.h"
#include "globals.h"

/**Edit values as per your configuration**/

ipaddr_t routerIP = { 192, 168, 1, 1 };
ipaddr_t serverIP;
ipaddr_t dnsIP = { 192, 168, 1, 1 };

uint16_t WWWPort = 80;
uint16_t WClientPort = 15979;
uint16_t UDPPort = 1200;

/***********************************************/
/***Do not assign values to variables below.***/

ipaddr_t deviceIP;
macaddr_t deviceMAC;
macaddr_t routerMAC;

int WebClientStatus = 0;
uint8_t WebClientQuery[250];

/* [] END OF FILE */