/*********************************************************************
  WiFi-clock with TM1637 7-segment display

  Based on: NTP TZ DST - bare minimum
  NetWork Time Protocol - Time Zone - Daylight Saving Time
  https://werner.rothschopf.net/202011_arduino_esp8266_ntp_en.htm
  https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/NTP-TZ-DST/NTP-TZ-DST.ino
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see https://www.gnu.org/licenses/.
  
  Code by Potatopasty https://tg-music.neocities.org
  potatopasty@pm.me
*********************************************************************/

#include <ESP8266WiFi.h>            
#include <WifiUDP.h>
#include <time.h>                  
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
#include <NTPClient.h>
#include <TM1637Display.h>

#define CLK 0  // GPIO0 = D3
#define DIO 4  // GPIO4 = D2

#define MY_NTP_SERVER "europe.pool.ntp.org"      // Europe   https://www.pool.ntp.org
#define MY_TZ "EET-2EEST,M3.5.0/3,M10.5.0/4"     // Helsinki https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

unsigned long previousMillis = 0;
const long interval = 1000;

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

TM1637Display display(CLK, DIO);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, MY_NTP_SERVER);

/* Globals */
time_t now;                         // this is the epoch
tm tm;                              // the structure tm holds time information in a more convient way

void showTime() {
  time(&now);                       // read the current time
  localtime_r(&now, &tm);           // update the structure tm with the current time
}

/*********************************************************************/

void setup() {

  configTime(MY_TZ, MY_NTP_SERVER);

  display.setBrightness(1);  // Set the display brightness from 0-7
  display.showNumberDec(0, true);  // Show text "0000" when not connected or connecting to WiFi.

  // Set your own password and network name. IP-Address: 192.168.4.1 for Wifi setup.
  WiFiManager wifiManager; // Use this, do not hard code your wifi name and password
  wifiManager.autoConnect("AP-NAME", "AP-PASSWORD");  // Make up your own WiFi-name and password.

  // start network

  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  display.setSegments(SEG_DONE);  // Show text "dOnE" when connected to WiFi.
  delay(5000);  //Show the dOnE-text for 5 seconds and then show the time.
}

/*********************************************************************/

void loop() {

  /* Show time on display, and update every second */

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      showTime();
      // Display hours
      display.showNumberDecEx((tm.tm_hour), 0b11100000, false, 2, 0);  // False = No leading zero
  
      // Display minutes
      display.showNumberDecEx((tm.tm_min), 0b11100000, true, 2, 2);  // True = With leading zero
      previousMillis = currentMillis;
    }
}

/*********************************************************************/
