/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp {

class poll_t {
protected:

    struct POLLFD { int fd; int md; };

    struct NODE {
        queue_t<POLLFD> ev;
        ptr_t<int>      ls;
    };  ptr_t<NODE>    obj;

public:

    wait_t<int>    onWrite;
    wait_t<int>    onError;
    wait_t<int>    onRead;

public: poll_t() noexcept : obj( new NODE() ) {}

    virtual ~poll_t() noexcept { 
        if ( obj.count() > 1 ){ return; }
        for( auto x : obj->ev.data() ) 
             onError.emit( x.md ); 
    }

    /*─······································································─*/

    ptr_t<int> get_last_poll() const noexcept { 
        ptr_t<int> result = obj->ls; return result; 
    }

    /*─······································································─*/

    int next () noexcept { return emit(); }

    int emit () noexcept { 
        static POLLFD x;
    coStart 
    
        if( obj->ev.empty() ){ coEnd; }

        while ( obj->ev.next() ) { x=obj->ev.get()->data;
            if( x.md == 1 ){ obj->ev.erase(obj->ev.get()); onWrite.emit(x.fd); obj->ls={{ 1, x.fd }}; coNext; }
          elif( x.md == 0 ){ obj->ev.erase(obj->ev.get());  onRead.emit(x.fd); obj->ls={{ 0, x.fd }}; coNext; }
          else             { obj->ev.erase(obj->ev.get()); onError.emit(x.fd); obj->ls={{-1, x.fd }}; coNext; }
        }

    coStop
    };

    /*─······································································─*/

    void push_write( const int& fd ) noexcept { 
	     obj->ev.push({ fd, 1 }); 
    }

    void push_read( const int& fd ) noexcept { 
         obj->ev.push({ fd, 0 }); 
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/