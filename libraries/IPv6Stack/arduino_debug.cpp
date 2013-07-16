#include <HardwareSerial.h>
#include <Arduino.h>
#include "uip.h"
#include "arduino_debug.h"

void arduino_debug_init() {
    #if ARDUINO_DEBUG
    SERIAL.begin(9600);
    #endif
}

void arduino_debug(char* s) {
    #if ARDUINO_DEBUG
    SERIAL.println(s);
    delay(20);
    #endif
}

void arduino_debug_hex(int b) {
  #if ARDUINO_DEBUG
    SERIAL.println(b, HEX);
    #endif
}

void arduino_debug_dec(long b) {
  #if ARDUINO_DEBUG
    SERIAL.println(b, DEC);
    #endif
}
    
void arduino_debug_address(uip_ip6addr_t* address) {
  #if ARDUINO_DEBUG
    int i;
    for (i=0; i<16; i++) {    
      if (address->u8[i]<0xf) 
        SERIAL.print("0");   
      SERIAL.print(address->u8[i], HEX);
      if (((i%2) == 1) && (i != 15))
        SERIAL.print(":");
    }    
    SERIAL.println();  
    #endif
}

void arduino_debug_lladdr(uip_lladdr_t* address){
  #if ARDUINO_DEBUG
    int i;
    SERIAL.print(address->addr[0], HEX);
    for (i=1; i< UIP_LLADDR_LEN; i++) {  
      SERIAL.print(":");
      SERIAL.print(address->addr[i], HEX);
    }
    SERIAL.println();  
    #endif
}

    
    
