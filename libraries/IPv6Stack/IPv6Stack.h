/*
 Arduino IPv6 stack class
 
 This class represents the IPv6 stack (ip, udp, rpl) which needs the implemetation of the link layer to send and receive packets
 
 created 29 June 2012
 by Alejandro Lampropulos (alejandro.lampropulos@telecom-bretagne.eu)
 Telecom Bretagne Rennes, France
 
 Copyright (c) 2012, Telecom Bretagne Rennes, France.
 All rights reserved.
 
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
 
 This work has been partly funded by French Agence Nationale de la Recherche
 under contract ANR-09-VERS-017 ARESA2
 
 */
#ifndef IPV6STACK_H
#define IPV6STACK_H

extern "C" {  
    #include <inttypes.h>    
	#include "uip.h"
    #include "simple_udp.h"
}    

#include "MACLayer.h"
#include "IPv6Address.h"
#include "IPv6llAddress.h"
#include "IPv6Timer.h"

class IPv6Stack
{
  private:
  
    static uint8_t mac_output(uip_lladdr_t *lladdr);
    static MACLayer* mac; //this pointer is for the mac_output function to be able to access the mac layer
    static struct simple_udp_connection broadcast_connection;
    static void receiver(struct simple_udp_connection *c, const uip_ipaddr_t *sender_addr,
         uint16_t sender_port, const uip_ipaddr_t *receiver_addr, uint16_t receiver_port, const uint8_t *data, uint16_t datalen);
    	//Processes ipv6 received packets if any
    static void processIpStack();
         
    static const uint8_t* udp_data;
    static uint16_t udp_data_length;
    static uint16_t udp_data_length_left;
    static const uip_ipaddr_t* sender_addr;
    static uint16_t sender_port;
    static IPv6llAddress ll_destination_address;
    static IPv6llAddress ll_source_address;
	
	static uint8_t count;
              
  public:
    
	//Initializes the mac layer (First function to call in setup). If it returns false, the MAC layer could not be initialized, so should stop working
    static bool initMacLayer(MACLayer* macLayer);
	//Initializes the variables of the IPv6 Stack (Second function to call in setup)
    static void initIpStack();
	//Initializes the UDP listening in the local port
    static void initUdp(uint16_t local_port);    
	//Sends a UDP message. to: the destination IPv6 address, remote_port: the destination port, data: pointer to data, datalen: the length of the data. 
    static void udpSend(const IPv6Address &to, uint16_t remote_port, const void *data, uint16_t datalen);
	//Returns true if a packet has been received at MAC layer and has been successfully decompressed (6lowPAN)
    static bool receivePacket();     
	//Refreshes timers and if expired, runs the corresponding methods (sending RS, RA, NS, etc)
    static void pollTimers(); //Note: polling timers put received udp data to 0
	//Adds manually an address to our list or addresses
    static void addAddress(IPv6Address &address);
	//When configured as router, this method allows to set a prefix to the router so it can communicate it with the RA
    static void setPrefix(IPv6Address &prefix, uint8_t prefix_length);//For ROUTERS only
	//In case we have a global address which has passed DAD, this returns true and copies the address to the parameter
    static bool getGlobalPreferredAddress(IPv6Address& address);
    //Returns the amount of bytes available from the last received UDP message (if any). Note: If timers are polled, this becomes 0 and that data is lost
    static int udpDataAvailable();
	//Reads one byte of the last UDP message received and gets ready to return the next byte of the data received (if any).
    static char readUdpData();
	//Returns the complete length of the data received by the last UDP message (if any). Note: If timers are polled, this becomes 0 and that data is lost
    static uint16_t getUdpDataLength();
	//Returns the data of the last UDP message received (if any), copied in the buffer passed by parameter (this buffer should have enough space for the data)
    static void getUdpData(char* buffer);
	//Returns the port used by the sender of the last received UDP message
    static int getUdpSenderPort();
	//Copies the IP address of the sender of the last received UDP message
    static void getUdpSenderIpAddress(IPv6Address &address);
	//Sends a PING (ICMPv6 Echo request) message to the specified address
	static void ping(IPv6Address &destination, uint8_t datalength);
};	

		
#endif

