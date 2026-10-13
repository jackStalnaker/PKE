#pragma once

#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
    lgfx::Panel_ST7796 _panel;
    lgfx::Bus_SPI _bus;

public:
    LGFX() {
        // SPI bus configuration
        {
            auto cfg = _bus.config();

            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 20000000;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = true;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;

            cfg.pin_sclk = 8;     // XIAO D8
            cfg.pin_mosi = 10;    // XIAO D10
            cfg.pin_miso = -1;    // Not connected
            cfg.pin_dc   = 4;     // XIAO D2

            _bus.config(cfg);
            _panel.setBus(&_bus);
        }

        // ST7796 panel configuration
        {
            auto cfg = _panel.config();

            cfg.pin_cs   = 3;     // XIAO D1
            cfg.pin_rst  = 5;     // XIAO D3
            cfg.pin_busy = -1;

            cfg.panel_width  = 320;
            cfg.panel_height = 480;
            cfg.memory_width  = 320;
            cfg.memory_height = 480;

            _panel.config(cfg);
        }

        setPanel(&_panel);
    }
};
