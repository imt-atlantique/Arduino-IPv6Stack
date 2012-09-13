/*
 Arduino IPv6 MAC Layer interface
 
 This class (interface) specifies the methods that must be implemented by the MAC Layer in order to be used by the IPv6 Stack for communicating.
 As long as these methods can be implemented, any technology could be used in the link layer.
 
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
#ifndef __MACLAYER__
#define __MACLAYER__

extern "C" {
  #include "uip.h"
  #include "sicsLowPan.h"
}

#include "IPv6llAddress.h"

/* Generic MAC return values. */
enum MACTransmissionStatus{
  /**< The MAC layer transmission was OK. */
  MAC_TX_STATUS_OK,

  /**< The MAC layer transmission could not be performed due to a
     collision. */
  MAC_TX_STATUS_COLLISION,

  /**< The MAC layer did not get an acknowledgement for the packet. */
  MAC_TX_STATUS_NO_ACK,

  /**< The MAC layer deferred the transmission for a later time. */
  MAC_TX_STATUS_DEFERRED,

  /**< The MAC layer transmission could not be performed because of an
     error. The upper layer may try again later. */
  MAC_TX_STATUS_ERR,

  /**< The MAC layer transmission could not be performed because of a
     fatal error. The upper layer does not need to try again, as the
     error will be fatal then as well. */
  MAC_TX_STATUS_ERR_FATAL,
};

class MACLayer{
 public:
    virtual bool init() = 0;
    virtual MACTransmissionStatus send(const IPv6llAddress& lladdr_dest, uint8_t* data, uint16_t length, int &number_transmissions) = 0;
    virtual bool receive(IPv6llAddress& lladdr_src, IPv6llAddress& lladdr_dest, uint8_t* data, uint16_t& length) = 0;
    virtual const IPv6llAddress& getMacAddress() = 0;
};

#endif
