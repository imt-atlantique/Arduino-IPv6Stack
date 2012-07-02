
#ifndef IPV6STACK_H
#define IPV6STACK_H

extern "C" {
    #include <inttypes.h>
    #include "uip.h"
    #include "simple_udp.h"
}    

#include "MACLayer.h"
#include "IPv6Address.h"
#include "IPv6Timer.h"

class IPv6Stack
{
  private:
  
    static uint8_t mac_output(uip_lladdr_t *lladdr);
    static MACLayer* mac; //this pointer is for the mac_output function to be able to access the mac layer
    static struct simple_udp_connection broadcast_connection;
    static void receiver(struct simple_udp_connection *c, const uip_ipaddr_t *sender_addr,
         uint16_t sender_port, const uip_ipaddr_t *receiver_addr, uint16_t receiver_port, const uint8_t *data, uint16_t datalen);
         
    static const uint8_t* udp_data;
    static uint16_t udp_data_length;
    static uint16_t udp_data_length_left;
    static const uip_ipaddr_t* sender_addr;
    static uint16_t sender_port;
              
  public:
    
    static bool init_macLayer(MACLayer* macLayer);
    static void init_ipStack();
    static void init_udp(uint16_t local_port, uint16_t remote_port);    
    static void udp_send(const IPv6Address &to, const void *data, uint16_t datalen);
    static bool receive_packet();     
    static void process_ipStack();
    static void poll_timers(); //Note: polling timers put received udp data to 0
    static void add_address(IPv6Address &address);
    static void set_prefix(IPv6Address &prefix, uint8_t prefix_length);//For ROUTERS only
    static bool getGlobalPreferredAddress(IPv6Address& address);
    
    static int udpDataAvailable();
    static char readUdpData();
    static uint16_t getUdpDataLength();
    static void getUdpData(char* buffer);
    static int getUdpSenderPort();
    static void getUdpSenderIpAddress(IPv6Address &address);
};	

#endif

