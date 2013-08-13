/*
  Arduino IPv6 stack example
 
 This sketch connects two Arduino Mega with wireless shield and Xbee
 mounted. This sketch demonstrates use of the IPv6 stack API for sending ping and coap ping messages and their answers.
 
 Circuit:
 * Arduino Mega 2560
 * Wireless shield w/ Xbee Series 1
 
 created 1st July 2013
 by Alejandro Lampropulos (alejandro.lampropulos@telecom-bretagne.eu)
 Telecom Bretagne Rennes, France
 
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
 
 /*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
   This example test the interoperability of different Arduino Mega + Xbee S1 to send messages and respond ping (echo Request) and coap ping messages.
   The example can be used with two leave nodes or router nodes or one router and one leaf node.
 
   It is necessary to have a USB2Serial Light model for Arduino in order to connect the Serial1 port of the Arduino Mega (TX1 = PIN18 and RX1 = PIN19).
   This will allow us to send commands to the Arduino in order to interprate them within our test application as well as receiving the debugging output.
   In order to achieve this, it will be necessary to use either the Arduino IDE's Serial Monitor or another application such as Coolterm, etc.
   Example:
   
   Once the Serial Monitor is open, we can use the following commands:
   
   ping6 FE80:0:0:0:2:12:6d:45:50:e6:65:8d
   
   ping6 FE80:0:0:0:0:FF:FE00:1 (if using 16 bit addresses for pingging MAC address 0x0001)
   
   coaping aaaa:0:0:0:2:12:6d:45:50:e6:65:8d 5683
   
   Note: notice that the IP addresses will vary depending on the MAC address of the XBee module.
   There is a part of our code where we add the nodes as neighbors. If using RPL this would not be necessary as DIO messages allow nodes to add the sender as a neighbor.
   If not using RPL, Neighbor discovery process will be attempted before a message can be sent to an unknown node.
 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <IPv6Stack.h>
#include <XBeeMACLayer.h>
#include <string.h>
#include <stdlib.h>

#if (UIP_LLADDR_LEN == UIP_802154_LONGADDR_LEN)
#define EUT1    0,0x12,0x6d,0x45,0x50,0xe6,0x65,0x8d //MAC address of one of the nodes in the network
#define EUT2    0,0x13,0xa2,0,0x40,0x71,0x76,0xb1 //MAC address of one of the nodes in the network
#else
#define EUT1	0,0x1 //16 bit MAC address of one of the nodes in the network (This can be selected by the developer)
#define EUT2	0,0x2 //16 bit MAC address of one of the nodes in the network (This can be selected by the developer)

//THIS IS THE 16 BIT ADDRESS FOR THE NODE THAT IS BEING FLASHED WITH THIS CODE. IF FLASHING ANOTHER NODE WITH THE SAME CODE, CHANGE THIS VALUE NOT TO HAVE REPEATED MAC ADDRESSES
#define EUT     EUT1

#endif

#define TEST_CHANNEL 15 //CHANNEL FOR COMMUNICATION

#define TEST_PANID  0 //PAN ID

#define TEST_MACMODE MACMODE_NO_ACK // MAC MODE FOR XBEE MODULE (COULD ALSO BE: MACMODE_ACK)

enum CommandType{
  IP_PING,
  COAP_PING
};

//This function calculates the RAM memory left
int mem(){
  uint8_t * heapptr, * stackptr;
  stackptr = (uint8_t *)malloc(4);
  heapptr = stackptr;
  free(stackptr);               
  stackptr = (uint8_t *)(SP);
  return stackptr - heapptr;
}

//We need a specific object to implement the MACLayer interface (the virtual methods). In this case we use XBee but could be anyone capable of implementing that interface
#if (UIP_LLADDR_LEN == UIP_802154_LONGADDR_LEN)
XBeeMACLayer macLayer(TEST_CHANNEL, TEST_PANID, TEST_MACMODE);
#else
XBeeMACLayer macLayer(TEST_CHANNEL, TEST_PANID, TEST_MACMODE, EUT);
#endif

//This will be the destination IP address
IPv6Address addr_dest;

//This will be the ping destination IP address
IPv6Address ping_addr_dest;

//If we are configured to be a router, this is our prefix. Up to now we hardcode it here. Note: The aaaa::/64 prefix is the only known by the 6loPAN context so it will achieve a better compression.
IPv6Address prefix(TEST_PREFIX, 0, 0, 0, 0, 0, 0, 0, 0);

char udp_data[50];
int udp_data_length = 0;
IPv6Address sender_address;
uint16_t dest_port;

#define COMMAND_MAX_BYTES 100
char buffer_command[COMMAND_MAX_BYTES];
u16_t addr[8];

char coap_ping[4] = {0x40, 0x00, 0xbe, 0xbe};

void udp_callback(char *data, int datalen, int sender_port, IPv6Address &sender_addr){
  if (coap_ping[0] == data[0]){
    SERIAL.println("COAP PING received");
    //take the message that should have the format: 
    //we put the VERSION to 1, TYPE to 3, the OC to 0 and the code to 0. We leave the same MID
    data[0] = 0x70;
    data[1] = 0;
    SERIAL.println("Sending RESET message to sender");
    //now we send this to the sender. It is only 4 bytes
    IPv6Stack::udpSend(sender_addr, sender_port, data, 4);
    delay(50);
  }else if (data[0] == 0x70 && data[1] == 0 && data[2] == coap_ping[2] && data[3] == coap_ping[3]){//test coap reset + MID
    SERIAL.println("COAP RESET received");
    delay(50);
  }
}

char getIpAddress(char* address, u16_t* addr){
  char* str;  
  int i = 0;
  SERIAL.println(address);
  while ((str = strtok_r(address, ":", &address)) != NULL){
    addr[i] = (u16_t) strtoul(str, NULL, 16);
    ++i;
  }
  if (i==8)
    return 1;
  return 0;
}

//send a coap ping message
void send_coap_ping(IPv6Address& ping_addr_dest, u16_t port){
  IPv6Stack::udpSend(ping_addr_dest, port, coap_ping, 4);
  delay(50);
}

//receives and interprates the input from the host
void hostInput()
{
    char* cmd;
    char* arg;
    int a = 0;
    u8_t arg_num = 0;
    u8_t hl = 0;
    u16_t port = 0;
    CommandType type;
    
    if(SERIAL.available()){
      a = SERIAL.readBytesUntil('\n', buffer_command, COMMAND_MAX_BYTES);
    }
    if (a != 0){
      buffer_command[a] = 0;
      SERIAL.println(buffer_command);  
      cmd = buffer_command;
      while ((arg = strtok_r(cmd, " ", &cmd)) != NULL){
        arg_num++;
        switch(arg_num){
          case 1:
            if(strncmp(arg,"ping6", 5) != 0){
              if(strncmp(arg,"coaping", 5) != 0){
                SERIAL.println("---");
                SERIAL.println("invalid command!");
                SERIAL.println("---");
                return;
              }else{
                type = COAP_PING; 
              }
            }else{
              type = IP_PING; 
            }
            break;
          case 2:
            if (getIpAddress(arg, addr)){
        
              ping_addr_dest.setAddress(addr[0], addr[1],addr[2],
                          addr[3],addr[4],addr[5],addr[6],addr[7]);
                
            }else{
              SERIAL.println("---");
              SERIAL.println("invalid address!");
              SERIAL.println("---");
              return;
            }
            break;
          case 3:
            if (type == IP_PING){
              hl = (u8_t) strtoul(arg, NULL, 10);
              break;
            }else{
              if (type == COAP_PING){
                //get the destination port
                port = (u16_t) strtoul(arg, NULL, 10);
                break;
              } 
            }
        }
        
      }
      if (arg_num >= 2){
        if (type == IP_PING){
          IPv6Stack::ping(ping_addr_dest, 4, hl);
          SERIAL.print("PING ");
          if (hl){
            SERIAL.print("(HL=");
            SERIAL.print(hl);
            SERIAL.print(") ");
          }        
          SERIAL.print("sent to address ");
          ping_addr_dest.print();
        }else{
          if (type == COAP_PING){
            if (port != 0){
              SERIAL.println("Sending COAP PING");
              send_coap_ping(ping_addr_dest, port);
            }else{
               SERIAL.print("Port was not specified or incorrect");
            }
          }
        }
      }
    }
}

//if not using RPL, neigbors will be added manually so initialize their IP addresses
#if !UIP_CONF_IPV6_RPL

  IPv6Address nbr_ipaddr1;

  IPv6Address nbr_ipaddr2;

  IPv6Address nbr_global_ipaddr1;

  IPv6Address nbr_global_ipaddr2;

  nbr_ipaddr1.setLinkLocalPrefix();
  nbr_ipaddr1.setIID(EUT1);
  IPv6llAddress nbr_lladdr1(EUT1);
  
  nbr_ipaddr2.setLinkLocalPrefix();
  nbr_ipaddr2.setIID(EUT2);
  IPv6llAddress nbr_lladdr2(EUT2);
  
  nbr_global_ipaddr1.setPrefix(TEST_PREFIX);
  nbr_global_ipaddr1.setIID(EUT1);
  IPv6llAddress nbr_global_lladdr1(EUT1);
  
  nbr_global_ipaddr2.setPrefix(TEST_PREFIX);
  nbr_global_ipaddr2.setIID(EUT2);
  IPv6llAddress nbr_global_lladdr2(EUT2);
  
#endif

void setup(){  
  SERIAL.begin(9600);
  delay(1000);
  SERIAL.println();
  SERIAL.print("MEMORY LEFT:");
  SERIAL.println(mem());
  delay(100);
  
  // init network-device
  if (!IPv6Stack::initMacLayer(&macLayer)){
    SERIAL.println("CANNOT INITIALIZE XBEE MODULE.. CANNOT CONTINUE");
    while (true){};
  }
  
  //init IP Stack
  IPv6Stack::initIpStack();  
  SERIAL.println("IPV6 INITIALIZED");
  delay(100);
  
  //init UDP
  IPv6Stack::initUdp(UDP_PORT);
  SERIAL.println("UDP INITIALIZED");
  delay(100);
  
#if !UIP_CONF_IPV6_RPL
  //This line is added to specify our prefix    
  IPv6Stack::setPrefix(prefix, 64); 
  #if UIP_CONF_ROUTER
    //Add FE80:: 
    IPv6Stack::addNeighbor(nbr_ipaddr1, nbr_lladdr1);
    IPv6Stack::addNeighbor(nbr_ipaddr2, nbr_lladdr2);
    //Add PREFIX::
    IPv6Stack::addNeighbor(nbr_global_ipaddr1, nbr_lladdr1);
    IPv6Stack::addNeighbor(nbr_global_ipaddr2, nbr_lladdr2);
  #else
    //Add default router if we are not a router
    IPv6Stack::addDefaultRouter(nbr_ipaddr1, nbr_lladdr1);
    IPv6Stack::addDefaultRouter(nbr_ipaddr2, nbr_lladdr2);
  #endif
#elif UIP_CONF_ROUTER
  //If using RPL only the router adds the prefix. Nodes will foind it within DIO messages 
  IPv6Stack::setPrefix(prefix, 64); 
#endif

  SERIAL.println("SETUP FINISHED");
  delay(100);  

}

void loop(){
  //Always need to poll timers in order to make the IPv6 Stack work
  IPv6Stack::pollTimers();  
  //We always check if we got anything. If we did, process that with the IPv6 Stack
  if (IPv6Stack::receivePacket()){
      //If we have udp data received, see if it is a COAP message. If it is a coap ping, respond
      if (IPv6Stack::udpDataAvailable()){
        udp_data_length = IPv6Stack::getUdpDataLength();
        IPv6Stack::getUdpData(udp_data);
        IPv6Stack::getUdpSenderIpAddress(sender_address);
        udp_callback(udp_data, udp_data_length, IPv6Stack::getUdpSenderPort(), sender_address);
      }
  }
  //Check for host input to send messages
  hostInput();  
  delay(10);
}

/*---------------------------------------------------------------------------*/
