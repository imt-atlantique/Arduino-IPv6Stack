
#include "XBeeMACLayer.h"

#define DEBUG 0

#if DEBUG
#define PRINTF(m) arduino_debug(m)
#define PRINTF_HEX(m) arduino_debug_hex(m)
#else
#define PRINTF(...)
#define PRINTF_HEX(...)
#endif

XBeeMACLayer::XBeeMACLayer(){
}

// serial high
uint8_t shCmd[] = {'S','H'};
// serial low
uint8_t slCmd[] = {'S','L'};

AtCommandRequest atRequest = AtCommandRequest(shCmd);

AtCommandResponse atResponse = AtCommandResponse();

#define AT_RESPONSE_MAX_ATTEMPTS 20

bool XBeeMACLayer::sendAtCommand(uint8_t& mac_position) {
  // send the command
  xbee.send(atRequest);

  //TEMPORARILY, we send the request and wait for the attempt several times until we got it. This is because we can receive other packets that will be placed in the same buffer and they must be ignored until getting our AT response
  for(int i=0; i<AT_RESPONSE_MAX_ATTEMPTS; ++i){
    
    // wait up to 3 seconds for the status response
    if (xbee.readPacket(3000)) {
      // got a response!
  
      // should be an AT command response
      if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
        xbee.getResponse().getAtCommandResponse(atResponse);
  
        if (atResponse.isOk()) {
  
          if (atResponse.getValueLength() == 4) {
            
            for (int i = 0; i < atResponse.getValueLength(); i++) {
              my_mac.addr[mac_position++] = atResponse.getValue()[i];
            }
            return true;
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
  return false;
}

bool XBeeMACLayer::init(){    
    xbee.begin(9600); 
    delay(3000); //wait for the XBEE to initialize
        
    uint8_t mac_position = 0;
    // get SH
    if (!sendAtCommand(mac_position))
      return false; 
         
    // set command to SL
    atRequest.setCommand(slCmd);  
    if (!sendAtCommand(mac_position))
      return false; 
        
    return true;
}


XBeeAddress64 destAddr64(0, 0);
XBeeRequest* xbeeRequest;
uint32_t dest = 0;

bool XBeeMACLayer::send(const uip_lladdr_t* lladdr_dest, uint8_t* data, uint16_t length){
    if (!rimeaddr_cmp((rimeaddr_t*)lladdr_dest, &rimeaddr_null)){
      if (UIP_LLADDR_LEN == UIP_802154_LONGADDR_LEN){
          ((char*)(&dest))[3] = lladdr_dest->addr[0];
          ((char*)(&dest))[2] = lladdr_dest->addr[1];
          ((char*)(&dest))[1] = lladdr_dest->addr[2];
          ((char*)(&dest))[0] = lladdr_dest->addr[3];
          destAddr64.setMsb(dest);
          ((char*)(&dest))[3] = lladdr_dest->addr[4];
          ((char*)(&dest))[2] = lladdr_dest->addr[5];
          ((char*)(&dest))[1] = lladdr_dest->addr[6];
          ((char*)(&dest))[0] = lladdr_dest->addr[7];
          destAddr64.setLsb(dest);
        Tx64Request longReq(destAddr64, data, length);
        xbeeRequest = &longReq;
      }else{
        Tx16Request shortReq(*((uint16_t*)lladdr_dest->addr[0]), data, length);
        xbeeRequest = &shortReq;
      }
    }else{//if it is null then send to broadcast
      Tx16Request shortReq(0xFFFF, data, length);
      xbeeRequest = &shortReq;
    }
    xbee.send(*xbeeRequest);
    return true;
}

ZBRxResponse rxResp = ZBRxResponse();

bool XBeeMACLayer::receive(uip_lladdr_t &lladdr_src, uip_lladdr_t &lladdr_dest, uint8_t* data, uint16_t& length){
    xbee.readPacket();    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      //ToDO: OPTIMIZE!!!
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
        //arduino_debug("ZB RESP");
        xbee.getResponse().getZBRxResponse(rxResp);
        memcpy(data, rxResp.getData(), rxResp.getDataLength());
        length = rxResp.getDataLength();
        
        //get the src and destination addresses
        if(rxResp.getOption() == ZB_BROADCAST_PACKET){ //Then packet was a broadcast.. destination address is 0xFFFF
          lladdr_dest.addr[0] = 0xFF;
          lladdr_dest.addr[1] = 0xFF;
        }else{
           //get dest address (it is my own)
           for(int i=0; i<UIP_LLADDR_LEN; ++i){
             lladdr_dest.addr[i] = my_mac.addr[i];
           }
        }
        //TODO: Check if it is 64 or 16 bit address
        *((uint32_t*)lladdr_src.addr[0]) = rxResp.getRemoteAddress64().getMsb();
        *((uint32_t*)lladdr_src.addr[UIP_LLADDR_LEN/2]) = rxResp.getRemoteAddress64().getLsb();
        
        return true;
      } else if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
	    if (mac_addr_size == UIP_802154_SHORTADDR_LEN){
                Rx16Response rx16 = Rx16Response();                
                xbee.getResponse().getRx16Response(rx16);
                memcpy(data, rx16.getData(), rx16.getDataLength());
                length = rx16.getDataLength();
                
                //get the src and destination addresses
                if(rx16.getOption() == ZB_BROADCAST_PACKET){ //Then packet was a broadcast.. destination address is 0xFFFF
                  lladdr_dest.addr[0] = 0xFF;
                  lladdr_dest.addr[1] = 0xFF;
                }else{
                  //get dest address (it is my own)
                  for(int i=0; i<UIP_LLADDR_LEN; ++i){
                    lladdr_dest.addr[i] = my_mac.addr[i];
                  }
                }
                
                *((uint16_t*)lladdr_src.addr[0]) = rxResp.getRemoteAddress16();
	    }else
  		return false;
        } else {
	  if (mac_addr_size == UIP_802154_LONGADDR_LEN){
                Rx64Response rx64 = Rx64Response();
                xbee.getResponse().getRx64Response(rx64);
                memcpy(data, rx64.getData(), rx64.getDataLength());
                length = rx64.getDataLength();
                                
                //get destination addresses
                if(rx64.getOption() == ZB_BROADCAST_PACKET){ //Then packet was a broadcast.. destination address is 0xFFFF
                  //arduino_debug("BCAST 64");
                  lladdr_dest.addr[0] = 0;//xFF;
                  lladdr_dest.addr[1] = 0;//xFF;
                }else{
                  //get dest address (it is my own)
                  for(int i=0; i<UIP_LLADDR_LEN; ++i){
                    lladdr_dest.addr[i] = my_mac.addr[i];
                  }
                }
                
                //get sorce address
                for(int i=0; i<UIP_LLADDR_LEN; ++i){
                  lladdr_src.addr[i] = rx64.getFrameData()[i];
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

const uip_lladdr_t& XBeeMACLayer::getMacAddress(){
  return my_mac;
}
