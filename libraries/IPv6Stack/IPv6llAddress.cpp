/*
 Arduino IPv6 link layer address class
 
 This class represents an IPv6 link layer address (8 octets) used by the IPv6 stack
 
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

#include "IPv6llAddress.h"
#include "arduino_debug.h"
extern "C"{
  #include "rimeaddr.h"
}

IPv6llAddress::IPv6llAddress(uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t addr5, uint8_t addr6, uint8_t addr7, uint8_t addr8){
  address.addr[0] = addr1;
  address.addr[1] = addr2;
  address.addr[2] = addr3;
  address.addr[3] = addr4;
  address.addr[4] = addr5;
  address.addr[5] = addr6;
  address.addr[6] = addr7;
  address.addr[7] = addr8;
}

IPv6llAddress::IPv6llAddress(){
  address.addr[0] = 0;
  address.addr[1] = 0;
  address.addr[2] = 0;
  address.addr[3] = 0;
  address.addr[4] = 0;
  address.addr[5] = 0;
  address.addr[6] = 0;
  address.addr[7] = 0;
}

void IPv6llAddress::print(){
  arduino_debug_lladdr(&address);
}

uint8_t IPv6llAddress::getAddressValue(uint8_t pos) const{
  return address.addr[pos];
}

void IPv6llAddress::setAddressValue(uint8_t value, uint8_t pos){
  address.addr[pos] = value;
}

IPv6llAddress& IPv6llAddress::operator=(IPv6llAddress const &addr){
  rimeaddr_copy((rimeaddr_t*)&address, (const rimeaddr_t*)&addr.address);
  return *this;
}

bool IPv6llAddress::isNull() const{
  return rimeaddr_cmp((rimeaddr_t*)&address, &rimeaddr_null);
}
