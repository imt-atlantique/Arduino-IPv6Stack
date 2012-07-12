#ifndef __ARDUINODEBUG_H__
#define __ARDUINODEBUG_H__

#define ARDUINO_DEBUG 1

#ifdef __cplusplus
  extern "C" {
#else
  #include "uip.h"
#endif  
 
extern void arduino_debug_init();
extern void arduino_debug(char* s);
extern void arduino_debug_hex(int b);
extern void arduino_debug_dec(long b);
extern void arduino_debug_address(uip_ip6addr_t* address);
extern void arduino_debug_lladdr(uip_lladdr_t* address);

#ifdef __cplusplus
}
#endif

#endif /* __ARDUINODEBUG_H__ */
