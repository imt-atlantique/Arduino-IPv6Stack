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

#ifndef IPV6ADDRESS_H
#define IPV6ADDRESS_H

extern "C" {        	
	#include "uip.h"
}

class IPv6Address{
  private:
    uip_ipaddr_t address;
    friend class IPv6Stack;
  public:
    IPv6Address(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t addr5, uint8_t addr6, uint8_t addr7, uint8_t addr8, uint8_t addr9, uint8_t addr10, uint8_t addr11, uint8_t addr12, uint8_t addr13, uint8_t addr14, uint8_t addr15);
    IPv6Address(uint16_t addr0, uint16_t addr1, uint16_t addr2, uint16_t addr3, uint16_t addr4, uint16_t addr5, uint16_t addr6, uint16_t addr7);
    IPv6Address();//Creates a link local multicast ipv6 address
    void print();
    uint16_t getAddressValue(uint8_t pos);//pos goes between 0 and 7
    void setAddressValue(uint16_t value, uint8_t pos);//pos goes between 0 and 7
    IPv6Address& operator=(IPv6Address const &addr);
};

#endif
