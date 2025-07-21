#ifndef NIC_H
#define NIC_H

#include <stdint.h>
#include <lwip/netif.h>

typedef struct NIC {
    int type;
    int mintu;
    int mtu;
    int irq;
    void *infoLocation;
    uint8_t MAC[6];
    struct netif lwip;
    uint32_t dhcpTransactionID;
} NIC;

#endif
