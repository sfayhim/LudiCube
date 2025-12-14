#include <Arduino.h>
#include "../include/jeux/memoire.h"
#include "../include/jeux/corps_humain.h"
#include "../include/jeux/assistant.h"

// Boutons configuration (same as memoire.cpp)
#define RESOLUTION 10
#define MAXVALUE (1 << RESOLUTION)
#define PRECISION 10
#define ADPIN A0

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
  Serial.println("===   LudiCube - Demarrage    ===");
  Serial.println("=================================");
  
  // Initialize button reading
  ADKeybegin();
  Serial.println("Boutons initialises");
  
  displayMenu();
  gameInitialized = true;
}

void loop() {
  if (!gameInitialized) return;
  
  // Read button state
  ADCKeyIn = analogRead(ADPIN);
  int8_t key = getKey();
  
  // Menu mode - wait for game selection
  if (currentGame == MENU) {
    if (key == 0) {  // Button 1 -> Assistant
      Serial.println("\n>>> SELECTION: Assistant <<<");
      currentGame = GAME_ASSISTANT;
      Assistant::begin();
      delay(500);
    }
    else if (key == 1) {  // Button 2 -> Memoire
      Serial.println("\n>>> SELECTION: Memoire <<<");
      currentGame = GAME_MEMOIRE;
      Memoire::begin();
      delay(500);
    }
    else if (key == 2) {  // Button 3 -> Corps Humain
      Serial.println("\n>>> SELECTION: Corps Humain <<<");
      currentGame = GAME_CORPS_HUMAIN;
      CorpsHumain::begin();
      delay(500);
    }
    return;
  }
  
  // Game mode
  switch (currentGame) {
    case GAME_MEMOIRE:
      Memoire::step();
      if (Memoire::isCompleted()) {
        Serial.println("\n=== Jeu termine ===");
        delay(2000);
        currentGame = MENU;
        displayMenu();
      }
      break;
      
    case GAME_CORPS_HUMAIN:
      CorpsHumain::step();
      if (CorpsHumain::isCompleted()) {
        Serial.println("\n=== Jeu termine ===");
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
  Serial.println("  Bouton 1 (1) -> Assistant");
  Serial.println("  Bouton 2 (2) -> Jeu Memoire");
  Serial.println("  Bouton 3 (3) -> Corps Humain");
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
