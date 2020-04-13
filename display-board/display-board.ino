#include <Wire.h>
#include <LiquidCrystal.h>

//Displayziffern
uint8_t temp_zehner;
uint8_t temp_einer;
uint8_t zeit_stunden;
uint8_t zeit_minuten_zehner;
uint8_t zeit_minuten_einer;
uint8_t drehzahl_tausender; //In der Original-Anzeige nur 0 oder 1
uint8_t drehzahl_hunderter;
uint8_t drehzahl_zehner;
uint8_t drehzahl_einer;

//Zusammengesetzte Werte
uint8_t temperatur;
uint8_t stunden;
uint8_t minuten;
uint16_t drehzahl;

//Symbole an Letzter Stelle eines Ziffernbytes
bool display_ml;
bool display_celsius;
bool display_trennstrich_links;
bool display_doppelpunkt_oben;
bool display_doppelpunkt_unten;
bool display_trennstrich_rechts;
bool display_tausend_oben;
bool display_tausend_unten;

//Hardware LEDs
bool led_start;
bool led_vorwaesche;
bool led_programme;
bool led_intensivspuelen;
bool led_auflockern;

//Symbole aus 0x0A
bool display_pumpe_reinigen;
bool display_service;
bool display_vorwaesche;
bool display_hauptwaesche;
bool display_spuelen;
bool display_weichspuelen;
bool display_schleudern;
bool display_abpumpen;

//Symbole aus 0x0B
bool display_wasserhahn_zu;
bool display_tastensperre;
bool display_tuer_offen;
bool display_zeitvorwahl;
bool display_steinschuessel;

//Symbole aus 0x0C
bool display_shirt_1;
bool display_shirt_2;
bool display_shirt_3;
bool display_kanne_ml;
bool display_flasche_0;
bool display_sense_blinken;

//Empfangsdatenbytes
byte receive_address; //Displayadresse
byte databyte0;
byte databyte1;
byte databyte2;
byte databyte3;
byte databyte4;
byte databyte5;
byte databyte6;
byte databyte7;
byte databyte8;
byte databyte9;
byte databyte10;
byte databyte11;
byte databyte12;

//Reihen und Spalten der Tastenmatrix definieren
#define row1 7
#define row2 8
#define col1 3
#define col2 4
#define col3 5
#define col4 6

//Byte zum Speichern der Tasterzustände
byte taster;

//LED-Pins definieren
#define pin_led_start A1
#define pin_led_vorwaesche A2
#define pin_led_programme A3
#define pin_led_intensivspuelen 1
#define pin_led_auflockern 2

//LCD-Library initialisieren
const int rs = 13, en = A0, d4 = 12, d5 = 11, d6 = 10, d7 = 9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {

  //Ein- und Ausgänge für Tastaturmatrix setzen
  pinMode(row1, OUTPUT);
  pinMode(row2, OUTPUT);
  pinMode(col1, INPUT_PULLUP);
  pinMode(col2, INPUT_PULLUP);
  pinMode(col3, INPUT_PULLUP);
  pinMode(col4, INPUT_PULLUP);

  //Ausgänge auf 1 Setzten (Negative Logik)
  digitalWrite(row1, HIGH);
  digitalWrite(row2, HIGH);

  //LED-Pins als Ausgänge setzen
  pinMode(pin_led_start, OUTPUT);
  pinMode(pin_led_vorwaesche, OUTPUT);
  pinMode(pin_led_programme, OUTPUT);
  pinMode(pin_led_intensivspuelen, OUTPUT);
  pinMode(pin_led_auflockern, OUTPUT);

  //LCD initialisieren
  lcd.begin(16, 2);

  Wire.begin(0x3A);                // i2c-Bus mit der Adresse 0x3A beitreten
  Wire.onReceive(receiveEvent); //Funktion beim Empfangen von i2c-Daten
  Wire.onRequest(requestEvent); //Funktion beim Anfordern von i2c-Daten
  //Serial.begin(9600);           //Seriellen Port öffnen


}

void loop() {

  //  //Wenn beide Segmente der Tausenderstelle aktiv sind, ist der Einser aktiv
  //  if (display_tausend_oben && display_tausend_unten == 1) {
  //    drehzahl_tausender = 1;
  //  } else {
  //    drehzahl_tausender = 0;
  //  }
  //
  //  temperatur = temp_einer + temp_zehner * 10;
  //  stunden = zeit_stunden;
  //  minuten = zeit_minuten_einer + zeit_minuten_zehner * 10;
  //  drehzahl = drehzahl_einer + (drehzahl_zehner * 10) + (drehzahl_hunderter * 100) + (drehzahl_tausender * 1000);
  //
  //  Serial.print("temp");
  //  if (temperatur == 11) {
  //    Serial.print("-");
  //  } else {
  //    Serial.print(temperatur);
  //  }
  //
  //  Serial.print(";time");
  //  if (zeit_stunden == 10) {
  //    Serial.print("h");
  //  } else {
  //    Serial.print(zeit_stunden);
  //  }
  //
  //  Serial.print(":");
  //  Serial.print(minuten);
  //
  //  Serial.print(";rot");
  //  Serial.print(drehzahl);
  //
  //  Serial.print(";");


  //--------------------------Werte auf LCD schreiben-----------------------------

  //lcd.clear();
  //Temperaturwerte auf LCD schreiben
  lcd.setCursor(0, 1);
  lcd.print(temp_zehner);
  lcd.setCursor(1, 1);
  lcd.print(temp_einer);
  lcd.setCursor(2, 1);
  lcd.print(" C");

  //Zeitwerte auf LCD schreiben
  lcd.setCursor(6, 1);
  if (zeit_stunden == 10) //kleines h für Zielzeiteingabe
  {
    lcd.print("h");
  } else {
    lcd.print(zeit_stunden);
  }

  if (display_doppelpunkt_oben && display_doppelpunkt_unten == HIGH) //Abfragen, ob Doppelpunkt aktiv ist
  {
    lcd.print(":");
  }
  lcd.setCursor(8, 1);
  lcd.print(zeit_minuten_zehner);
  lcd.print(zeit_minuten_einer);

  //Drehzahlwerte auf LCD schreiben
  lcd.setCursor(12, 1);
  if (display_tausend_oben && display_tausend_unten == 1) //Abfragen, ob Tausenderstelle aktiv ist
  {
    lcd.print(1);
  }
  lcd.setCursor(13, 1);
  lcd.print(drehzahl_hunderter);
  lcd.print(drehzahl_zehner);
  lcd.print(drehzahl_einer);


  //--------------------------Tastaturmatrix abfragen-----------------------------

  //Reihe 1 abfragen
  digitalWrite(row1, LOW);
  bitWrite(taster, 0, !digitalRead(col1));  //Taster Start
  bitWrite(taster, 1, !digitalRead(col2));  //Taster Vorwäsche
  bitWrite(taster, 2, !digitalRead(col3));  //Taster Programme
  bitWrite(taster, 3, !digitalRead(col4));  //Taster Temperatur
  digitalWrite(row1, HIGH);
  //Reihe 2 abfragen
  digitalWrite(row2, LOW);
  bitWrite(taster, 4, !digitalRead(col1));  //Taster Fertig in
  bitWrite(taster, 5, !digitalRead(col2));  //Taster schleudern
  bitWrite(taster, 6, !digitalRead(col3));  //Taster Intensivspülen
  bitWrite(taster, 7, !digitalRead(col4));  //Taster Auflockern
  digitalWrite(row2, HIGH);

  //Serial.println(taster, BIN);


  //--------------------------LED-Ausgänge beschreiben-----------------------------

  //Über i2c empfangene Werte auf LED-Ausgänge schreiben
  digitalWrite(pin_led_start, led_start);
  digitalWrite(pin_led_vorwaesche, led_vorwaesche);
  digitalWrite(pin_led_programme, led_programme);
  digitalWrite(pin_led_intensivspuelen, led_intensivspuelen);
  digitalWrite(pin_led_auflockern, led_auflockern);

}

//Funktion, die beim anfordern von i2c-Daten aufgerufen wird
void requestEvent() {
  Wire.write(taster); //Byte 0 - Tasterwerte
  Wire.write(~taster); //Byte 1 - Tasterwerte Invertiert
}

//Funktion, die beim empfangen von i2c-Daten aufgerufen wird
void receiveEvent() {
  receive_address = Wire.read(); //Adresse auslesen
  if (Wire.available()) {       //Wenn noch daten vorhanden sind, daten auslesen
    databyte0 = Wire.read();
    databyte1 = Wire.read();
    databyte2 = Wire.read();
    databyte3 = Wire.read();
    databyte4 = Wire.read();
    databyte5 = Wire.read();
    databyte6 = Wire.read();
    databyte7 = Wire.read();
    databyte8 = Wire.read();
    databyte9 = Wire.read();
    databyte10 = Wire.read();
    databyte11 = Wire.read();
    databyte12 = Wire.read();
  }
  if (receive_address == 0x00) {

    //Temperatur Zehnerstelle
    temp_zehner = segment_to_int(databyte0);
    display_ml = bitRead(databyte0, 7);

    //Temperatur Einerstelle
    temp_einer = segment_to_int(databyte1);
    display_celsius = bitRead(databyte1, 7);

    //Stunden
    zeit_stunden = segment_to_int(databyte2);
    display_trennstrich_links = bitRead(databyte2, 7);

    //Minuten Zehnerstelle
    zeit_minuten_zehner = segment_to_int(databyte3);
    display_doppelpunkt_oben = bitRead(databyte3, 7);

    //Minuten Einerstelle
    zeit_minuten_einer = segment_to_int(databyte4);
    display_doppelpunkt_unten = bitRead(databyte4, 7);

    //Drehzahl Hunderterstelle
    drehzahl_hunderter = segment_to_int(databyte5);
    display_trennstrich_rechts = bitRead(databyte5, 7);

    //Drehzahl Zehnerstelle
    drehzahl_zehner = segment_to_int(databyte6);
    display_tausend_oben = bitRead(databyte6, 7);

    //Drehzahl Einerstelle
    drehzahl_einer = segment_to_int(databyte7);
    display_tausend_unten = bitRead(databyte7, 7);

    //LEDs Hardwaretasten dauerleuchten
    led_start = bitRead(databyte8, 0);
    led_vorwaesche = bitRead(databyte8, 1);
    led_programme = bitRead(databyte8, 2);
    led_intensivspuelen = bitRead(databyte8, 6);
    led_auflockern = bitRead(databyte8, 7);

    //LEDs Hardwaretasten 4x aufblinken (wird verwendet, falls Funktion momentan nicht verfügbar ist)

    //Symbole
    display_pumpe_reinigen = bitRead(databyte10, 0);
    display_service = bitRead(databyte10, 1);
    display_vorwaesche = bitRead(databyte10, 2);
    display_hauptwaesche = bitRead(databyte10, 3);
    display_spuelen = bitRead(databyte10, 4);
    display_weichspuelen = bitRead(databyte10, 5);
    display_schleudern = bitRead(databyte10, 6);
    display_abpumpen = bitRead(databyte10, 7);

    //Symbole
    display_wasserhahn_zu = bitRead(databyte11, 0);
    display_tastensperre = bitRead(databyte11, 1);
    display_tuer_offen = bitRead(databyte11, 2);
    display_zeitvorwahl = bitRead(databyte11, 3);
    display_steinschuessel = bitRead(databyte11, 5);

    //Symbole
    display_shirt_1 = bitRead(databyte12, 0);
    display_shirt_2 = bitRead(databyte12, 1);
    display_shirt_3 = bitRead(databyte12, 2);
    display_kanne_ml = bitRead(databyte12, 3);
    display_flasche_0 = bitRead(databyte12, 4);
    display_sense_blinken = bitRead(databyte12, 6);
  }

}

//Umwandeln von 7-Segment in INT
uint8_t segment_to_int(byte segments) {

  bitWrite(segments, 7, 0); //Zusatzbit mit 0 überschreiben
  switch (segments) {
    case B00111111: //Ziffer 0
      return 0;
      break;
    case B00000110: //Ziffer 1
      return 1;
      break;
    case B01011011: //Ziffer 2
      return 2;
      break;
    case B01001111: //Ziffer 3
      return 3;
      break;
    case B01100110: //Ziffer 4
      return 4;
      break;
    case B01101101: //Ziffer 5
      return 5;
      break;
    case B01111101: //Ziffer 6
      return 6;
      break;
    case B00000111: //Ziffer 7
      return 7;
      break;
    case B01111111: //Ziffer 8
      return 8;
      break;
    case B01101111: //Ziffer 9
      return 9;
      break;
    case B01110100: //kleines h
      return 10;
      break;
    default:
      return 0;
      break;
  }
}
