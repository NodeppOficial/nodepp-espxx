#ifndef NODEPP_MUTEX
#define NODEPP_MUTEX

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "arduino/mutex.cpp"
#else
    #error "This OS Does not support mutex.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif