#include "IPv6llAddress.h"
#include "arduino_debug.h"
extern "C"{
  #include "rimeaddr.h"
}

IPv6llAddress::IPv6llAddress(uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t addr5, uint8_t addr6, uint8_t addr7, uint8_t addr8){
  address.addr[0] = addr1;
  address.addr[1] = addr2;
  address.addr[2] = addr3;
  address.addr[3] = addr4;
  address.addr[4] = addr5;
  address.addr[5] = addr6;
  address.addr[6] = addr7;
  address.addr[7] = addr8;
}

IPv6llAddress::IPv6llAddress(){
  address.addr[0] = 0;
  address.addr[1] = 0;
  address.addr[2] = 0;
  address.addr[3] = 0;
  address.addr[4] = 0;
  address.addr[5] = 0;
  address.addr[6] = 0;
  address.addr[7] = 0;
}

void IPv6llAddress::print(){
  arduino_debug_lladdr(&address);
}

uint8_t IPv6llAddress::getAddressValue(uint8_t pos) const{
  return address.addr[pos];
}

void IPv6llAddress::setAddressValue(uint8_t value, uint8_t pos){
  address.addr[pos] = value;
}

IPv6llAddress& IPv6llAddress::operator=(IPv6llAddress const &addr){
  rimeaddr_copy((rimeaddr_t*)&address, (const rimeaddr_t*)&addr.address);
  return *this;
}

bool IPv6llAddress::isNull() const{
  return rimeaddr_cmp((rimeaddr_t*)&address, &rimeaddr_null);
}
