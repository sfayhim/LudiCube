#include <Arduino.h>
#include "../include/jeux/memoire.h"
#include "../include/jeux/corps_humain.h"
#include "../include/jeux/assistant.h"


// Boutons configuration
#define RESOLUTION 10
#define MAXVALUE (1 << RESOLUTION)
#define PRECISION 10
#define ADPIN A0
#define BUTTON_8_PIN 8  // Bouton dédié à l'assistant
#define BUTTON_2_PIN 2 // Bouton dédié au corps humain

uint32_t ADKeyVal[10] = {0};
uint32_t ADCKeyIn = 0;

// Game state
enum GameState {
  MENU,
  GAME_MEMOIRE,
  GAME_CORPS_HUMAIN,
  GAME_ASSISTANT
};

GameState currentGame = MENU;
bool gameInitialized = false;

// Forward declarations
void ADKeybegin();
int8_t getKey();
void displayMenu();

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println();
  Serial.println("=================================");
  Serial.println("===   LudiCube - Démarrage    ===");
  Serial.println("=================================");
  
  // Initialisation boutons
  ADKeybegin();
  pinMode(BUTTON_8_PIN, INPUT_PULLUP); // Assistant
  pinMode(BUTTON_2_PIN, INPUT_PULLUP); // Corps Humain
  // Les autres boutons sont initialisés dans leurs modules respectifs
  Serial.println("Boutons initialisés");

  displayMenu();
  gameInitialized = true;
}

void loop() {
  if (!gameInitialized) return;
  
  // Lecture boutons
  ADCKeyIn = analogRead(ADPIN);
  int8_t key = getKey();

  // Menu : attendre sélection jeu
  if (currentGame == MENU) {
    // Assistant : bouton pin 8
    if (digitalRead(BUTTON_8_PIN) == LOW) {
      Serial.println("\n>>> SELECTION: Assistant <<<");
      currentGame = GAME_ASSISTANT;
      Assistant::begin();
      delay(500);
      return;
    }
    // Mémoire : bouton 0 du pavé (A0)
    if (key == 0) {
      Serial.println("\n>>> SELECTION: Mémoire <<<");
      currentGame = GAME_MEMOIRE;
      Memoire::begin();
      delay(500);
      return;
    }
    // Corps humain : bouton du coeur (BUTTON_1_PIN)
    if (digitalRead(BUTTON_8_PIN) == LOW) { // 2 = BUTTON_1_PIN
      Serial.println("\n>>> SELECTION: Corps Humain <<<");
      currentGame = GAME_CORPS_HUMAIN;
      CorpsHumain::begin();
      delay(500);
      return;
    }
    return;
  }
  
  // Game mode
  switch (currentGame) {
    case GAME_MEMOIRE:
      Memoire::step();
      if (Memoire::isCompleted()) {
        Serial.println("\n=== Jeu terminé ===");
        delay(2000);
        currentGame = MENU;
        displayMenu();
      }
      break;
      
    case GAME_CORPS_HUMAIN:
      CorpsHumain::step();
      if (CorpsHumain::isCompleted()) {
        Serial.println("\n=== Jeu terminé ===");
        delay(2000);
        currentGame = MENU;
        displayMenu();
      }
      break;

    case GAME_ASSISTANT:
      Assistant::step();
      if (Assistant::isCompleted()) {
        Serial.println("\n=== Retour au menu ===");
        delay(2000);
        currentGame = MENU;
        displayMenu();
      }
      break;

    default:
      break;
  }
}

void displayMenu() {
  Serial.println("\n================================");
  Serial.println("    MENU - Choisissez un jeu    ");
  Serial.println("================================");
  Serial.println("  Assistant");
  Serial.println("  Jeu Mémoire");
  Serial.println("  Corps Humain");
  Serial.println("================================");
  Serial.println("En attente...");
}

// Button reading functions
void ADKeybegin() {
  float RESValue[10] = {0, 3, 6.2, 9.1, 15, 24, 33, 51, 100, 220};
  for (uint8_t i = 0; i < 10; i++) {
    ADKeyVal[i] = RESValue[i] / (RESValue[i] + 22) * MAXVALUE;
  }
}

int8_t getKey() {
  for (uint8_t i = 0; i < 10; i++) {
    if (abs((int)ADCKeyIn - (int)ADKeyVal[i]) < PRECISION) {
      return i;
    }
  }
  return -1;
}
