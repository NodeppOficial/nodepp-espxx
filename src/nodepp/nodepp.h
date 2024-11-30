/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_NODEPP
#define NODEPP_NODEPP

/*────────────────────────────────────────────────────────────────────────────*/

#include "import.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    array_t<string_t> args;
    
    /*─······································································─*/

    void start(){}

    void start( int argc, char** args ){
        int i=0; do {
            process::args.push(args[i]);
        }   while( i ++< argc - 1 ); process::start();
    }

    /*─······································································─*/

    template< class... T >
    int  spawn( const T&... args ){ return ::system(args...); }

    template< class... T >
    void error( const T&... msg ){ _ERROR( msg... ); }

    void exit( int err=0 ){ ::exit(err); }

    void abort(){ ::abort(); }

    /*─······································································─*/

    void stop(){  while( !process::empty() ){
        onSIGNEXT.emit(); process::next();
    }}

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif