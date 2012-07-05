
extern "C" {        	
	#include "uip.h"
        #include "sicsLowPan.h"
        #include "uip_ds6.h"
	#include "uip_nd6.h"
	#include "uip_arp.h"
	#include <string.h>	
}
#include "XBee.h"
#include "IPv6Stack.h"
#include "arduino_debug.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

MACLayer* IPv6Stack::mac;
struct simple_udp_connection IPv6Stack::broadcast_connection;

const uint8_t* IPv6Stack::udp_data;
uint16_t IPv6Stack::udp_data_length = 0;
uint16_t IPv6Stack::udp_data_length_left = 0;
const uip_ipaddr_t* IPv6Stack::sender_addr;
uint16_t IPv6Stack::sender_port;


bool IPv6Stack::initMacLayer(MACLayer* macLayer){
  IPv6Stack::mac = macLayer;
  return IPv6Stack::mac->init();
}

uint8_t IPv6Stack::mac_output(uip_lladdr_t *lladdr_destination) {
  IPv6Stack::mac->send(lladdr_destination, uip_6lp_buf.u8, uip_6lp_len);
  return 0;
}

void IPv6Stack::initIpStack() {
    uint8_t addr_pos;
    #if ARDUINO_DEBUG
    arduino_debug_init();
    #endif
        
    // copy MAC-address
    for (addr_pos = 0; addr_pos<UIP_LLADDR_LEN; addr_pos++) 
        uip_lladdr.addr[addr_pos] = IPv6Stack::mac->getMacAddress().addr[addr_pos];
    
    sicslowpan_init(IPv6Stack::mac_output);
    
    tcpip_init();
    clock_init();    
}

void
IPv6Stack::receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
   IPv6Stack::udp_data = data;
   IPv6Stack::udp_data_length = datalen;
   IPv6Stack::udp_data_length_left = datalen;
   IPv6Stack::sender_addr = sender_addr;
   IPv6Stack::sender_port = sender_port;
}

void IPv6Stack::initUdp(uint16_t local_port) {
  simple_udp_register(&broadcast_connection, local_port, NULL, 0, IPv6Stack::receiver);
}

void IPv6Stack::udpSend(const IPv6Address &to, uint16_t remote_port, const void *data, uint16_t datalen){
	broadcast_connection.remote_port = remote_port;
	broadcast_connection.udp_conn->rport = UIP_HTONS(remote_port);
  simple_udp_sendto(&broadcast_connection, data, datalen, &to.address); 
}

bool IPv6Stack::receivePacket() {       
  if (IPv6Stack::mac->receive(ll_src_addr, ll_dst_addr, uip_6lp_buf.u8, uip_6lp_len)){  
    //now decompress
    input(&ll_src_addr, &ll_dst_addr);
    return true;
  }else
    return false;
}
 

void IPv6Stack::processIpStack() {
    return tcpip_input();
}    

void IPv6Stack::pollTimers() {
    IPv6Stack::udp_data_length = 0;
    IPv6Stack::udp_data_length_left = 0;
    etimer_poll();
}

void IPv6Stack::addAddress(IPv6Address &address){
  uip_ds6_addr_add(&address.address, 0, ADDR_AUTOCONF);
}

#if UIP_CONF_ROUTER
void IPv6Stack::setPrefix(IPv6Address &prefix, uint8_t prefix_length)
{  
    uip_ipaddr_t ipaddr;
    memcpy(&ipaddr, &prefix.address, 16);
    uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
    uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
    uip_ds6_prefix_add(&prefix.address, prefix_length, 1, UIP_ND6_RA_FLAG_AUTONOMOUS, UIP_ND6_INFINITE_LIFETIME, 0);
  
}
#endif

bool IPv6Stack::getGlobalPreferredAddress(IPv6Address& address){
  uip_ds6_addr_t * result = uip_ds6_get_global(ADDR_PREFERRED);
  if (result != NULL){
    address.address = result->ipaddr;
    return true;
  }
  return false;
}

int IPv6Stack::udpDataAvailable(){
  return IPv6Stack::udp_data_length_left;
}

char IPv6Stack::readUdpData(){
  return IPv6Stack::udp_data[IPv6Stack::udp_data_length - (udp_data_length_left--)];
}

uint16_t IPv6Stack::getUdpDataLength(){
  return IPv6Stack::udp_data_length;
}

void IPv6Stack::getUdpData(char* buffer){
  memcpy(buffer, IPv6Stack::udp_data, IPv6Stack::udp_data_length);
}

int IPv6Stack::getUdpSenderPort(){
  return IPv6Stack::sender_port;
}

void IPv6Stack::getUdpSenderIpAddress(IPv6Address &address){
  address.address = *IPv6Stack::sender_addr;
}
