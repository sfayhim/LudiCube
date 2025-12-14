#include "../include/jeux/memoire.h"

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define RESOLUTION 10
#define MAXVALUE (1 << RESOLUTION)
#define PRECISION 10
#define ADPIN A0

namespace Memoire {

// === LEDs ===
int leds[] = {A1, A2, A3, A4, 6, 7, A5};
int ledCount = sizeof(leds) / sizeof(leds[0]);

// === Boutons ===
uint32_t ADKeyVal[10] = {0};
uint32_t ADCKeyIn = 0;

// Pattern de jeu
const int PATTERN_LENGTH = 5;   // longueur du pattern aléatoire
const int ROUNDS_TO_WIN = 1;     // nombre de rounds pour terminer
int pattern[PATTERN_LENGTH] = {0};
int playerIndex = 0;
int roundsCompleted = 0;
bool gameCompleted = false;
bool waitingForRelease = true;  // attendre que les boutons soient relâchés
bool messageShown = false;      // pour afficher "À vous de jouer!" une seule fois

// Prototypes
void ADKeybegin();
int8_t getKey();
void playPattern();
bool checkPlayer(int8_t key);
void lightLed(int ledIndex, int duration);

void begin() {
  // Serial déjà initialisé dans main.cpp
  
  // Réinitialiser l'état du jeu
  playerIndex = 0;
  roundsCompleted = 0;
  gameCompleted = false;
  waitingForRelease = true;
  messageShown = false;

  // Setup LEDs
  for (int i = 0; i < ledCount; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  // Setup boutons
  ADKeybegin();

  delay(500); // petit délai avant le début

  // Générer pattern aléatoire
  randomSeed(analogRead(A0));
  for (int i = 0; i < PATTERN_LENGTH; i++) {
    pattern[i] = random(0, ledCount); // valeurs 0..6
  }

  Serial.println("=== Pattern à reproduire ===");
  for (int i = 0; i < PATTERN_LENGTH; i++) {
    Serial.print(pattern[i] + 1); Serial.print(" ");
  }
  Serial.println();

  // Jouer le pattern initial
  playPattern();
}

void step() {
  ADCKeyIn = analogRead(ADPIN);
  int8_t key = getKey();
  
  // Attendre que tous les boutons soient relâchés avant de commencer
  if (waitingForRelease) {
    if (key == -1) {  // Aucun bouton appuyé
      waitingForRelease = false;
      if (!messageShown) {
        Serial.println("À vous de jouer!");
        messageShown = true;
      }
    }
    return;  // Ne rien faire tant qu'un bouton est encore enfoncé
  }

  if (key >= 0 && key < 7) { // Ignorer touches > 7
    Serial.print("Touche appuyée : "); Serial.println(key + 1);

    lightLed(key, 200); // feedback LED

    if (checkPlayer(key)) {
      playerIndex++;
      if (playerIndex >= PATTERN_LENGTH) {
        roundsCompleted++;
        Serial.print("Bravo! Pattern complet. Round ");
        Serial.print(roundsCompleted);
        Serial.print("/");
        Serial.println(ROUNDS_TO_WIN);
        
        if (roundsCompleted >= ROUNDS_TO_WIN) {
          Serial.println("=== VICTOIRE! Jeu Mémoire terminé! ===");
          gameCompleted = true;
          delay(2000);
          return; // Exit to allow game switch
        }
        
        delay(1000);
        playerIndex = 0;
        playPattern(); // rejouer pattern
      }
    } else {
      Serial.println("Erreur! Recommencez.");
      playerIndex = 0;
      delay(1000);
      playPattern(); // rejouer pattern
    }
  }
}

// === Fonctions ===

void ADKeybegin() {
  float RESValue[10] = {0, 3, 6.2, 9.1, 15, 24, 33, 51, 100, 220};
  for (uint8_t i = 0; i < 10; i++) {
    ADKeyVal[i] = RESValue[i] / (RESValue[i] + 22) * MAXVALUE;
  }
}

int8_t getKey() {
  for (uint8_t i = 0; i < 10; i++) {
    if (abs((int)ADCKeyIn - (int)ADKeyVal[i]) < PRECISION) {
      return i; // retourne 0..9
    }
  }
  return -1; // aucune touche
}

void playPattern() {
  Serial.println("Pattern :");
  for (int i = 0; i < PATTERN_LENGTH; i++) {
    lightLed(pattern[i], 400);
    delay(200);
  }
  Serial.println("À vous de jouer!");
}

bool checkPlayer(int8_t key) {
  return key == pattern[playerIndex];
}

void lightLed(int ledIndex, int duration) {
  digitalWrite(leds[ledIndex], HIGH);
  delay(duration);
  digitalWrite(leds[ledIndex], LOW);
}

bool isCompleted() {
  return gameCompleted;
}

} // namespace Memoire