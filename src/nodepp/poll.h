/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_POLL
#define NODEPP_POLL

#if _KERNEL == NODEPP_KERNEL_ARDUINO
    #include "wait.h"
    #include "arduino/poll.cpp"
#else
    #error "poll.h is not supported by this OS"
#endif

#endif

/*────────────────────────────────────────────────────────────────────────────*/