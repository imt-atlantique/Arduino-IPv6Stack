/*
 Arduino IPv6 XBee MAC Layer implementation
 
 This class implements the methods defined by the MACLayer interface. It is developed to use the XBee Series 1 module.
 
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
 
 */
#include "XBeeMACLayer.h"

#define DEBUG DEBUG_NONE
#include "uip_debug.h"

XBeeMACLayer::XBeeMACLayer(){
}

// serial high
uint8_t shCmd[] = {'S','H'};
// serial low
uint8_t slCmd[] = {'S','L'};
// cca failures
uint8_t ecCmd[] = {'E','C'};

uint8_t mac_position;

AtCommandRequest atRequest = AtCommandRequest(shCmd);

AtCommandResponse atResponse = AtCommandResponse();

#define AT_RESPONSE_MAX_ATTEMPTS 5
#define AT_REQUEST_MAX_ATTEMPTS 20

bool XBeeMACLayer::getResponseMAC(){
          if (atResponse.getValueLength() == 4) {            
            for (int i = 0; i < atResponse.getValueLength(); i++) {
              my_mac.setAddressValue(atResponse.getValue()[i], mac_position++);
            }
            return true;
          }
          return false;
}

uint16_t cca_retries = 0;

bool XBeeMACLayer::getResponseCCAFailure(){
          PRINTF("AT CMD LENGTH: ");
          PRINTF_HEX(atResponse.getValueLength());
          if (atResponse.getValueLength() == 2) {            
            for (int i = 0; i < atResponse.getValueLength(); i++) {
              ((char*)(&cca_retries))[1] = atResponse.getValue()[0];
              ((char*)(&cca_retries))[0] = atResponse.getValue()[1];
            }
            PRINTF("AT CMD VALUE: ");
            PRINTF_HEX(cca_retries);
            return true;
          }
          return false;
}

bool XBeeMACLayer::sendAtCommand() {
  for(int i=0; i<AT_REQUEST_MAX_ATTEMPTS; ++i){
    // send the command
    xbee.send(atRequest);
  
    //TEMPORARILY, we send the request and wait for the attempt several times until we got it. This is because we can receive other packets that will be placed in the same buffer and they must be ignored until getting our AT response
    for(int i=0; i<AT_RESPONSE_MAX_ATTEMPTS; ++i){
      
      // wait up to 3 seconds for the status response
      if (xbee.readPacket(1000)) {
        // got a response!
    
        // should be an AT command response
        if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
          xbee.getResponse().getAtCommandResponse(atResponse);
    
          if (atResponse.isOk()) {
            if (atRequest.getCommand() == shCmd || atRequest.getCommand() == slCmd){
              return getResponseMAC();
            }else{
              if (atRequest.getCommand() == ecCmd){
                 return getResponseCCAFailure();
              }
            }
          } 
          else {
            PRINTF("Command return error code: ");
            PRINTF_HEX(atResponse.getStatus());        
          }
        } else {
          PRINTF("Expected AT response but got ");
          PRINTF_HEX(xbee.getResponse().getApiId());
        }   
      } else {
        // at command failed
        if (xbee.getResponse().isError()) {
          PRINTF("Error reading packet.  Error code: ");  
          PRINTF_HEX(xbee.getResponse().getErrorCode());
        } 
        else {
          PRINTF("No response from radio");  
        }
      }
    }
  }
  return false;
}

bool XBeeMACLayer::init(){    
    xbee.begin(9600); 
    delay(3000); //wait for the XBEE to initialize
        
    mac_position = 0;    
    // get SH
    if (!sendAtCommand())
      return false; 
    // set command to SL
    atRequest.setCommand(slCmd);
    // set command to SL
    atRequest.setCommand(slCmd);  
    if (!sendAtCommand())
        return false; 
    return true;
}


XBeeAddress64 destAddr64(0, 0);
XBeeRequest* xbeeRequest;
uint32_t dest = 0;
uint16_t dest16 = 0;

#define XBEE_SEND_OK 0x0
#define XBEE_SEND_NO_ACK 0x1
#define XBEE_SEND_COLLISION 0x2
#define XBEE_SEND_PURGED 0x3 //This happens when Coordinator times out of an indirect transmission (should not use coordinators in our case)

TxStatusResponse txStatus;

uint16_t ecCommandValue = 0; //reset counter to 0

#define CCA_RETRIES_FAIL 10;

int XBeeMACLayer::getNumberOfTransmissions(){
      atRequest.setCommand(ecCmd); 
      atRequest.setCommandValue((uint8_t*) &ecCommandValue);
      if (!sendAtCommand())
        return CCA_RETRIES_FAIL; 
      return cca_retries;
}

MACTransmissionStatus XBeeMACLayer::send(const IPv6llAddress& lladdr_dest, uint8_t* data, uint16_t length, int &number_transmissions){
  unsigned long send_start_time;
  unsigned int sending_time;
  uint8_t status;
    if (!lladdr_dest.isNull()){      
      if (UIP_LLADDR_LEN == UIP_802154_LONGADDR_LEN){
          ((char*)(&dest))[3] = lladdr_dest.getAddressValue(0);
          ((char*)(&dest))[2] = lladdr_dest.getAddressValue(1);
          ((char*)(&dest))[1] = lladdr_dest.getAddressValue(2);
          ((char*)(&dest))[0] = lladdr_dest.getAddressValue(3);
          destAddr64.setMsb(dest);
          ((char*)(&dest))[3] = lladdr_dest.getAddressValue(4);
          ((char*)(&dest))[2] = lladdr_dest.getAddressValue(5);
          ((char*)(&dest))[1] = lladdr_dest.getAddressValue(6);
          ((char*)(&dest))[0] = lladdr_dest.getAddressValue(7);
          destAddr64.setLsb(dest);
        Tx64Request longReq(destAddr64, data, length);
        xbeeRequest = &longReq;
      }else{
        ((char*)(&dest))[1] = lladdr_dest.getAddressValue(0);
        ((char*)(&dest))[0] = lladdr_dest.getAddressValue(1);
        Tx16Request shortReq(dest16, data, length);
        xbeeRequest = &shortReq;
      }
    }else{//if it is null then send to broadcast
      Tx16Request shortReq(0xFFFF, data, length);
      xbeeRequest = &shortReq;
    }
    send_start_time = millis();
    xbee.send(*xbeeRequest);
    
    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    for(int i=0; i<AT_RESPONSE_MAX_ATTEMPTS; ++i){
      if (xbee.readPacket(5000)) {
          // got a response!
          // should be a znet tx status            	
      	if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
      	   xbee.getResponse().getTxStatusResponse(txStatus);
              PRINTF("");
              PRINTF("");
              PRINTF("STATUS:");
              PRINTF_HEX(txStatus.getStatus());
              PRINTF("SENDING TIME:");
              PRINTF_HEX(sending_time);
              PRINTF("");
      	   // get the delivery status, the fifth byte
             switch (txStatus.getStatus()) {
               case XBEE_SEND_OK:
                 number_transmissions = getNumberOfTransmissions();
                 ++number_transmissions;//we have always one (the first one), plus the number of times that we had cca failure (collision)
                 PRINTF("TRANSMISSIONS:");
                 PRINTF_HEX(number_transmissions);
                 return MAC_TX_STATUS_OK;
               case XBEE_SEND_NO_ACK:
                 return MAC_TX_STATUS_NO_ACK;
               case XBEE_SEND_COLLISION:
                 number_transmissions = getNumberOfTransmissions();
                 ++number_transmissions;//we have always one (the first one), plus the number of times that we had cca failure (collision)
                 PRINTF("TRANSMISSIONS:");
                 PRINTF_HEX(number_transmissions);
                 return MAC_TX_STATUS_COLLISION;
               case XBEE_SEND_PURGED:
               default:
                 return MAC_TX_STATUS_ERR;
             }     
          }   
          PRINTF("RESP RECEIVED BUT NOT TX RESPONSE.. LOOPING....");
      } else if (xbee.getResponse().isError()) {
        //nss.print("Error reading packet.  Error code: ");  
        //nss.println(xbee.getResponse().getErrorCode());
        // or flash error led
        PRINTF("TX RESPONSE IS ERROR");
        //return MAC_TX_STATUS_ERR;
      } else {
        PRINTF("TX RESPONSE IS ERROR FATAL");
        // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
        return MAC_TX_STATUS_ERR;
      }
    }
    return MAC_TX_STATUS_ERR;
}

ZBRxResponse rxResp = ZBRxResponse();

bool XBeeMACLayer::receive(IPv6llAddress& lladdr_src, IPv6llAddress& lladdr_dest, uint8_t* data, uint16_t& length){
    xbee.readPacket();    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
                PRINTF("");
                PRINTF("MAC LAYER RESP");
                PRINTF("");
        // got a rx packet
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
                PRINTF("");
                PRINTF("16 RX");
                PRINTF("");
          if (mac_addr_size == UIP_802154_SHORTADDR_LEN){
                Rx16Response rx16 = Rx16Response();                
                xbee.getResponse().getRx16Response(rx16);
                memcpy(data, rx16.getData(), rx16.getDataLength());
                length = rx16.getDataLength();
                
                //get the src and destination addresses
                if(rx16.getOption() == ZB_BROADCAST_PACKET){ //Then packet was a broadcast.. destination address is 0xFFFF, we use null value 0
                  lladdr_dest.setAddressValue(0, 0);
                  lladdr_dest.setAddressValue(0, 1);
                }else{
                  //get dest address (it is my own)
                  lladdr_dest = my_mac;
                }
                
                lladdr_src.setAddressValue(rxResp.getRemoteAddress16() >> 8, 0);
                lladdr_src.setAddressValue(rxResp.getRemoteAddress16() & 0x00FF, 1);
          }else
            return false;
        } else {           
                PRINTF("");
                PRINTF("64 RX");
                PRINTF("");
            if (mac_addr_size == UIP_802154_LONGADDR_LEN){
                Rx64Response rx64 = Rx64Response();
                xbee.getResponse().getRx64Response(rx64);
                memcpy(data, rx64.getData(), rx64.getDataLength());
                length = rx64.getDataLength();
                                
                //get destination addresses
                if(rx64.getOption() == ZB_BROADCAST_PACKET){ //Then packet was a broadcast.. destination address is 0xFFFF, we use null value 0
                  PRINTF("BCAST 64");
                  lladdr_dest.setAddressValue(0, 0);
                  lladdr_dest.setAddressValue(0, 1);
                }else{
                  //get dest address (it is my own)
                  lladdr_dest = my_mac;
                }
                
                //get sorce address
                for(int i=0; i<UIP_LLADDR_LEN; ++i){
                  lladdr_src.setAddressValue(rx64.getFrameData()[i], i);
                }
                
             }else
               return false;
        }
                
        return true;
        
      }
    } else if (xbee.getResponse().isError()) {
      PRINTF("ERROR");
      uint8_t code = xbee.getResponse().getErrorCode();
      if (code == CHECKSUM_FAILURE){
         PRINTF("CHECKSUM_FAILURE");
      }else if (code == PACKET_EXCEEDS_BYTE_ARRAY_LENGTH){
          PRINTF("PACKET_EXCEEDS_BYTE_ARRAY_LENGTH");
      }else if(code == UNEXPECTED_START_BYTE ){
            PRINTF("UNEXPECTED_START_BYTE");
      }
    } 
    
    return false;
}

const IPv6llAddress& XBeeMACLayer::getMacAddress(){
  return my_mac;
}
