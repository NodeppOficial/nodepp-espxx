#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    ulong seconds(){ return ::esp_timer_get_time() / 1000000; }

    ulong millis(){ return ::esp_timer_get_time() / 1000; }

    ulong micros(){ return ::esp_timer_get_time(); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace process {

    ulong now(){ return ::esp_timer_get_time() / 1000; }

    void delay( ulong time ){ 
        if( time == 0 ){ return; }
        ets_delay_us( time * 1000 ); 
    }

    void  yield(){ delay( TIMEOUT ); }

}}