
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
