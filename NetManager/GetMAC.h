//
//  GetMAC.h
//  NetManager
//
//  Created by XWJACK on 3/12/16.
//  Copyright © 2016 XWJACK. All rights reserved.
//

#ifndef GetMAC_h
#define GetMAC_h

#include <stdio.h>
#include <stdlib.h>
//GetMAC
#include <sys/sysctl.h>

#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <sys/socket.h>

#include <err.h>
#include <errno.h>

int ctoMACAddress(in_addr_t addr, char *back);

#endif /* GetMAC_h */
