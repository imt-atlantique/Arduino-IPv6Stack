
#ifndef __MACLAYER__
#define __MACLAYER__

extern "C" {
  #include "uip.h"
  #include "sicsLowPan.h"
}

class MACLayer{
 public:
    virtual bool init() = 0;
    virtual bool send(const uip_lladdr_t *lladdr_dest, uint8_t* data, uint16_t length) = 0;
    virtual bool receive(uip_lladdr_t &lladdr_src, uip_lladdr_t &lladdr_dest, uint8_t* data, uint16_t& length) = 0;
    virtual const uip_lladdr_t& getMacAddress() = 0;
};

#endif
