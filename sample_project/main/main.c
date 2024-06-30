// SPDX-FileCopyrightText: 2023 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <Arduino_GFX_Library.h>
#include <Adafruit_FT6206.h>
#include <Adafruit_CST8XX.h>

//Arduino_DataBus *bus = new Arduino_HWSPI(16 /* DC */, 5 /* CS */);
//Arduino_GFX *gfx = new Arduino_ILI9341(bus, 17 /* RST */);

Arduino_XCA9554SWSPI *expander = new Arduino_XCA9554SWSPI(
        PCA_TFT_RESET, PCA_TFT_CS, PCA_TFT_SCK, PCA_TFT_MOSI,
        &Wire, /*0x40*/0x3F);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
        TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
        TFT_R1, TFT_R2, TFT_R3, TFT_R4, TFT_R5,
        TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
        TFT_B1, TFT_B2, TFT_B3, TFT_B4, TFT_B5,
        1 /* hsync_polarity */, 50 /* hsync_front_porch */, 2 /* hsync_pulse_width */, 44 /* hsync_back_porch */,
        1 /* vsync_polarity */, 16 /* vsync_front_porch */, 2 /* vsync_pulse_width */, 18 /* vsync_back_porch */
        //    ,1, 30000000
        );

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
        // 2.1" 480x480 round display
        480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */,
        true /* auto_flush */, expander, GFX_NOT_DEFINED /* RST */,
        TL021WVC02_init_operations, sizeof(TL021WVC02_init_operations));

uint16_t *colorWheel;

// The Capacitive touchscreen overlays uses hardware I2C (SCL/SDA)

// Most touchscreens use FocalTouch with I2C Address often but not always 0x48!
//#define I2C_TOUCH_ADDR 0x48

// 2.1" 480x480 round display use CST826 touchscreen with I2C Address at 0x15
#define I2C_TOUCH_ADDR 0x15  // often but not always 0x48!

Adafruit_FT6206 focal_ctp = Adafruit_FT6206();  // this library also supports FT5336U!
Adafruit_CST8XX cst_ctp = Adafruit_CST8XX();
bool touchOK = false;        // we will check if the touchscreen exists
bool isFocalTouch = false;

// https://chat.openai.com/share/8edee522-7875-444f-9fea-ae93a8dfa4ec
void generateColorWheel(uint16_t *colorWheel) {
    int width = gfx->width();
    int height = gfx->height();
    int half_width = width / 2;
    int half_height = height / 2;
    float angle;
    uint8_t r, g, b;
    int index, scaled_index;

    for(int y = 0; y < half_height; y++) {
        for(int x = 0; x < half_width; x++) {
            index = y * half_width + x;
            angle = atan2(y - half_height / 2, x - half_width / 2);
            r = uint8_t(127.5 * (cos(angle) + 1));
            g = uint8_t(127.5 * (sin(angle) + 1));
            b = uint8_t(255 - (r + g) / 2);
            uint16_t color = RGB565(r, g, b);

            // Scale this pixel into 4 pixels in the full buffer
            for(int dy = 0; dy < 2; dy++) {
                for(int dx = 0; dx < 2; dx++) {
                    scaled_index = (y * 2 + dy) * width + (x * 2 + dx);
                    colorWheel[scaled_index] = color;
                }
            }
        }
    }
}

void app_main(void)
{

    gfx->begin();
    gfx->fillScreen(BLACK);
    gfx->setCursor(10, 10);
    gfx->setTextColor(RED);
    gfx->println("Hello World!");


  //  Serial.begin(115200);
    //while (!Serial) delay(100);

#ifdef GFX_EXTRA_PRE_INIT
    GFX_EXTRA_PRE_INIT();
#endif

    //Serial.println("Beginning");
    // Init Display

    Wire.setClock(1000000); // speed up I2C
    if (!gfx->begin()) {
       // Serial.println("gfx->begin() failed!");
    }

   // Serial.println("Initialized!");

    gfx->fillScreen(BLACK);

    expander->pinMode(PCA_TFT_BACKLIGHT, OUTPUT);
    expander->digitalWrite(PCA_TFT_BACKLIGHT, HIGH);

    colorWheel = (uint16_t *) ps_malloc(gfx->width() * gfx->height() * sizeof(uint16_t));
    if (colorWheel) {
        generateColorWheel(colorWheel);
        gfx->draw16bitRGBBitmap(0, 0, colorWheel, gfx->width(), gfx->height());
    }

    if (!focal_ctp.begin(0, &Wire, I2C_TOUCH_ADDR)) {
        // Try the CST826 Touch Screen
        if (!cst_ctp.begin(&Wire, I2C_TOUCH_ADDR)) {
           // Serial.print("No Touchscreen found at address 0x");
           // Serial.println(I2C_TOUCH_ADDR, HEX);
            touchOK = false;
        } else {
           // Serial.println("CST826 Touchscreen found");
            touchOK = true;
            isFocalTouch = false;
        }
    } else {
      //  Serial.println("Focal Touchscreen found");
        touchOK = true;
        isFocalTouch = true;
    }

    while(true) {
        if (touchOK) {
            if (isFocalTouch && focal_ctp.touched()) {
                TS_Point p = focal_ctp.getPoint(0);
             //   Serial.printf("(%d, %d)\n", p.x, p.y);
                gfx->fillRect(p.x, p.y, 5, 5, WHITE);
            } else if (!isFocalTouch && cst_ctp.touched()) {
                CST_TS_Point p = cst_ctp.getPoint(0);
             //   Serial.printf("(%d, %d)\n", p.x, p.y);
                gfx->fillRect(p.x, p.y, 5, 5, WHITE);
            }
        }

        // use the buttons to turn off
        if (! expander->digitalRead(PCA_BUTTON_DOWN)) {
            expander->digitalWrite(PCA_TFT_BACKLIGHT, LOW);
        }
        // and on the backlight
        if (! expander->digitalRead(PCA_BUTTON_UP)) {
            expander->digitalWrite(PCA_TFT_BACKLIGHT, HIGH);
        }
    }

}
