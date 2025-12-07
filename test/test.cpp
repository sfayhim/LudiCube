#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// RFID
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Bouton assistant
#define BUTTON_ASSISTANT 8
bool attenteTheme = false;

// Gestion des thèmes
enum Theme { THEME_NONE, THEME_SPORT, THEME_CULTURE, THEME_SANTE };
Theme currentTheme = THEME_NONE;

// Messages
const char* msgSport[] = {
  "As-tu bouge aujourd’hui ?",
  "Faire du sport donne de l’energie !",
  "Bouger c’est bon pour ton corps !",
  "As-tu fait un etirement aujourd’hui ?"
};

const char* msgCulture[] = {
  "Quel est le plus grand ocean du monde ?",
  "Pourquoi le ciel est bleu ?",
  "Quel animal vit la nuit ?",
  "Ou se trouve la pyramide de Kheops ?"
};

const char* msgSante[] = {
  "As-tu bu de l’eau aujourd’hui ?",
  "Bravo ! Prends soin de toi.",
  "As-tu bien dormi cette nuit ?",
  "Pense a laver tes mains avant de manger !"
};

// Compteurs
const int msgSportCount   = sizeof(msgSport) / sizeof(msgSport[0]);
const int msgCultureCount = sizeof(msgCulture) / sizeof(msgCulture[0]);
const int msgSanteCount   = sizeof(msgSante) / sizeof(msgSante[0]);

unsigned long lastBadgeTime = 0;

// PROTOTYPES
void scanBadge();
void direPhraseTheme();

void setup() {
  Serial.begin(115200);

  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BUTTON_ASSISTANT, INPUT_PULLUP);

  randomSeed(analogRead(A0));

  Serial.println("Assistant prêt.");
}

void loop() {

  // Bouton assistant
  if (digitalRead(BUTTON_ASSISTANT) == LOW) {
    delay(150);
    Serial.println("Quel theme voulez-vous ?");
    attenteTheme = true;
  }

  if (attenteTheme) {
    scanBadge();
  }
}

void scanBadge() {

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  if (millis() - lastBadgeTime < 1500) return;
  lastBadgeTime = millis();

  // Construire l'UID
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  Serial.print("Badge scanne : ");
  Serial.println(uid);

  // Badge SPORT
  if (uid == "6590FB75") {
    currentTheme = THEME_SPORT;
    direPhraseTheme();
  }
  // Badge CULTURE
  else if (uid == "1A6CCF9A") {
    currentTheme = THEME_CULTURE;
    direPhraseTheme();
  }
  // Badge SANTE
  else if (uid == "1A5A349A") {
    currentTheme = THEME_SANTE;
    direPhraseTheme();
  }
  else {
    Serial.println("Badge inconnu.");
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  attenteTheme = false;
}

void direPhraseTheme() {
  int index = 0;

  switch (currentTheme) {

    case THEME_SPORT:
      index = random(msgSportCount);
      Serial.println(msgSport[index]);
      break;

    case THEME_CULTURE:
      index = random(msgCultureCount);
      Serial.println(msgCulture[index]);
      break;

    case THEME_SANTE:
      index = random(msgSanteCount);
      Serial.println(msgSante[index]);
      break;

    default:
      Serial.println("Aucun theme choisi.");
      break;
  }
}
