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

Serial.println("Bien joué");
return true; // Réponse correcte

} else {

Serial.println("Essaie encore");
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
  // Serial.println(uidString);

  // 🔹 Gestion des badges
  if (uidString == "ABB28950") {
    Serial.println("Mode facile Activé");
    isEasy = true;
    gameStarted = true;
    gameFinished = false;
  }
  else if (uidString == "043F0C5E") {
    Serial.println("Mode difficile Activé");
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
Serial.println("Lecteur prêt !");
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
  if (isEasy) {
    Serial.println("MODE FACILE ACTIVé");
    Serial.println("Appuie sur un bouton pour voir l'organe correspondant.");
  } else {
    Serial.println("MODE DIFFICILE ACTIVé");
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



if (buttonState1 == LOW) {
  Serial.println("Les intestins : ils aident à digérer la nourriture");
  delay(500);
}

if (buttonState2 == LOW) {
  Serial.println("Le coeur : il fait circuler le sang");
  delay(500);
}

if (buttonState3 == LOW) {
  Serial.println("Le cerveau : il commande le corps");
  delay(500);
}

if (buttonState4 == LOW) {
  Serial.println("Les poumons : ils servent à respirer");
  delay(500);
}


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
Serial.println("Quel organe correspond à : ");
// send organ to find
if (targetOrgan == 1) Serial.println("Les INTESTINS");
if (targetOrgan == 2) Serial.println("Le Coeur");
if (targetOrgan == 3) Serial.println("Le Cerveau");
if (targetOrgan == 4) Serial.println("Les Poumons");

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
Serial.println("Fin du jeu !");

gameFinished = true; // Marquer le jeu comme terminé, permettre un nouveau badge

}

bool isCompleted() {
  return gameFinished;
}

} 