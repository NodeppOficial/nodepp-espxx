#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include <esp_wifi.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class wifi_t {
public:

    virtual ~wifi_t() noexcept {}
             wifi_t() noexcept {}

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
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_wifi_init( &cfg ); esp_wifi_start();

        wifi_scan_config_t scan_config = {
            .show_hidden = true, .ssid = 0,
            .channel = 0,        .bssid = 0,
        };

        esp_wifi_scan_start( &scan_config, true );
        
        ptr_t<wifi_ap_record_t> ap_info; uint16_t len=0; 
        while( !ap_info.empty() ) {
            if( esp_wifi_connect() == ESP_OK ) {
                esp_wifi_scan_get_ap_num(&len); ap_info.resize(len);
                esp_wifi_scan_get_ap_records(&len,&ap_info);
            }   process::next();
        }

        return ap_info;
    }

    void connect( const string_t& ssid, const string_t& port ) const noexcept {
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        esp_wifi_init( &cfg ); wifi_config_t wifi_config; 

        memset( wifi_config, 0, sizeof(wifi_config_t) );
                wifi_config.sta = {
                    .ssid     = ssid.c_str(),
                    .password = port.c_str()
                };

        esp_wifi_set_mode( WIFI_MODE_STA );
        esp_wifi_set_config( ESP_IF_WIFI_STA, &wifi_config );

        if( esp_wifi_start() != ESP_OK )
          { process::error("can't connect to ssid"); }

        wifi_ap_record_t ap_info; 
        while( true ) {
            if( esp_wifi_connect() == ESP_OK ) {
                esp_wifi_scan_get_ap_record(&ap_info);
            if( ap_info.rssi > -50 ){ break; }
            }   process::next();
        }
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/