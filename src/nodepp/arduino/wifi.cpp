#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include <esp_wifi.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class wifi_t {
public:

    event_t<void*,wifi_promiscuous_pkt_type_t> onPackage;
    event_t<except_t>                          onError;
    event_t<>                                  onClose;

   ~wifi_t() noexcept { onClose.emit(); } wifi_t() noexcept {
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); esp_wifi_init( &cfg ); 
        esp_wifi_set_storage(WIFI_STORAGE_RAM); esp_wifi_set_mode(WIFI_MODE_NULL);
        esp_wifi_start();
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

    void unpipe() const noexcept {
        esp_wifi_set_promiscuous(false);
    }

    void pipe() const noexcept {

        esp_wifi_set_promiscuous(true);
        auto self = type::bind(this);

        wifi_promiscuous_filter_t filter = {
            .filter_mask = WIFI_PROMIS_FILTER_MASK_ALL
        };   esp_wifi_set_promiscuous_filter(&filter);

        esp_wifi_set_promiscuous_rx_cb([=]( void* buf, wifi_promiscuous_pkt_type_t type ){
            self->onPackage( buff, type );
        }); esp_wifi_set_channel( 1, WIFI_SECOND_CHAN_NONE );

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
        
        wifi_config_t wifi_config;
        wifi_config.ap.channel = channel;
        wifi_config.ap.ssid    = ssid.get();
        wifi_config.ap.max_connection = MAX_SOCKET;

        if( !pass.empty() ){
            wifi_config.ap.password = pass.get();
            wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
        }

        esp_wifi_set_mode( WIFI_MODE_AP );
        esp_wifi_set_config( ESP_IF_WIFI_AP, &wifi_config );

        if( esp_wifi_start() != ESP_OK )
          { _EERROR( onError, "can't start wifi mode"); }

    }

    void connect( const string_t& ssid, const string_t& pass ) const noexcept {
        
        wifi_config_t wifi_config; 
        wifi_config.sta.ssid     = ssid.get();
        wifi_config.sta.password = pass.get();

        esp_wifi_set_mode( WIFI_MODE_STA );
        esp_wifi_set_config( ESP_IF_WIFI_STA, &wifi_config );

        if( esp_wifi_start() != ESP_OK )
          { _EERROR( onError, "can't start wifi mode"); }

        if( esp_wifi_connect() != ESP_OK )
          { _EERROR( onError, "can't connect to ssid"); }

    }

};}

/*────────────────────────────────────────────────────────────────────────────*/