#include "e1000.h"
#include <stddef.h>
#include <string.h>

static volatile uint32_t *e1000_regs;
static struct e1000_rx_desc rx_descs[E1000_NUM_RX_DESC] __attribute__((aligned(16)));
static struct e1000_tx_desc tx_descs[E1000_NUM_TX_DESC] __attribute__((aligned(16)));
static uint8_t rx_buffers[E1000_NUM_RX_DESC][E1000_RX_BUF_SIZE];
static uint8_t tx_buffers[E1000_NUM_TX_DESC][E1000_RX_BUF_SIZE];

static uint32_t rx_cur = 0;
static uint32_t tx_cur = 0;

static inline void e1000_write(uint32_t reg, uint32_t val) {
    e1000_regs[reg/4] = val;
}

static inline uint32_t e1000_read(uint32_t reg) {
    return e1000_regs[reg/4];
}

void e1000_init(uintptr_t mmio_base) {
    e1000_regs = (volatile uint32_t*)mmio_base;

    // Setup RX descriptors
    for (int i = 0; i < E1000_NUM_RX_DESC; ++i) {
        rx_descs[i].addr = (uintptr_t)&rx_buffers[i];
        rx_descs[i].status = 0;
    }
    e1000_write(E1000_REG_RDBAL, (uint32_t)(uintptr_t)rx_descs);
    e1000_write(E1000_REG_RDBAH, (uint32_t)((uintptr_t)rx_descs >> 32));
    e1000_write(E1000_REG_RDLEN, E1000_NUM_RX_DESC * sizeof(struct e1000_rx_desc));
    e1000_write(E1000_REG_RDH, 0);
    e1000_write(E1000_REG_RDT, E1000_NUM_RX_DESC - 1);

    // Setup TX descriptors
    for (int i = 0; i < E1000_NUM_TX_DESC; ++i) {
        tx_descs[i].addr = (uintptr_t)&tx_buffers[i];
        tx_descs[i].status = 0;
    }
    e1000_write(E1000_REG_TDBAL, (uint32_t)(uintptr_t)tx_descs);
    e1000_write(E1000_REG_TDBAH, (uint32_t)((uintptr_t)tx_descs >> 32));
    e1000_write(E1000_REG_TDLEN, E1000_NUM_TX_DESC * sizeof(struct e1000_tx_desc));
    e1000_write(E1000_REG_TDH, 0);
    e1000_write(E1000_REG_TDT, 0);

    // Enable RX and TX
    e1000_write(E1000_REG_RCTL, 0x00000002 | 0x00000010 | 0x00008000); // EN | BAM | SECRC
    e1000_write(E1000_REG_TCTL, 0x00000002 | 0x00000008 | 0x00000100 | 0x00000400); // EN | PSP | CT=0x10 | COLD=0x40
}

int e1000_send(const void *data, uint16_t len) {
    uint32_t cur = tx_cur;
    if (!(tx_descs[cur].status & 0x1)) return -1; // Not ready

    memcpy((void*)tx_descs[cur].addr, data, len);
    tx_descs[cur].length = len;
    tx_descs[cur].cmd = 0x9; // RS | EOP
    tx_descs[cur].status = 0;

    tx_cur = (cur + 1) % E1000_NUM_TX_DESC;
    e1000_write(E1000_REG_TDT, tx_cur);
    return 0;
}

int e1000_recv(void *buf, uint16_t bufsize) {
    uint32_t next = (rx_cur + 1) % E1000_NUM_RX_DESC;
    if (!(rx_descs[next].status & 0x1)) return 0; // No packet

    uint16_t len = rx_descs[next].length;
    if (len > bufsize) len = bufsize;
    memcpy(buf, (void*)rx_descs[next].addr, len);

    rx_descs[next].status = 0;
    e1000_write(E1000_REG_RDT, next);
    rx_cur = next;
    return len;
}
