#include <stdio.h>
#include <FT6206.h>

// The Capacitive touchscreen overlays uses hardware I2C (SCL/SDA)

// Most touchscreens use FocalTouch with I2C Address often but not always 0x48!
//#define I2C_TOUCH_ADDR 0x48

// 2.1" 480x480 round display use CST826 touchscreen with I2C Address at 0x15
#define I2C_TOUCH_ADDR 0x15  // often but not always 0x48!


FT6206 focal_ctp = FT6206();  // this library also supports FT5336U!
bool touchOK = false;        // we will check if the touchscreen exists
bool isFocalTouch = false;



extern void app_main(void){

    if (!focal_ctp.begin(0, /*&Wire,*/ I2C_TOUCH_ADDR)) {
        //        // Try the CST826 Touch Screen
        //        if (!cst_ctp.begin(&Wire, I2C_TOUCH_ADDR)) {
        //            Serial.print("No Touchscreen found at address 0x");
        //            Serial.println(I2C_TOUCH_ADDR, HEX);
        //            touchOK = false;
        //        } else {
        //            Serial.println("CST826 Touchscreen found");
        //            touchOK = true;
        //            isFocalTouch = false;
        //        }
    } else {
        //        Serial.println("Focal Touchscreen found");
        touchOK = true;
        isFocalTouch = true;
    }

    while(touchOK){
        if (isFocalTouch && focal_ctp.touched()) {
            TS_Point p = focal_ctp.getPoint(0);
//            Serial.printf("(%d, %d)\n", p.x, p.y);
//            gfx->fillRect(p.x, p.y, 5, 5, WHITE);
//        } else if (!isFocalTouch && cst_ctp.touched()) {
//            CST_TS_Point p = cst_ctp.getPoint(0);
//            Serial.printf("(%d, %d)\n", p.x, p.y);
//            gfx->fillRect(p.x, p.y, 5, 5, WHITE);
        }
    }
}
