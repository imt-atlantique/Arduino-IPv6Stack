/*
 Arduino IPv6 address class
 
 This class represents an IPv6 address (16 octets) used by the IPv6 stack
 
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

#include "IPv6Address.h"
#include "arduino_debug.h"

IPv6Address::IPv6Address(uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t addr5, uint8_t addr6, uint8_t addr7, uint8_t addr8, uint8_t addr9, uint8_t addr10, uint8_t addr11, uint8_t addr12, uint8_t addr13, uint8_t addr14, uint8_t addr15, uint8_t addr16){
  uip_ip6addr_u8(&address, addr1,addr2,addr3,addr4,addr5,addr6,addr7,addr8,addr9,addr10,addr11,addr12,addr13,addr14,addr15,addr16);
}

IPv6Address::IPv6Address(uint16_t addr1, uint16_t addr2, uint16_t addr3, uint16_t addr4, uint16_t addr5, uint16_t addr6, uint16_t addr7, uint16_t addr8){
  uip_ip6addr(&address, addr1,addr2,addr3,addr4,addr5,addr6,addr7,addr8);
}

IPv6Address::IPv6Address(){
  uip_create_linklocal_allnodes_mcast(&address);
}

void IPv6Address::print(){
  arduino_debug_address(&address);
}

uint16_t IPv6Address::getAddressValue(uint8_t pos){
  return address.u16[pos];
}

void IPv6Address::setAddressValue(uint16_t value, uint8_t pos){
  address.u16[pos] = value;
}

IPv6Address& IPv6Address::operator=(IPv6Address const &addr){
  uip_ipaddr_copy(&address, &addr.address);
  return *this;
}

void IPv6Address::setAddress(uint16_t addr1, uint16_t addr2, uint16_t addr3, uint16_t addr4, uint16_t addr5, uint16_t addr6, uint16_t addr7, uint16_t addr8){
	uip_ip6addr(&address, addr1,addr2,addr3,addr4,addr5,addr6,addr7,addr8);
}

void IPv6Address::setPrefix(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t addr5, uint8_t addr6, uint8_t addr7){
	address.u8[0] = addr0;
	address.u8[1] = addr1;
	address.u8[2] = addr2;
	address.u8[3] = addr3;
	address.u8[4] = addr4;
	address.u8[5] = addr5;
	address.u8[6] = addr6;
	address.u8[7] = addr7;
}

void IPv6Address::setLinkLocalPrefix(){
	uip_create_linklocal_prefix(&address);
}
#if (UIP_LLADDR_LEN == UIP_802154_LONGADDR_LEN)
void IPv6Address::setIID(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t addr5, uint8_t addr6, uint8_t addr7){
	uip_lladdr_t lladdr;
	lladdr.addr[0] = addr0;
	lladdr.addr[1] = addr1;
	lladdr.addr[2] = addr2;
	lladdr.addr[3] = addr3;
	lladdr.addr[4] = addr4;
	lladdr.addr[5] = addr5;
	lladdr.addr[6] = addr6;
	lladdr.addr[7] = addr7;
	uip_ds6_set_addr_iid(&address, &lladdr);
}
#else
void IPv6Address::setIID(uint8_t addr0, uint8_t addr1){
	uip_lladdr_t lladdr;
	lladdr.addr[0] = addr0;
	lladdr.addr[1] = addr1;
	uip_ds6_set_addr_iid(&address, &lladdr);
}
#endif