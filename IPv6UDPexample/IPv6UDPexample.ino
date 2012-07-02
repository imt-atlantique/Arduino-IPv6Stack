/*
  Arduino IPv6 stack example
 
 This sketch connects two Arduino Mega with wireless shield and Xbee
 mounted. This sketch demonstrates use of the IPv6 stack library.
 
 Circuit:
 * Arduino Mega 2560
 * Wireless shield w/ Xbee Series 1
 
 created 29 June 2012
 by Alejandro Lampropulos
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
 
 */
 
#include <IPv6Stack.h>
#include <XBeeMACLayer.h>

#define UDP_PORT 8765

#define SEND_INTERVAL		(10 * CLOCK_SECOND)

int mem(){
  uint8_t * heapptr, * stackptr;
  stackptr = (uint8_t *)malloc(4);
  heapptr = stackptr;
  free(stackptr);               
  stackptr = (uint8_t *)(SP);
  return stackptr - heapptr;
}

MACLayer* macLayer = new XBeeMACLayer();

#define UDP_MAX_DATA_LEN 200

char udp_send[UDP_MAX_DATA_LEN];

IPv6Address addr_dest;

#if UIP_CONF_ROUTER
  IPv6Address prefix(0xaa, 0xaa, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif

IPv6Timer send_timer;

bool change_addr_prefix(IPv6Address &address){
  IPv6Address globalAddress;
  if (IPv6Stack::getGlobalPreferredAddress(globalAddress)){//we get our global address' prefix, given by our router, to send the response to the sender using global addresses (this will allow us to see routing)
    address.setAddressValue(globalAddress.getAddressValue(0), 0);
    address.setAddressValue(globalAddress.getAddressValue(1), 1);
    address.setAddressValue(globalAddress.getAddressValue(2), 2);
    address.setAddressValue(globalAddress.getAddressValue(3), 3);
    return true;
  }
  return false;
}

void udp_callback(char *data, int datalen, int sender_port, IPv6Address &sender_addr){  
  Serial.println(mem());
  delay(100);
  data[datalen] = 0;
  Serial.println();
  Serial.println();
  Serial.print("Data received from ip: ");
  sender_addr.print();  
  Serial.print(" port: ");
  Serial.print(sender_port);
  Serial.print(", data: ");
  //Serial.println(data);
  while (IPv6Stack::udpDataAvailable()){
     Serial.print(IPv6Stack::readUdpData());
  }
  Serial.println();
  
  delay(SEND_INTERVAL/2);//take SEND_INTERVAL/2 to resend
    
  Serial.println("Sending response..");
  Serial.println(); 
  delay(50); 
  int j;
  for(j=0; j<datalen; ++j){
    udp_send[datalen-1-j] = data[j]; 
  }
  change_addr_prefix(sender_addr);
  addr_dest = sender_addr; //now if our message is not responded, it will be resent from the main loop to the same destination
  IPv6Stack::udp_send(sender_addr, udp_send, datalen);
  delay(50);
  send_timer.restart();//each time we receive something we reset this timer so we should not send in the main loop as long as our message is responded
}

void setup(){  
  Serial.begin(9600);
  delay(1000);
  Serial.println();
  Serial.print("MEMORY LEFT:");
  Serial.println(mem());
  delay(100);
  // init network-device
  if (!IPv6Stack::init_macLayer(macLayer)){
    Serial.println("CANNOT INITIALIZE XBEE MODULE.. CANNOT CONTINUE");
    while (true){};
  }
  IPv6Stack::init_ipStack();  
  Serial.println("IPV6 INITIALIZED");
  delay(500);
  IPv6Stack::init_udp(UDP_PORT, UDP_PORT);
  Serial.println("UDP INITIALIZED");
  delay(500);
  
  #if !UIP_CONF_ROUTER
      send_timer.set(SEND_INTERVAL);
      Serial.println("SEND TIMER SET");
      delay(50);      
  #else
      //This line is added to specify our prefix    
      IPv6Stack::set_prefix(prefix, 64);    
  #endif /*UIP_CONF_ROUTER*/
  
  Serial.println("SETUP FINISHED!");
  delay(100);
}

char udp_data[UDP_MAX_DATA_LEN];
int udp_data_length = 0;
IPv6Address sender_address;

void loop(){
  IPv6Stack::poll_timers();  
#if !UIP_CONF_ROUTER
  if (send_timer.expired()){
      send_timer.reset();
      Serial.println();
      Serial.println("Sending data..");
      delay(50);   
      IPv6Stack::udp_send(addr_dest, "0123456789", 10);
  }
#endif
  if (IPv6Stack::receive_packet()){
      IPv6Stack::process_ipStack();  
#if !UIP_CONF_ROUTER
      if (IPv6Stack::udpDataAvailable()){
        udp_data_length = IPv6Stack::getUdpDataLength();
        IPv6Stack::getUdpData(udp_data);
        IPv6Stack::getUdpSenderIpAddress(sender_address);
        udp_callback(udp_data, udp_data_length, IPv6Stack::getUdpSenderPort(), sender_address);
      }
#endif
  }
  delay(100);
}
