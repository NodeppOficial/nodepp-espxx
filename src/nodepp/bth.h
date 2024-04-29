#ifndef NODEPP_BLUETOOTH
#define NODEPP_BLUETOOTH

/*────────────────────────────────────────────────────────────────────────────*/

#include "bsocket.h"

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp {

/*────────────────────────────────────────────────────────────────────────────*/

class bth_t {
protected:

    struct NODE {
        int                        state = 0;
        agent_t                    agent;
        function_t<void,bsocket_t> func ;
    };  ptr_t<NODE> obj;
    
public: bth_t() noexcept : obj( new NODE() ) {}

    event_t<bsocket_t> onConnect;
    event_t<bsocket_t> onSocket;
    event_t<>          onClose;
    event_t<except_t>  onError;
    event_t<bsocket_t> onOpen;
    
    /*─······································································─*/

    bth_t( decltype(NODE::func) _func, agent_t* opt ) noexcept : obj( new NODE() ) 
         { obj->func=_func; obj->agent=opt==nullptr?agent_t():*opt;  }
    
    /*─······································································─*/
    
    void     close() const noexcept { if( obj->state<0 ){ return; } obj->state=-1; onClose.emit(); }
    bool is_closed() const noexcept { return obj == nullptr ? 1 : obj->state < 0; }
    
    /*─······································································─*/

    void listen( const string_t& host, int port, decltype(NODE::func)* cb=nullptr ) const noexcept {
        if( obj->state == 1 ){ return; } obj->state = 1; auto inp = type::bind( this );

        bsocket_t *sk = new bsocket_t; 
                  sk->AF  = AF_BTH; 
                  sk->PROT= IPPROTO_BTH;
                  sk->socket( host, port ); 
        
        if(   sk->bind()  < 0 ){ process::error(onError,"Error while binding Bluetooth");   close(); delete sk; return; }
        if( sk->listen()  < 0 ){ process::error(onError,"Error while listening Bluetooth"); close(); delete sk; return; }

        onOpen.emit(*sk); if( cb != nullptr ){ (*cb)(*sk); } 
        
        process::task::add([=](){
            static int _accept = 0; 
        coStart

            while( sk != nullptr ){ _accept = sk->_accept();
                if( inp->is_closed() || !sk->is_available() )
                  { break; } elif ( _accept != -2 )
                  { break; } coYield(1);
            }

            if( _accept == -1 ){ process::error(inp->onError,"Error while accepting Bluetooth"); coGoto(2); }
            elif ( !sk->is_available() || inp->is_closed() ){ coGoto(2); }
            else { bsocket_t cli( _accept ); if( cli.is_available() ){ 
                   process::poll::add([=]( bsocket_t cli ){
                        cli.set_sockopt( inp->obj->agent ); 
                        inp->onSocket.emit( cli ); 
                        inp->obj->func( cli ); 
                        return -1;
                   }, cli );
            } coGoto(0); }

            coYield(2); inp->close(); delete sk; 
        
        coStop
        });

    }

    void listen( const string_t& host, int port, decltype(NODE::func) cb ) const noexcept { 
         listen( host, port, &cb ); 
    }
    
    /*─······································································─*/

    void connect( const string_t& host, int port, decltype(NODE::func)* cb=nullptr ) const noexcept {
        if( obj->state == 1 ){ return; } obj->state = 1; auto inp = type::bind( this );

        bsocket_t sk = bsocket_t(); 
                  sk.AF  = AF_BTH; 
                  sk.PROT= IPPROTO_BTH;
                  sk.socket( host, port ); 
                  sk.set_sockopt( obj->agent );

        if( sk.connect() < 0 ){ process::error(onError,"Error while connecting Bluetooth"); close(); return; }
        if( cb != nullptr ){ (*cb)(sk); } sk.onClose.on([=](){ inp->close(); });
        onOpen.emit(sk); sk.onOpen.emit(); onSocket.emit(sk); obj->func(sk);
    }

    void connect( const string_t& host, int port, decltype(NODE::func) cb ) const noexcept { 
         connect( host, port, &cb ); 
    }

};

/*────────────────────────────────────────────────────────────────────────────*/

namespace bth {

    bth_t server( const bth_t& server ){ server.onSocket([=]( bsocket_t cli ){
        ptr_t<_file_::read> _read = new _file_::read;
        cli.onDrain.once([=](){ cli.free(); });

        server.onConnect.once([=]( bsocket_t cli ){ process::poll::add([=](){
            if(!cli.is_available() ) { cli.close(); return -1; }
            if((*_read)(&cli)==1 )   { return 1; } 
            if(  _read->c  <=  0 )   { return 1; }
            cli.onData.emit(_read->y); return 1;
        }) ; });

        process::task::add([=](){
            server.onConnect.emit(cli); return -1;
        });

    }); return server; }

    /*─······································································─*/

    bth_t server( agent_t* opt=nullptr ){
        auto server = bth_t( [=]( bsocket_t /*unused*/ ){}, opt );
        bth::server( server ); return server; 
    }

    /*─······································································─*/

    bth_t client( const bth_t& client ){ client.onOpen.once([=]( bsocket_t cli ){
        ptr_t<_file_::read> _read = new _file_::read;
        cli.onDrain.once([=](){ cli.free(); });

        process::poll::add([=](){
            if(!cli.is_available() ) { cli.close(); return -1; }
            if((*_read)(&cli)==1 )   { return 1; } 
            if(  _read->c  <=  0 )   { return 1; }
            cli.onData.emit(_read->y); return 1;
        });

    }); return client; }

    /*─······································································─*/

    bth_t client( agent_t* opt=nullptr ){
        auto client = bth_t( [=]( bsocket_t /*unused*/ ){}, opt );
        bth::client( client ); return client; 
    }
    
}

/*────────────────────────────────────────────────────────────────────────────*/

}

#endif