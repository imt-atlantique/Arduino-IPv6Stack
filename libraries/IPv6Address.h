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
