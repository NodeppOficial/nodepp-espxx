#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include <esp_wifi.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class wifi_t {
protected:

    struct NODE {
        wifi_config_t wifi_cfg;
        wifi_init_config_t cfg;
        int   state = 1;
    };  ptr_t<NODE> obj;

public:

    event_t<void*,wifi_promiscuous_pkt_type_t> onPackage;
    event_t<except_t>                          onError;
    event_t<>                                  onClose;

   ~wifi_t() noexcept { 
       if( obj.count() > 1 ){ return; }
       if( obj->state == 0 ){ return; }
       obj->state = 0; onClose.emit(); 
    } 
   
   wifi_t() noexcept : obj( new NODE() ) {
        obj->cfg = WIFI_INIT_CONFIG_DEFAULT(); esp_wifi_init( &obj->cfg ); 
        esp_wifi_set_storage(WIFI_STORAGE_RAM); 
        esp_wifi_set_mode(WIFI_MODE_NULL);
    }

    int turn_on() const noexcept {
        return esp_wifi_start() == ESP_OK;
    }

    int turn_off() const noexcept {
        return esp_wifi_stop() == ESP_OK;
    }

    string_t get_hostname() const noexcept {
        return dns::get_hostname();
    }

    ptr_t<wifi_ap_record_t> get_ssid_list() const noexcept {

        wifi_scan_config_t scan_config;
        scan_config.show_hidden = true;
        scan_config.ssid    = 0;
        scan_config.channel = 0;
        scan_config.bssid   = 0;

        if( esp_wifi_scan_start( &scan_config, true ) != ESP_OK )
          { _EERROR( onError, "can't scan wifi ssid list" ); }  
        
        ptr_t<wifi_ap_record_t> ap_info; uint16_t len=0; 
        while( !ap_info.empty() ) { if( esp_wifi_connect() == ESP_OK ) {
               esp_wifi_scan_get_ap_num(&len); ap_info.resize(len);
               esp_wifi_scan_get_ap_records(&len,&ap_info);
           }   process::next();
        }

        return ap_info;
    }

    void softAP( const string_t& ssid, const string_t& pass, int channel ) const noexcept {
        
        obj->wifi_cfg.ap.channel = channel;
        obj->wifi_cfg.ap.max_connection = MAX_SOCKET;
        memcpy( obj->wifi_cfg.ap.ssid, ssid.get(), min( (uchar)32, (uchar)ssid.size() ));

        if( !pass.empty() ){
            memcpy( obj->wifi_cfg.ap.password, pass.get(), min( (uchar)32, (uchar)pass.size() ));
            obj->wifi_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
        }

        esp_wifi_set_mode( WIFI_MODE_AP );
        esp_wifi_set_config( WIFI_IF_AP, &obj->wifi_cfg );

        if( esp_wifi_start() != ESP_OK )
          { _EERROR( onError, "can't start wifi mode"); }

    }

    void connect( const string_t& ssid, const string_t& pass ) const noexcept {
        
        memcpy( obj->wifi_cfg.sta.ssid    , ssid.get(), min( (uchar)32, (uchar)ssid.size() ));
        memcpy( obj->wifi_cfg.sta.password, pass.get(), min( (uchar)32, (uchar)pass.size() ));

        esp_wifi_set_mode( WIFI_MODE_STA );
        esp_wifi_set_config( WIFI_IF_STA, &obj->wifi_cfg );

        if( esp_wifi_start() != ESP_OK )
          { _EERROR( onError, "can't start wifi mode"); }

        if( esp_wifi_connect() != ESP_OK )
          { _EERROR( onError, "can't connect to ssid"); }

    }

};}

/*────────────────────────────────────────────────────────────────────────────*/