/*
 Arduino IPv6 stack class
 
 This class represents the IPv6 stack (ip, udp, rpl) which needs the implemetation of the link layer to send and receive packets
 
 created 29 June 2012
 by Alejandro Lampropulos (alejandro.lampropulos@telecom-bretagne.eu)
 Telecom Bretagne Rennes, France
 
 Copyright (c) 2012, Telecom Bretagne Rennes, France.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 3. Neither the name of the Institute nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
 
 */


extern "C" {        	
	#include "uip.h"
        #include "sicsLowPan.h"
	#include "uip_icmp6.h"
        #include "uip_ds6.h"
	#include "uip_nd6.h"
	#include "uip_arp.h"
	#include "ctimer.h"
        #include "neighbor_info.h"
        #include "rpl.h"
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

IPv6llAddress IPv6Stack::ll_destination_address;
IPv6llAddress IPv6Stack::ll_source_address;

const uint8_t* IPv6Stack::udp_data;
uint16_t IPv6Stack::udp_data_length = 0;
uint16_t IPv6Stack::udp_data_length_left = 0;
const uip_ipaddr_t* IPv6Stack::sender_addr;
uint16_t IPv6Stack::sender_port;


bool IPv6Stack::initMacLayer(MACLayer* macLayer){
  IPv6Stack::mac = macLayer;
  return IPv6Stack::mac->init();
}

uint8_t IPv6Stack::mac_output(uip_lladdr_t *lladdr) {
  int numberTrans = 0;
  rimeaddr_copy((rimeaddr_t*)&ll_destination_address.address, (const rimeaddr_t*) lladdr);
  uint8_t status = IPv6Stack::mac->send(ll_destination_address, uip_6lp_buf.u8, uip_6lp_len, numberTrans);
  neighbor_info_packet_sent(status, numberTrans, (const rimeaddr_t*)lladdr);
  return status;
}

void IPv6Stack::initIpStack() {
    uint8_t addr_pos;
    #if ARDUINO_DEBUG
    arduino_debug_init();
    #endif
    
    ctimer_init();
    
    // copy MAC-address
    for (addr_pos = 0; addr_pos<UIP_LLADDR_LEN; addr_pos++) 
        uip_lladdr.addr[addr_pos] = IPv6Stack::mac->getMacAddress().getAddressValue(addr_pos);
    
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
  if (IPv6Stack::mac->receive(ll_source_address, ll_destination_address, uip_6lp_buf.u8, uip_6lp_len)){ 
    rimeaddr_copy((rimeaddr_t*)&ll_dst_addr, (const rimeaddr_t*)&ll_destination_address.address);
    rimeaddr_copy((rimeaddr_t*)&ll_src_addr, (const rimeaddr_t*)&ll_source_address.address);
    uip_len = 0;
    //now decompress
    input(&ll_src_addr, &ll_dst_addr);
    if (uip_len != 0){//if we decompressed sth, update the neighbor info and process the decompressed IPv6 packet
      neighbor_info_packet_received((const rimeaddr_t*)&ll_src_addr);
      processIpStack();
      return true;
    }
  }
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
    #if !UIP_CONF_IPV6_RPL
      uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
      uip_ds6_prefix_add(&prefix.address, prefix_length, 1, UIP_ND6_RA_FLAG_AUTONOMOUS, UIP_ND6_INFINITE_LIFETIME, 0);  
    #else /* UIP_CONF_IPV6_RPL */
	  uip_ds6_addr_add(&ipaddr, 0, ADDR_MANUAL);
	  uip_ds6_set_addr_iid(&prefix.address, &uip_lladdr);
      rpl_dag_t *dag;
      rpl_set_root((uip_ip6addr_t *)&prefix.address);
      dag = rpl_get_dag(RPL_ANY_INSTANCE);
      rpl_set_prefix(dag, &prefix.address, prefix_length);
    #endif /* UIP_CONF_IPV6_RPL */   
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

void IPv6Stack::ping(IPv6Address &dest, uint8_t datalength){
	uip_icmp6_echo_request_output(&dest.address, datalength);
}
