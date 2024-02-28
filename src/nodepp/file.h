#ifndef NODEPP_FILE
#define NODEPP_FILE

/*────────────────────────────────────────────────────────────────────────────*/

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "event.h"
    #include "generator.h"
    #include "arduino/file.cpp"
#else
    #error "This OS Does not support file.h"
#endif

/*────────────────────────────────────────────────────────────────────────────*/

#endif