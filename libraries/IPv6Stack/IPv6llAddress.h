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

#ifndef IPV6LLADDRESS_H
#define IPV6LLADDRESS_H

extern "C" {        	
	#include "uip.h"
}

class IPv6llAddress{
  private:
    uip_lladdr_t address;
    friend class IPv6Stack;
  public:
    IPv6llAddress(uint8_t addr0, uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t addr5, uint8_t addr6, uint8_t addr7);
    IPv6llAddress();//Creates 0 value link layer address
    void print();
    uint8_t getAddressValue(uint8_t pos) const;//pos goes between 0 and 7
    void setAddressValue(uint8_t value, uint8_t pos);//pos goes between 0 and 7
    IPv6llAddress& operator=(IPv6llAddress const &addr);
    bool isNull() const;
};

#endif
