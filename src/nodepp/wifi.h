#ifndef NODEPP_WIFI
#define NODEPP_WIFI

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "dns.h"
    #include "event.h"
    #include "arduino/wifi.cpp"
#else
    #error "This OS Does not support wifi.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif