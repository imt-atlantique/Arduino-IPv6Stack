
#ifndef __XBEEMACLAYER__
#define __XBEEMACLAYER__

#include "MACLayer.h"
#include "XBee.h"
#include "arduino_debug.h"

class XBeeMACLayer: public MACLayer{
    private:
      XBee xbee;
      uip_lladdr_t my_mac;
      
      bool sendAtCommand(uint8_t& mac_position);
    public:      
      XBeeMACLayer();
      bool init();
      bool send(const uip_lladdr_t *lladdr_dest, uint8_t* data, uint16_t length);
      bool receive(uip_lladdr_t &lladdr_src, uip_lladdr_t &lladdr_dest, uint8_t* data, uint16_t& length);
      const uip_lladdr_t& getMacAddress();
};

#endif
