/*

'<'+
truncate([SpeedLocal]) +'\t'+ 
truncate([MaxSpeedLocal]) +'\t'+ 
format([CurrentLapTime],'mm\\"ss"\'"fff') + '\t'+
format([CurrentLap],'000') + '\t'+
format([CompletedLaps],'000') + '\t'+
isnull([PersistantTrackerPlugin.AllTimeBestLiveDeltaSeconds],0) +'\t'+
isnull([PersistantTrackerPlugin.AllTimeBestLiveDeltaProgressSeconds],0) +'\t'+
[GameRawData.Physics.PacketId] + '>\n'

 */
 
/* CONFIG */
#define BTN_DOWN   27
#define BTN_SELECT 26
#define BTN_UP     25
#define LED_PIN    16
 
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>
#include "fonts/impact19.h"
#include "fonts/impact14.h"
#include "fonts/impact20.h"
#include "fonts/impact22.h"
#include "fonts/impact38.h"
#include "fonts/impact48t33.h"
#include "fonts/impact48laptime.h"
#include "fonts/impact36laptime.h"
#include "fonts/impact42laptime.h"
#include "fonts/impact72.h"
#include "fonts/impact65.h"
#include "fonts/impact68.h"
#include "fonts/ArialNB18.h"
#include "graphics/logo.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 5, /* dc=*/ 13, /* reset=*/ 12);    // Enable U8G2_16BIT in u8g2.h

Adafruit_NeoPixel strip(2, LED_PIN, NEO_GRB);

// datos tal como los recibo por serial
int startIdx;

//Live Speed
char speed[5];
String maxx;
char maxSpeed[5];
int MaxSpeed=0;
int maxspeeed=0;

//Lap Time
String lapp;
char laptime[20];

//Lap count
String currentlapcount;
String completedlapscount;
char currentlap[5];
char completedlaps[5];

//Lateral G  -----------------------
String lateralgcount;
char lateralg[20];

//Longitudinal G  -----------------------
String longitudinalgcount;
char longitudinalg[20];

//Lateral G Bar

//Live Delta
char liveDelta[10];
char liveDeltaProgress[10];

// datos convertidos para visualizar
float liveDeltaProgressFloat;
float liveDeltaFloat;
char liveDeltaL[5];
char liveDeltaR[5];

char buff[256];
int bytesReceived;
int retries=0;
bool newDatagram;

int s1=0,s2=0,s3=0,s4=0,s5=0,s6=0; //--------------------
int currentScreen = 0;
bool inMenu=false;
uint8_t current_selection = 0;
bool buttonState = false;

void setup(void) {
    Serial.begin(115200);
    u8g2.begin(
        BTN_SELECT,    /* select      */
        BTN_DOWN,      /* next        */
        BTN_UP,        /* prev        */
        U8X8_PIN_NONE, /* up          */
        U8X8_PIN_NONE, /* down        */
        U8X8_PIN_NONE  /* home|cancel */
    );
    u8g2.setContrast(255);
    strip.begin();
    strip.setBrightness(32);
    splashAnimation();
}

void loop(void) {
 u8g2.setFlipMode(1);
    int button = readButtons();
    switch(button) {
        case 1:
            if(currentScreen<5)
            currentScreen=currentScreen+1;        delay(500);

            break;
        case 3: 
            if(currentScreen>0)
            currentScreen=currentScreen-1;        delay(500);

            break;
    }
    getDatagram();
    if (newDatagram) parseDatagram();

    if (currentScreen == 0) {
       if(s1==0){predictivelaptiming();}
        renderDelta();
        renderLeds();
        s2=0;
        s3=0;
        s4=0;
        s5=0;
        s6=0;
    } 
    if (currentScreen == 1) {
       if(s2==0){livespeed();}
       renderSpeed();
       s1=0;       
       s3=0;
       s4=0;
       s5=0;
       s6=0;
    }
    if (currentScreen == 2) {
       if(s3==0){laptimingtext();}
       lapptime();
       s1=0;
       s2=0;       
       s4=0;
       s5=0;
       s6=0;
    }
    if (currentScreen == 3) {
       if(s4==0){lapcounttext();}
       Lapcount();
       s1=0;
       s2=0;
       s3=0;
       s5=0;
       s6=0;
    }
    if (currentScreen == 4) {
       if(s5==0){lateralgtext();}
       Lateralg();
       s1=0;
       s2=0;
       s3=0;
       s4=0;
       s6=0;
    }
    if (currentScreen == 5) {
       if(s6==0){longitudinalgtext();}
       Longitudinalg();
       s1=0;
       s2=0;
       s3=0;
       s4=0;
       s5=0;
    }
}
    
int readButtons(void) {
   if (digitalRead(BTN_DOWN) == LOW) {return 1;}
   if (digitalRead(BTN_SELECT) == LOW) {return 2;}
   if (digitalRead(BTN_UP) == LOW) {return 3;}
   
   return 0;
}

void ledsOff() {
    strip.setPixelColor(0, strip.Color(0,0,0));
    strip.setPixelColor(1, strip.Color(0,0,0));
    strip.show();
}

void renderLeds() {
    int delta = liveDeltaProgressFloat * 1000;
    int minDelta = 20;
    int maxDelta = 100;
    int v0;
    int v1;

    if (liveDeltaProgressFloat >= 0) {
        v0 = map(delta, minDelta, minDelta + ((maxDelta-minDelta)/2), 0,255);
        v1 = map(delta, minDelta + ((maxDelta-minDelta)/2), maxDelta, 0,255);
        if (v0 < 0) v0 =0;
        if (v1 < 0) v1 =0;
        if (v0 > 255) v0=255;
        if (v1 > 255) v1=255;
        strip.setPixelColor(0, strip.Color(v0,0,0));
        strip.setPixelColor(1, strip.Color(v1,0,0));

//        Serial.print(v0);
//        Serial.print(":");
//        Serial.println(v1);
    } else {
        v0 = map(-delta, minDelta, minDelta + ((maxDelta-minDelta)/2), 0,255);
        v1 = map(-delta, minDelta + ((maxDelta-minDelta)/2), maxDelta, 0,255);
        if (v0 < 0) v0 =0;
        if (v1 < 0) v1 =0;
        if (v0 > 255) v0=255;
        if (v1 > 255) v1=255;
        strip.setPixelColor(0, strip.Color(0,v0,0));
        strip.setPixelColor(1, strip.Color(0,v1,0));

        //strip.setPixelColor(1, strip.Color(0,v1,0));
//        Serial.print(-v0);
//        Serial.print(":");
//        Serial.println(-v1);
    }
    delay(1);
    strip.show();
    delay(1);
}

void renderDelta() {

    u8g2.clearBuffer();
    
    // delta text
    u8g2.setFont(impact38);
    int deltaLwidth = u8g2.getUTF8Width(liveDeltaL);
    u8g2.drawStr(120 - (deltaLwidth), 30, liveDeltaL);
    u8g2.drawStr(127, 30, liveDeltaR);
    u8g2.drawBox(120, 24, 6,6);
    
    // centerline
    u8g2.drawHLine(0,33,255);

    // tick lines
    for (int i=0; i<=11; i++) {
        u8g2.drawBox(2+(i*25), 35, 2, 5);
    }
    int barWidth;
//    // delta progress bar
//    if (liveDeltaProgressFloat >= 0) {
//        barWidth = liveDeltaProgressFloat * 250; // 1 segundo son 25px
//        u8g2.drawBox(127,42, barWidth+2, 22);
//    } else {
//        barWidth = abs(liveDeltaProgressFloat) * 250; // 1 segundo son 25px
//        u8g2.drawBox(127-barWidth,42, barWidth+2, 22);
//    }
    // delta bar 
    if (liveDeltaFloat >= 0) {
        barWidth = liveDeltaFloat * 25; // 1 segundo son 25px
        u8g2.drawBox(127,42, barWidth+2, 22);
    } else {
        barWidth = abs(liveDeltaFloat) * 25; // 1 segundo son 25px
        u8g2.drawBox(127-barWidth,42, barWidth+2, 22);
    }
    //u8g2.drawBox(128,42,25,22);
    u8g2.sendBuffer();
}

void renderSpeed() {
    
    if(MaxSpeed<atoi(speed)){MaxSpeed=atoi(speed);}
    if (digitalRead(BTN_SELECT) == LOW)  {MaxSpeed=0;}
    maxx = String(MaxSpeed);
    u8g2.clearBuffer();
    // SPEED
    u8g2.setFont(impact72);
    int width = u8g2.getUTF8Width(speed);
    int posx = 190 - width;
    u8g2.drawStr(posx, 64-0, speed);
    // KMH
    u8g2.setFont(ArialNB18);
    u8g2.drawStr(201, 64-7, "km/h");
    //max speed
    u8g2.setFont(ArialNB18);
    u8g2.drawStr(10, 32, "Max");
    u8g2.setFont(impact19);
    u8g2.drawStr(10, 55,maxx.c_str());
    u8g2.sendBuffer();   
} 


void lapptime(){

 lapp = String(laptime);
 
    u8g2.clearBuffer();
    u8g2.setFont(impact42laptime);
    //u8g2.drawStr(5, 60, lapp.c_str());  //impact48laptime
    //u8g2.drawStr(30, 54, lapp.c_str());  //impact36laptime
    u8g2.drawStr(18, 57, lapp.c_str());  //impact42laptime
    u8g2.setFont(impact19);
    //u8g2.drawStr(201, 25, "Best");
    //u8g2.drawStr(215, 40, "+");
    //u8g2.drawStr(230, 60, liveDelta);
     u8g2.sendBuffer();   

  }

float rnd(float number) { 
    float value = (int)(number* 100 + .5); 
    return (float)value / 100; 
}

void getDatagram() {
    if (Serial.available() <= 0) {
        newDatagram = false;
        return;
    }
    memset(buff, 0, sizeof buff);
    bytesReceived = Serial.readBytesUntil('\n', buff, 255);
    //Serial.println(buff);
    buff[bytesReceived] = '\0';

    startIdx=-1;
    for (int i=0; i<bytesReceived; i++) {
        if (buff[i] == '<') {
            startIdx=i;
        }
    }
    if (startIdx == -1) {
        newDatagram = false;
        return;
    }
    newDatagram=true;
    if (startIdx > 0) {
        memmove(buff, buff+startIdx, 256-startIdx);
    }
    if (startIdx >0){ 
//        Serial.print("startIdx:");
//        Serial.print(startIdx);
//        Serial.print(" buffer: ");
//        Serial.println(buff);
    }
}

void predictivelaptiming(){  ///delta
     
     u8g2.clearBuffer();
     u8g2.setFont(impact20);
     u8g2.drawStr(7, 38, "PREDICTIVE LAP TIMING");
     u8g2.sendBuffer();   
     delay(2000);
     s1=88;
}


void livespeed(){ 

     u8g2.clearBuffer();
     u8g2.setFont(impact20);
     u8g2.drawStr(98, 33, "SPEED");
     u8g2.setFont(impact14);
     u8g2.drawStr(58, 56, "PRESS Q TO RESET");
     u8g2.sendBuffer();   
     delay(2000);
     s2=88;
}

void laptimingtext(){ //Laptime menu text

     u8g2.clearBuffer();
     u8g2.setFont(impact20);
     u8g2.drawStr(70, 38, "LAP TIMING");
     u8g2.sendBuffer();   
     delay(2000);
     s3=88;
}

void lapcounttext(){
     u8g2.clearBuffer();
     u8g2.setFont(impact20);
     u8g2.drawStr(70, 38, "LAP COUNT");
     u8g2.sendBuffer();   
     delay(2000);
     s4=88;
}

  void Lapcount (){

  currentlapcount = String(currentlap);
  completedlapscount = String(completedlaps);
 
    u8g2.clearBuffer();
    u8g2.setFont(impact14);
    u8g2.drawStr(23, 19, "TOTAL LAPS");
    u8g2.drawStr(147, 19, "CURRENT LAP");
    u8g2.setFont(impact36laptime);
    u8g2.drawStr(27, 64, completedlapscount.c_str());
    u8g2.setFont(impact36laptime);
    u8g2.drawStr(156, 64, currentlapcount.c_str());
    u8g2.drawHLine(0,22,255);
    u8g2.drawVLine(129,0,64);
    u8g2.sendBuffer(); 

}

  void lateralgtext(){

     u8g2.clearBuffer();
     u8g2.setFont(impact20);
     u8g2.drawStr(75, 33, "LATERAL G");
     u8g2.setFont(impact14);
     u8g2.drawStr(58, 56, "PRESS Q TO RESET");
     u8g2.sendBuffer();   
     delay(2000);
     s5=88;
}


  void Lateralg (){

  lateralgcount = String(lateralg);
  
    u8g2.clearBuffer();
    u8g2.setFont(impact42laptime);
    u8g2.drawStr(20, 57, lateralgcount.c_str());//-----------------------------
    u8g2.drawStr(140, 58, "g");
    u8g2.setFont(ArialNB18);
    u8g2.drawStr(205, 25, "Max");
    u8g2.drawHLine(215,34,20);  //Horizontal line (start, height, distance)
    u8g2.sendBuffer(); 
    
}

void longitudinalgtext(){

     u8g2.clearBuffer();
     u8g2.setFont(impact20);
     u8g2.drawStr(45, 33, "LONGITUDINAL G");
     u8g2.setFont(impact14);
     u8g2.drawStr(58, 56, "PRESS Q TO RESET");
     u8g2.sendBuffer();   
     delay(2000);
     s6=88;
}

void Longitudinalg (){

  longitudinalgcount = String(longitudinalg);
  
    u8g2.clearBuffer();
    u8g2.setFont(impact42laptime);
    u8g2.drawStr(20, 57, longitudinalgcount.c_str());//-----------------------------
    u8g2.drawStr(140, 58, "g");
    u8g2.setFont(ArialNB18);
    u8g2.drawStr(205, 25, "Max");
    u8g2.drawVLine(190,25,25);
    u8g2.sendBuffer(); 
    
}
void parseDatagram() {
    char* idx;
    idx = strtok(buff, "\t");
    if (idx != NULL) strcpy(speed, idx+1);
    idx = strtok(NULL, "\t");
    if (idx != NULL) strcpy(maxSpeed, idx);
    idx = strtok(NULL, "\t");
    if (idx != NULL) strcpy(laptime, idx);
    idx = strtok(NULL, "\t");
    if (idx != NULL) strcpy(currentlap, idx);
    idx = strtok(NULL, "\t");
    if (idx != NULL) strcpy(completedlaps, idx);
    idx = strtok(NULL, "\t");
    if (idx != NULL) strcpy(lateralg, idx);
    idx = strtok(NULL, "\t");
    if (idx != NULL) strcpy(longitudinalg, idx);
    idx = strtok(NULL, "\t");
    if (idx != NULL) strcpy(liveDelta, idx);
    idx = strtok(NULL, "\t");
    if (idx != NULL) strcpy(liveDeltaProgress, idx);
    
    liveDeltaProgressFloat = atof(liveDeltaProgress);
        
    // convertir el delta a float redondearlo y volverlo a char[]
    liveDeltaFloat = rnd((float)atof(liveDelta));
    sprintf(liveDelta, "%.2f", liveDeltaFloat);

    // agregar + al delta si es positivo
    if (liveDelta[0] != '-') {
        memmove(liveDelta+1, liveDelta, 9);
        liveDelta[0] = '+';
    }
    
    // extraer el entero
    idx = strtok(liveDelta, ".");
    strcpy(liveDeltaL, idx);

    // extraer el decimal
    idx = strtok(NULL, ".");
    strcpy(liveDeltaR, idx);
    liveDeltaR[2] = 0;

    // reconstruir liveDelta que fue destruido por strtok
    sprintf(liveDelta, "%2.f", liveDeltaFloat);

    if (startIdx<=0) return;
//    Serial.print("\t|speed:");
//    Serial.print(speed);
//    Serial.print(" maxSpeed:");
//    Serial.print(maxSpeed);
//    Serial.print(" laptime:");
//    Serial.print(laptime);
//    Serial.print(" liveDelta:");
//    Serial.print(liveDelta);
//    Serial.print(" liveDeltaProgress:");
//    Serial.print(liveDeltaProgress);
//    Serial.println();
}
