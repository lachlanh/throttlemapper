/* this needs to be structured properly..
   holding area for all the display related bits*/

// #include <U8g2lib.h>
//#include <U8x8lib.h>
//ssd1306
//#include <Wire.h>
//#include <SPI.h>
//#include <ssd1306.h>


// U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2);
//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8();
//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

// U8X8_SSD1306_128X64_NONAME_HW_I2C u8g2();

//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R2, 3, 2, U8X8_PIN_NONE);

void setup() {
  //setup the display
  //u8x8.begin();
  // u8g2.begin();
  //u8x8.setFlipMode(1);
  //ssd1306_128x64_i2c_init();
  //ssd1306_128x64_init()
  //ssd1306_setFixedFont(ssd1306xled_font6x8);
  //ssd1306_clearScreen();
}

void updateDisplay(long cadence, long kph, float volts, float ah)
{
  //Serial.print(F("updating display"));
  //Serial.println(F("Free RAM = ")); //F function does the same and is now a built in library, in IDE > 1.0.0
  //Serial.println(freeMemory(), DEC);

  // ssd1306_setFixedFont(ssd1306xled_font6x8);
  // ssd1306_clearScreen();
  // sprintf(str, "%2ld", cadence);
  // ssd1306_printFixed(0,  8, str, STYLE_NORMAL);
  // ssd1306_printFixed(32,  8, "CAD", STYLE_NORMAL);




  /* u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0,0);
  u8x8.print(cadence);
  u8x8.setCursor(4,0);
  u8x8.print(F("cad"));
  
  u8x8.setCursor(8,0);
  u8x8.print(volts,1);
  u8x8.setCursor(22,0);
  u8x8.print(F("v")); */
  
  /*  
  u8g2.firstPage();
  do
  {
    //x offset, y offset
    
    u8g2.setFont(u8g2_font_logisoso26_tr);
    sprintf(str, "%2ld", cadence);
    u8g2.drawStr(0,26,str);
    sprintf(str, "%2ld", kph);
    u8g2.drawStr(0,56,str);
    dtostrf(volts,2, 0, str);
    u8g2.drawStr(0,86,str);
    dtostrf(ah,2, 0, str);
    u8g2.drawStr(0,116,str);

    u8g2.setFont(u8g2_font_logisoso16_tr);
    u8g2.drawStr(34,26,"cd");//x offset, y offset
    u8g2.drawStr(34,56,"kh");
    u8g2.drawStr(34,86,"v");
    u8g2.drawStr(34,116,"ah");
    
    //x offset, y offset
    u8g2.setFont(u8g2_font_logisoso26_tr);
    sprintf(str, "%2ld", cadence);
    u8g2.drawStr(8, 26, str);
    sprintf(str, "%2ld", kph);
    u8g2.drawStr(8, 56, str);
    dtostrf(volts, 3, 1, str);
    u8g2.drawStr(66, 26, str);
    dtostrf(ah, 3, 1, str);
    u8g2.drawStr(66, 56, str);

    u8g2.setFont(u8g2_font_artossans8_8r);
    u8g2.drawStr(42, 26, "cd"); //x offset, y offset
    u8g2.drawStr(42, 56, "rpm");
    u8g2.drawStr(120, 8, "v");
    u8g2.drawStr(112, 56, "ah");
    
    //u8g2.drawStr(0, 64, "hello");
  } while (u8g2.nextPage());
*/
  //Serial.print(F("updated display"));
}

