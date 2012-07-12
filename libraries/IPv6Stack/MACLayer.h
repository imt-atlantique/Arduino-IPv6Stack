
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
