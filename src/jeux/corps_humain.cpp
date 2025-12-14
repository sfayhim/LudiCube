#include "../include/jeux/corps_humain.h"
// --- Includes ---
#include <SPI.h>

#include <MFRC522.h>

// --- Pin Definitions ---
#define RST_PIN 9
#define SS_PIN 10
#define BUTTON_1_PIN 2

#define BUTTON_2_PIN 3

#define BUTTON_3_PIN 4

#define BUTTON_4_PIN 5

// --- RFID partagé (accessible par Assistant aussi) ---
MFRC522 mfrc522(SS_PIN, RST_PIN);

namespace CorpsHumain {

// --- Global Variables ---
int buttonState1 = 0;

int buttonState2 = 0;

int buttonState3 = 0;

int buttonState4 = 0;
bool isEasy = true;// set true for Easy mode, false for Hard mode
bool gameStarted = false;
bool gameFinished = false;
static unsigned long lastBadgeTime = 0;
int targetOrgan = 0;

// --- Function: checkAnswer ---
bool checkAnswer(int pressed) {

if (pressed == targetOrgan) {

Serial.println("MSG_CORRECT");
return true; // Réponse correcte

} else {

Serial.println("MSG_ESSAYER_ENCORE");
return false; // Réponse incorrecte, essayer encore

}

}

// --- Function: checkBadge ---
void checkBadge() {
  // Vérifie si un badge est présent
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Éviter de lire le même badge trop rapidement
  unsigned long currentTime = millis();
  if (currentTime - lastBadgeTime < 2000) {
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }
  lastBadgeTime = currentTime;

  // Récupère l'UID du badge (HEX sans espaces)
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidString.toUpperCase();

  Serial.print("Badge détecté : ");
  Serial.println(uidString);

  // 🔹 Gestion des badges
  if (uidString == "ABB28950") {
    Serial.println("MSG_BADGE_FACILE");
    isEasy = true;
    gameStarted = true;
    gameFinished = false;
  }
  else if (uidString == "043F0C5E") {
    Serial.println("MSG_BADGE_DIFFICILE");
    isEasy = false;
    gameStarted = true;
    gameFinished = false;
  }
  else {
    Serial.println("Badge inconnu ❌");
  }

  // On arrête la communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// --- Function: begin() ---
void begin() {

// Serial already initialized in main.cpp
SPI.begin();
mfrc522.PCD_Init();
Serial.println("Lecteur RFID prêt !");
Serial.println("Approche un badge pour choisir le mode (ou le jeu démarre en mode facile)...");

pinMode(BUTTON_1_PIN, INPUT_PULLUP);

pinMode(BUTTON_2_PIN, INPUT_PULLUP);

pinMode(BUTTON_3_PIN, INPUT_PULLUP);

pinMode(BUTTON_4_PIN, INPUT_PULLUP);
randomSeed(analogRead(A0));

// Start game automatically in easy mode
gameStarted = true;
isEasy = true;
gameFinished = false;

}

// --- Function: step() ---
void step() {
// Toujours vérifier les badges
checkBadge();

// Attendre qu'un badge de jeu soit scanné
if (!gameStarted) {
  delay(200);
  return;
}

// Afficher le message de mode une fois
static bool modeShown = false;
static bool lastModeEasy = true;

if (!modeShown || lastModeEasy != isEasy) {
  Serial.println("MSG_DEBUT");
  if (isEasy) {
    Serial.println("MSG_MODE_FACILE");
  } else {
    Serial.println("MSG_MODE_DIFFICILE");
  }
  modeShown = true;
  lastModeEasy = isEasy;
  gameFinished = false; // Reset pour le nouveau mode
}

if (isEasy) {

// Easy mode: show organ when button pressed

buttonState1 = digitalRead(BUTTON_1_PIN);

buttonState2 = digitalRead(BUTTON_2_PIN);

buttonState3 = digitalRead(BUTTON_3_PIN);

buttonState4 = digitalRead(BUTTON_4_PIN);

if (buttonState1 == LOW) { Serial.println("MSG_COEUR"); delay(500); }
if (buttonState2 == LOW) { Serial.println("MSG_POUMONS"); delay(500); }
if (buttonState3 == LOW) { Serial.println("MSG_FOIE"); delay(500); }
if (buttonState4 == LOW) { Serial.println("MSG_REIN"); delay(500); }

delay(100);
return;
}
// Hard mode: 3 unique questions

if (gameFinished) return; // Attendre un nouveau badge

bool asked[4] = {false, false, false, false};

int askedCount = 0;
while (askedCount < 3) {

// pick an organ not asked yet

do {

targetOrgan = random(1, 5);

} while (asked[targetOrgan - 1]);
asked[targetOrgan - 1] = true;
askedCount++;

// send organ to find
if (targetOrgan == 1) Serial.println("MSG_COEUR");
if (targetOrgan == 2) Serial.println("MSG_POUMONS");
if (targetOrgan == 3) Serial.println("MSG_FOIE");
if (targetOrgan == 4) Serial.println("MSG_REIN");

// wait for button press
bool answered = false;
while (!answered) {
  buttonState1 = digitalRead(BUTTON_1_PIN);
  buttonState2 = digitalRead(BUTTON_2_PIN);
  buttonState3 = digitalRead(BUTTON_3_PIN);
  buttonState4 = digitalRead(BUTTON_4_PIN);

  if (buttonState1 == LOW) { 
    answered = checkAnswer(1); 
    delay(700); // délai pour éviter les rebonds
  }
  if (buttonState2 == LOW) { 
    answered = checkAnswer(2); 
    delay(700); 
  }
  if (buttonState3 == LOW) { 
    answered = checkAnswer(3); 
    delay(700); 
  }
  if (buttonState4 == LOW) { 
    answered = checkAnswer(4); 
    delay(700); 
  }
}
}
Serial.println("MSG_FIN");

gameFinished = true; // Marquer le jeu comme terminé, permettre un nouveau badge

}



// #include <Arduino.h>

// // Nouvelle séquence : A1, A2, A3, A4, 6, 7, A5
// int leds[] = {A1, A2, A3, A4, 6, A5, 7};
// int count = sizeof(leds) / sizeof(leds[0]);

// void setup() {
//     for (int i = 0; i < count; i++) {
//         pinMode(leds[i], OUTPUT);
//         digitalWrite(leds[i], LOW);
//     }
// }

// void loop() {
//     for (int i = 0; i < count; i++) {
//         // sécurité : tout éteindre avant d’allumer
//         for (int j = 0; j < count; j++) {
//             digitalWrite(leds[j], LOW);
//         }

//         digitalWrite(leds[i], LOW);
//         delay(300);
//     }
// }

/*
// ============================================
// CODE DE TEST - NE PAS UTILISER
// ============================================
#include <SPI.h>
#include <MFRC522.h>
#define RESOLUTION 10
#define MAXVALUE (1 << RESOLUTION)
#define PRECISION 10
#define ADPIN A0

uint32_t ADKeyVal[10] = {0};
uint32_t ADCKeyIn = 0;

// Prototypes obligatoires pour PlatformIO
void ADKeybegin();
int8_t getKey();

void setup() {
  Serial.begin(115200);
  ADKeybegin();
}

void loop() {
  ADCKeyIn = analogRead(ADPIN);

  int8_t key = getKey();
  if (key != -1) {
    // Si la touche 0 est l’index 0 → afficher 0, sinon afficher le numéro
    Serial.print("Touche : ");
    Serial.println(key);  
  }

  delay(150);
}

void ADKeybegin() {
  float RESValue[10] = {0, 3, 6.2, 9.1, 15, 24, 33, 51, 100, 220};

  for (uint8_t i = 0; i < 10; i++) {
    ADKeyVal[i] = RESValue[i] / (RESValue[i] + 22) * MAXVALUE;
  }
}

int8_t getKey() {
  for (uint8_t i = 0; i < 10; i++) {
    if (abs((int)ADCKeyIn - (int)ADKeyVal[i]) < PRECISION) {
      return i;   // retourne 0..9
    }
  }
  return -1;     
}
*/

bool isCompleted() {
  return gameFinished;
}

} // namespace CorpsHumain