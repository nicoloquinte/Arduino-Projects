#include <Sodaq_DS3231.h>
#include <U8glib.h>
#include <LowPower.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Time.h>

SoftwareSerial BTSerial(10, 11); // RX, TX
const int PGDW = 2; //précédent
const int PGUP = 3; //suivant
const int OK = 4; //OK
int Fselect = 1;
int TempC = 0;
int TempF = 0;


int DaySet = 0;
int MonthSet = 0;
int YearSet = 0;
int MinSet = 0;
int HourSet = 0;

int entermenu = -1;
int entermenu1 = -1;
int Selection = -1;

U8GLIB_SSD1306_128X64 u8g(
  U8G_I2C_OPT_DEV_0 |
  U8G_I2C_OPT_NO_ACK |
  U8G_I2C_OPT_FAST);


const unsigned char battery_empty1 [] U8G_PROGMEM = {
  0xFF, 0xFF, 0xE1, 0xE1, 0xE1, 0xE1, 0xE1, 0xE1, 0xE1, 0xE1, 0xF3, 0xFF
};
const unsigned char battery_empty2 [] U8G_PROGMEM = {
  0xFF, 0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x9C, 0xFF
};
const unsigned char battery_empty3 [] U8G_PROGMEM = {
  0xE0, 0xE0, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0xE0, 0xE0
};

const unsigned char deg [] U8G_PROGMEM = {
  0x0F, 0x3F, 0x7F , 0x70, 0xE0, 0xE0, 0xE0, 0xE0, 0x70, 0x7F, 0x3F, 0x0F
};
const unsigned char deg2 [] U8G_PROGMEM = {
  0x00, 0xC0, 0xE0, 0xE0, 0x70, 0x70, 0x70, 0x70, 0xE0, 0xE0, 0xC0, 0x00
};


const unsigned char warning[] U8G_PROGMEM = {
  0x18, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x18, 0x00, 0x18, 0x3c, 0x3c, 0x18
};

const unsigned char Thermometer[] U8G_PROGMEM = {
  0x10, 0x28, 0x28, 0x2C, 0x28, 0x38, 0x3C, 0x38, 0x38, 0x7C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38
};


const unsigned char  Clock1[] U8G_PROGMEM = {
  0x07, 0x1F, 0x3C, 0x70, 0x60, 0xE0, 0xCC, 0xC3, 0xC1, 0xC0, 0xE0, 0x60, 0x70, 0x3C, 0x1F, 0x07
};
const unsigned char  Clock2[] U8G_PROGMEM = {
  0xE0, 0xF8, 0x3C, 0x0E, 0x06, 0x27, 0x43, 0x83, 0x83, 0x03, 0x03, 0x06, 0x0E, 0x3C, 0xF8, 0xE0
};


const unsigned char  Gear1[] U8G_PROGMEM = {
  0x01, 0x33, 0x7B, 0x7F, 0x3F, 0x1E, 0x7C, 0xF8, 0xF8, 0x7C, 0x1E, 0x3F, 0x7F, 0x7B, 0x33, 0x01
};
const unsigned char  Gear2[] U8G_PROGMEM = {
  0x80, 0xCC, 0xDE, 0xFE, 0xFC, 0x78, 0x3E, 0x1F, 0x1F, 0x3E, 0x78, 0xFC, 0xFE, 0xDE, 0xCC, 0x80
};


const unsigned char  Min1[] U8G_PROGMEM = {
  0x01, 0x01, 0x01, 0x61, 0x60, 0xE0, 0xCC, 0xC3, 0xC1, 0xC0, 0xE0, 0x60, 0x70, 0x3C, 0x1F, 0x07
};
const unsigned char  Min2[] U8G_PROGMEM = {
  0xE0, 0xF8, 0xBC, 0x8E, 0x06, 0x07, 0x03, 0x83, 0x83, 0x03, 0x07, 0x06, 0x0E, 0x3C, 0xF8, 0xE0
};


const unsigned char  Chrono1[] U8G_PROGMEM = {
  0x07, 0x02, 0x4F, 0x3F, 0x70, 0x62, 0xE8, 0xC1, 0xD2, 0xC0, 0xE8, 0x62, 0x70, 0x3F, 0x0F
};
const unsigned char  Chrono2[] U8G_PROGMEM = {
  0x90, 0xe0, 0x70, 0x30, 0xB8, 0x18, 0x58, 0x18, 0xB8, 0x30, 0x70, 0xE0, 0x80
};

const uint8_t  Arrow_Select[] U8G_PROGMEM = {
  B00010000,
  B00011000,
  B00011100,
  B00011110,
  B00011111,
  B00011110,
  B00011100,
  B00011000,
  B00010000,
};


// chaîne pour date/heure formatée
char time_buffer[20];
char date_buffer[20];
char temp_buffer[20];
char time_SET_buffer [20];
char date_SET_buffer [20];
char year_SET_buffer [20];
int draw_state = 0;

void defaultinterface() {
  // joli cadre rond tout mimi
  u8g.drawRFrame(0, 16, 128, 48, 8);  // un cadre à coins arrondis  [arg : x, y, largeur, hauteur, rayon coins]
  u8g.drawBitmapP( 1, 1, 1, 12, battery_empty1);
  u8g.drawBitmapP( 9, 1, 1, 12, battery_empty2);
  u8g.drawBitmapP( 17, 1, 1, 12, battery_empty3);

  // u8g.drawBitmapP( 100, 1, 1, 16, bluetooth);
  // u8g.drawBitmapP( 50, 1, 1, 16, warning);

}

void drawtime() {
  u8g.drawBitmapP( 111, 0, 1, 16, Clock1);
  u8g.drawBitmapP( 119, 0, 1, 16, Clock2);
  DateTime now = rtc.now();
  u8g.setFont(u8g_font_profont29r);
  u8g.setFontPosTop(); // toujours après choix police
  u8g.drawStr(10, 20 , time_buffer);
  u8g.setFont(u8g_font_6x10);
  u8g.setFontPosTop();
  u8g.drawStr(20, 50 , date_buffer);
}

void drawtemp() {
  u8g.drawBitmapP( 115, 0, 1, 15, Thermometer);
  u8g.setFont(u8g_font_profont29r);
  u8g.setFontPosTop();
  if (Fselect == 1) {
    sprintf(temp_buffer, "%d",
            TempC);
    u8g.drawStr(75, 25, "C");
  }
  if (Fselect == 0) {
    sprintf(temp_buffer, "%d",
            TempF);
    u8g.drawStr(75, 25, "F");
  }
  u8g.drawStr(35, 25, temp_buffer);
  u8g.drawBitmapP( 91, 27, 1, 12, deg);
  u8g.drawBitmapP( 99, 27, 1, 12, deg2);

  u8g.setFont(u8g_font_6x10);
  u8g.setFontPosTop();
  u8g.drawStr(42, 3 , time_buffer);
}

void drawApp_chrono() {
  u8g.drawBitmapP( 111, 0, 1, 15, Chrono1);
  u8g.drawBitmapP( 119, 2, 1, 13, Chrono2);
  u8g.setFont(u8g_font_6x10);
  u8g.setFontPosTop();
  u8g.drawStr(42, 3 , time_buffer);
}
void drawApp_minuteur() {
  u8g.drawBitmapP( 111, 0, 1, 16, Min1);
  u8g.drawBitmapP( 119, 0, 1, 16, Min2);
  u8g.setFont(u8g_font_6x10);
  u8g.setFontPosTop();
  u8g.drawStr(42, 3 , time_buffer);
}
void drawparams() {
  u8g.drawBitmapP( 111, 0, 1, 16, Gear1);
  u8g.drawBitmapP( 119, 0, 1, 16, Gear2);
  u8g.setFont(u8g_font_6x10);
  u8g.setFontPosTop();
  u8g.drawStr(42, 3 , time_buffer);
  /*u8g.setFont(u8g_font_profont29r);
    u8g.setFontPosTop(); // toujours après choix police
    u8g.drawStr(10, 20  , date_SET_buffer );*/
  u8g.drawStr(20, 22, "Date");
  u8g.drawStr(20, 35, "Heure");
  u8g.drawStr(20, 48, "Format");
  u8g.drawStr(75, 24, "Alarme1");
  u8g.drawStr(75, 45, "Alarme2");
}

void drawparamsTIME() {
  u8g.drawBitmapP( 111, 0, 1, 16, Gear1);
  u8g.drawBitmapP( 119, 0, 1, 16, Gear2);
  u8g.setFont(u8g_font_6x10);
  u8g.setFontPosTop();
  u8g.drawStr(42, 3 , time_buffer);
  u8g.drawStr(50, 50  , "Heure" );
  u8g.setFont(u8g_font_profont29r);
  u8g.setFontPosTop(); // toujours après choix police
  u8g.drawStr(10, 20  , time_SET_buffer );
}
void drawparamsDATE() {
  u8g.drawBitmapP( 111, 0, 1, 16, Gear1);
  u8g.drawBitmapP( 119, 0, 1, 16, Gear2);
  u8g.setFont(u8g_font_6x10);
  u8g.setFontPosTop();
  u8g.drawStr(42, 3 , time_buffer);
  u8g.drawStr(54, 50  , "Date" );
  u8g.setFont(u8g_font_profont29r);
  u8g.setFontPosTop(); // toujours après choix police
  u8g.drawStr(26, 20  , date_SET_buffer );
}
void drawparamsYEAR() {
  u8g.drawBitmapP( 111, 0, 1, 16, Gear1);
  u8g.drawBitmapP( 119, 0, 1, 16, Gear2);
  u8g.setFont(u8g_font_6x10);
  u8g.setFontPosTop();
  u8g.drawStr(42, 3 , time_buffer);
  u8g.drawPixel(70, 51); //accent
  u8g.drawPixel(71, 50); //accent
  u8g.drawStr(50, 50  , "Annee" );
  u8g.setFont(u8g_font_profont29r);
  u8g.setFontPosTop(); // toujours après choix police
  u8g.drawStr(35, 20  , year_SET_buffer );
}

void formatSelect() {
  u8g.drawBitmapP( 111, 0, 1, 16, Gear1);
  u8g.drawBitmapP( 119, 0, 1, 16, Gear2);
  u8g.setFont(u8g_font_6x10);
  u8g.setFontPosTop();
  u8g.drawStr(42, 3 , time_buffer);
    u8g.drawStr(45, 50  , "Format" );
  u8g.setFont(u8g_font_profont29r);
  u8g.setFontPosTop(); // toujours après choix police
  u8g.drawStr(35, 20  , "F  C");
  
}
void SetTime() {
  //year, month, date, hour, min, sec and week-day(starts from 0 and goes to 6)
  DateTime dt(YearSet, MonthSet, DaySet, HourSet, MinSet, 0, 5);
  rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above
}
void draw(void) {
  defaultinterface();
  switch (draw_state) {
    case 0:
      drawtime();
      break;
    case 1:
      drawtemp();
      break;
    case 2:
      drawApp_chrono();
      break;
    case 3:
      drawApp_minuteur();
      break;
    case 4: //params
      switch (Selection) {
        case -1:
          drawparams();
          break;
        case 0:
          switch (entermenu1) {
            case -1:
              drawparams();
              u8g.drawBitmapP( 7, 22, 1, 9, Arrow_Select);//
              break;
            case 0:
              drawparamsDATE();
              u8g.drawRFrame(23, 20, 36, 27, 8);
              break;
            case 1:
              drawparamsDATE();
              u8g.drawRFrame(71, 20, 36, 27, 8);
              break;
            case 2:
              drawparamsYEAR();
              u8g.drawRFrame(31, 20, 69, 27, 8);// un cadre à coins arrondis  [arg : x, y, largeur, hauteur, rayon coins]

              break;
          }
          break;
        case 1:
          switch (entermenu1) {
            case -1:
              drawparams();
              u8g.drawBitmapP( 7, 35, 1, 9, Arrow_Select);//
              break;
            case 0:
              drawparamsTIME();
              u8g.drawRFrame(23, 20, 36, 27, 8);
              break;
            case 1:
              drawparamsTIME();
              u8g.drawRFrame(71, 20, 36, 27, 8);
              break;
          }
          break;
        case 2:
          switch (entermenu1) {
            case -1:
              drawparams();
              u8g.drawBitmapP( 7, 47, 1, 9, Arrow_Select);//
              break;
            case 0:
              switch (Fselect) {
                case 0:
                  formatSelect();
                  u8g.drawRFrame(23, 20, 36, 27, 8);
                  break;
                case 1:
                  formatSelect();
                  u8g.drawRFrame(71, 20, 36, 27, 8);
                  break;
              }
              
          }
          break;
        case 3:
          drawparams();
          u8g.drawBitmapP( 62, 24, 1, 9, Arrow_Select);//
          break;
        case 4:
          drawparams();
          u8g.drawBitmapP( 62, 45, 1, 9, Arrow_Select);//
          break;
      }

      break;
  }

}

void imageInit() {

  // début de la boucle d'affichage
  u8g.firstPage();
  // boucle d'affichage d'U8glib
  do {
    // appel répété à notre fonction de dessin
    draw();
  } while ( u8g.nextPage() ); // fin de boucle

}
void setup(void) {

  Serial.begin(9600);
  BTSerial.begin(9600);
  Wire.begin();
  rtc.begin();
  // activation mesure
  Serial.println("Initialisation......");
  Serial.println("Port series/RTC ok");
  pinMode(2, OUTPUT);
  pinMode(OK, INPUT);
  for (int i = 2; i < 5 ; i++) {
    pinMode(i, OUTPUT);
  }
  /*   for (int i = 12; i < 17 ; i++) {
    pinMode(i, OUTPUT);
    }
    Serial.println("I/O ok");
  */

  Serial.println("Fin d'initialisation");
  digitalWrite(2, HIGH);
  delay(1500);
  digitalWrite(2, LOW);
  delay(200);
  for (int i = 0; i < 3; i++) {
    digitalWrite(2, HIGH);
    delay(200);
    digitalWrite(2, LOW);
    delay(200);
  }
  DateTime now = rtc.now();
  sprintf(date_buffer, "%02d/%02d/%d     %02d",
          now.date(),    // jour
          now.month(),   // mois
          now.year(),    // année
          now.second()); //seconde
  sprintf(time_buffer, " %02d:%02d",
          now.hour(),    // heures
          now.minute()); // minutes
}


void loop() {
  imageInit();

  rtc.convertTemperature();
  TempC = rtc.getTemperature();
  TempF = (TempC * 1.8) + 32.0; // Convert C to F

  DateTime now = rtc.now();

  HourSet = now.hour();
  MinSet = now.minute();
  DaySet = now.date();
  MonthSet = now.month();
  YearSet = now.year();

  sprintf(date_buffer, "%02d/%02d/%d     %02d",
          now.date(),    // jour
          now.month(),   // mois
          now.year(),    // année
          now.second()); //seconde
  sprintf(time_buffer, " %02d:%02d",
          now.hour(),    // heures
          now.minute()); // minutes


  if (digitalRead(PGDW) == HIGH) {
    draw_state --;
  }
  if (digitalRead(PGUP) == HIGH) {
    draw_state ++;
  }
  if (digitalRead(OK) == HIGH) {
    if (draw_state == 4) {
      entermenu = 0;
      Selection = 0;
      delay(400);
      while (entermenu == 0) {
        Serial.println("ah");
        if (digitalRead(PGDW) == HIGH) {
          Selection --;
          delay(100);
        }
        if (digitalRead(PGUP) == HIGH) {
          Selection ++;
          delay(100);
        }
        if (digitalRead(OK) == HIGH) {

          if (Selection == 2) {
            entermenu1 = 0;
            delay(400);

            while (entermenu1 == 0) {
              if (digitalRead(PGDW) == HIGH) {
                Fselect --;
                delay(100);
              }
              if (digitalRead(PGUP) == HIGH) {
                Fselect ++;
                delay(100);
              }
              if (Fselect > 1) {
                Fselect = 0;
              }
              if (Fselect < 0) {
                Fselect = 1;
              }
              imageInit();
              if (digitalRead(OK) == HIGH) {
                entermenu1 ++ ;
              }
            }
            delay(400);
            entermenu1 = -1;
          }
          if (Selection == 1) {
            entermenu1 = 0;
            delay(400);
            while (entermenu1 == 0) {

              sprintf(time_SET_buffer, " %02d:%02d",
                      HourSet,    // heures
                      MinSet); // minutes
              if (digitalRead(PGDW) == HIGH) {
                HourSet --;
                delay(100);
              }
              if (digitalRead(PGUP) == HIGH) {
                HourSet ++;
                delay(100);
              }
              if (digitalRead(5) == HIGH) {
                entermenu1 = 2;
              }
              if (HourSet > 23) {
                HourSet = 0;
              }
              if (HourSet < 0) {
                HourSet = 24;
              }
              imageInit();
              if (digitalRead(OK) == HIGH) {
                entermenu1 ++ ;
              }
            }
            delay(400);
            while (entermenu1 == 1) {

              sprintf(time_SET_buffer, " %02d:%02d",
                      HourSet,    // heures
                      MinSet); // minutes
              if (digitalRead(PGDW) == HIGH) {
                MinSet --;
                delay(100);
              }
              if (digitalRead(PGUP) == HIGH) {
                MinSet ++;
                delay(100);
              }
              if (digitalRead(5) == HIGH) {
                entermenu1 = 2;
              }

              if (MinSet > 59) {
                MinSet = 0;
              }
              if (MinSet < 0) {
                MinSet = 59;
              }
              imageInit();
              if (digitalRead(OK) == HIGH) {
                entermenu1 ++ ;
                SetTime();
              }
            }
            delay(400);
            entermenu1 = -1;
          }
          if (Selection == 0) {
            entermenu1 = 0;
            delay(400);
            while (entermenu1 == 0) {
              sprintf(date_SET_buffer, "%02d/%02d",
                      DaySet,    // jour
                      MonthSet);  //month
              if (digitalRead(PGDW) == HIGH) {
                DaySet --;
                delay(100);
              }
              if (digitalRead(PGUP) == HIGH) {
                DaySet ++;
                delay(100);
              }
              if (digitalRead(5) == HIGH) {
                entermenu1 = 3;
              }
              if (DaySet > 31) {
                DaySet = 0;
              }
              if (DaySet < 0) {
                DaySet = 31;
              }
              imageInit();
              if (digitalRead(OK) == HIGH) {
                entermenu1 ++ ;
              }
            }
            delay(400);
            while (entermenu1 == 1) {

              sprintf(date_SET_buffer, "%02d/%02d",
                      DaySet,    // jour
                      MonthSet);  //mois
              if (digitalRead(PGDW) == HIGH) {
                MonthSet --;
                delay(100);
              }
              if (digitalRead(PGUP) == HIGH) {
                MonthSet ++;
                delay(100);
              }
              if (digitalRead(5) == HIGH) {
                entermenu1 = 3;
              }

              if (MonthSet > 12) {
                MonthSet = 0;
              }
              if (MonthSet < 0) {
                MonthSet = 12;
              }
              imageInit();
              if (digitalRead(OK) == HIGH) {
                entermenu1 ++ ;
              }
            }
            delay(400);
            while (entermenu1 == 2) {

              sprintf(year_SET_buffer, "%d", YearSet);  //year
              if (digitalRead(PGDW) == HIGH) {
                YearSet --;
                delay(100);
              }
              if (digitalRead(PGUP) == HIGH) {
                YearSet ++;
                delay(100);
              }
              if (digitalRead(5) == HIGH) {
                entermenu1 = 2;
              }

              if (YearSet < 0) {
                YearSet = 2000;
              }
              imageInit();
              if (digitalRead(OK) == HIGH) {
                entermenu1 ++ ;
                SetTime();
              }
            }
            entermenu1 = -1;
          }
        }
        if (digitalRead(5) == HIGH) {
          entermenu = 5;
        }
        if (Selection > 4) {
          Selection = 0;
        }
        if (Selection < 0) {
          Selection = 4;
        }
        imageInit();
      }
    }
    entermenu = -1;
    Selection = -1;
    delay(400);

  }

  if (digitalRead(5) == HIGH) {
    draw_state = 0;
  }

  if (draw_state > 4) {
    draw_state = 0;
  }
  if (draw_state < 0) {
    draw_state = 4;
  }
}
