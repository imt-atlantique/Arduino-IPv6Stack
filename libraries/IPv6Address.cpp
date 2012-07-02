#include "IPv6Address.h"
#include "arduino_debug.h"

IPv6Address::IPv6Address(uint8_t addr1, uint8_t addr2, uint8_t addr3, uint8_t addr4, uint8_t addr5, uint8_t addr6, uint8_t addr7, uint8_t addr8, uint8_t addr9, uint8_t addr10, uint8_t addr11, uint8_t addr12, uint8_t addr13, uint8_t addr14, uint8_t addr15, uint8_t addr16){
  uip_ip6addr_u8(&address, addr1,addr2,addr3,addr4,addr5,addr6,addr7,addr8,addr9,addr10,addr11,addr12,addr13,addr14,addr15,addr16);
}

IPv6Address::IPv6Address(uint16_t addr1, uint16_t addr2, uint16_t addr3, uint16_t addr4, uint16_t addr5, uint16_t addr6, uint16_t addr7, uint16_t addr8){
  uip_ip6addr(&address, addr1,addr2,addr3,addr4,addr5,addr6,addr7,addr8);
}

IPv6Address::IPv6Address(){
  uip_create_linklocal_allnodes_mcast(&address);
}

void IPv6Address::print(){
  arduino_debug_address(&address);
}

uint16_t IPv6Address::getAddressValue(uint8_t pos){
  return address.u16[pos];
}

void IPv6Address::setAddressValue(uint16_t value, uint8_t pos){
  address.u16[pos] = value;
}

IPv6Address& IPv6Address::operator=(IPv6Address const &addr){
  uip_ipaddr_copy(&address, &addr.address);
  return *this;
}
