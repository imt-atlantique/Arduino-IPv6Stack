/*
 Arduino IPv6 XBee MAC Layer implementation
 
 This class implements the methods defined by the MACLayer interface. It is developed to use the XBee Series 1 module.
 
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
#ifndef __XBEEMACLAYER__
#define __XBEEMACLAYER__

#include "MACLayer.h"
#include "XBee.h"
#include "arduino_debug.h"

class XBeeMACLayer: public MACLayer{
    private:
      XBee xbee;
      IPv6llAddress my_mac;
      
      bool sendAtCommand();
      bool getResponseMAC();
      bool getResponseCCAFailure();
      int getNumberOfTransmissions();
      
    public:      
      XBeeMACLayer();
      bool init();
      MACTransmissionStatus send(const IPv6llAddress& lladdr_dest, uint8_t* data, uint16_t length, int &number_transmissions);
      bool receive(IPv6llAddress& lladdr_src, IPv6llAddress& lladdr_dest, uint8_t* data, uint16_t& length);
      const IPv6llAddress& getMacAddress();
};

#endif
