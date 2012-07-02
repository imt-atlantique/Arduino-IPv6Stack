
/* THIS FILES ARE USED TO REPLACE THE PACKETBUF */
#ifndef __PACKETBUF_H__
#define __PACKETBUF_H__

#include "contiki_conf.h"
#include "uip.h"

//ADDED ALE
typedef union {
	uint8_t u8[UIP_BUFSIZE];
} uip_ll_buf_t;

//The buffer that will contain the compressed IPv6 packet (the 6loWPAN packet) that must be sent by the link (MAC) layer.
//It is also the buffer the buffer that contains the incoming compressed IPv6 packet (the 6loWPAN packet) that is willing to be decompressed.
extern uip_ll_buf_t uip_6lp_buf;

//The variable that will contain the length of the compressed IPv6 packet (the 6loWPAN packet) that must be sent by the link (MAC) layer.
//It is also the length of the compressed IPv6 packet (the 6loWPAN packet) that is willing to be decompressed.
extern u16_t uip_6lp_len;

//link layer addresses used to send and receive packets
extern uip_lladdr_t ll_src_addr;
extern uip_lladdr_t ll_dst_addr;

// mac_addr_size: Size (in bytes) of the link layer (MAC) destination mac address (2 or 8 bytes) which will receive the frame that we want to send.
extern uint8_t mac_addr_size;

#endif
