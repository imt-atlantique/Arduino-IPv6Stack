#include "packetbuf.h"

//ADDED ALE
uip_ll_buf_t uip_6lp_buf; //Buffer for 6loWPAN packets

u16_t uip_6lp_len; //Length of 6loWPAN packet in uip_6lp_buf

//link layer addresses used to send and receive packets
uip_lladdr_t ll_src_addr;
uip_lladdr_t ll_dst_addr;

// mac_addr_size: Size (in bytes) of the link layer (MAC) destination mac address (2 or 8 bytes) which will receive the frame that we want to send.
uint8_t mac_addr_size;
