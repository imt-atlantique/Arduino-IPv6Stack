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
