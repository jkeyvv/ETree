#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx.h"

/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   2
#define MAC_ADDR1   0
#define MAC_ADDR2   0
#define MAC_ADDR3   38
#define MAC_ADDR4   0
#define MAC_ADDR5   40

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   172
#define IP_ADDR1   16
#define IP_ADDR2   5
#define IP_ADDR3   10

/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   172
#define GW_ADDR1   16
#define GW_ADDR2   5
#define GW_ADDR3   254

#define USE_DHCP       /* enable DHCP, if disabled static address is used */

#endif
