#include <Arduino.h>
#include "../include/jeux/memoire.h"
#include "../include/jeux/corps_humain.h"
#include "../include/jeux/assistant.h"

// ================= CONFIGURATION =================
#define RESOLUTION 10
#define MAXVALUE (1 << RESOLUTION)
#define PRECISION 10
#define ADPIN A0

// Boutons
const int boutonPin = 8;           // POWER / Assistant
#define BUTTON_2_PIN 3             // Poumons / Corps Humain

const unsigned long TEMPS_APPUI_LONG = 2000; // 2 secondes

// ================= ETAT SYSTEME =================
bool systemeAllume = true;
bool gameInitialized = false;

bool etatBoutonPrecedent = HIGH;
unsigned long tempsDebutAppui = 0;

// ================= ADC BUTTONS =================
uint32_t ADKeyVal[10] = {0};
uint32_t ADCKeyIn = 0;

// ================= GAME STATE =================
enum GameState {
  MENU,
  GAME_MEMOIRE,
  GAME_CORPS_HUMAIN,
  GAME_ASSISTANT
};

GameState currentGame = MENU;

// ================= DECLARATIONS =================
void ADKeybegin();
int8_t getKey();
void displayMenu();

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(3000);

  pinMode(boutonPin, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);

  Serial.println();
  Serial.println("Bienvenu sur LudiCube");
  ADKeybegin();

  displayMenu();
  gameInitialized = true;
}

// ================= LOOP =================
void loop() {
  if (!gameInitialized) return;

  // ===== Bouton pin 8 : appui court / long =====
  bool etatBouton = digitalRead(boutonPin);

  if (etatBoutonPrecedent == HIGH && etatBouton == LOW) {
    tempsDebutAppui = millis();
  }

  if (etatBoutonPrecedent == LOW && etatBouton == HIGH) {
    unsigned long duree = millis() - tempsDebutAppui;

    if (duree >= TEMPS_APPUI_LONG) {
      systemeAllume = !systemeAllume;

      Serial.println();
      Serial.println("=================================");
      Serial.print("=== Le CUBE est ");
      Serial.print(systemeAllume ? "ALLUMé" : "ETEINT");
      Serial.println(" ===");
      Serial.println("=================================");

      if (systemeAllume) {
        currentGame = MENU;
        displayMenu();
      }

      etatBoutonPrecedent = etatBouton;
      delay(300);
      return;
    }
  }

  etatBoutonPrecedent = etatBouton;

  if (!systemeAllume) return;

  // ===== Lecture ADC =====
  ADCKeyIn = analogRead(ADPIN);
  int8_t key = getKey();

  // ================= MENU =================
  if (currentGame == MENU) {

    // Assistant (appui court pin 8)
    if (etatBouton == LOW && (millis() - tempsDebutAppui) < TEMPS_APPUI_LONG) {
      Serial.println("\nAssistant lancé...");
      currentGame = GAME_ASSISTANT;
      Assistant::begin();
      delay(500);
    }

    // Mémoire (ADC)
    else if (key == 0) {
      Serial.println("\nJeu mémoire lancé...");
      currentGame = GAME_MEMOIRE;
      Memoire::begin();
      delay(500);
    }

    // Corps Humain (bouton pin 3)
    else if (digitalRead(BUTTON_2_PIN) == LOW) {
      Serial.println("\nJeu corps humain lancé...");
      currentGame = GAME_CORPS_HUMAIN;
      CorpsHumain::begin();
      delay(500);
    }

    return;
  }

  // ================= JEUX =================
  switch (currentGame) {

    case GAME_MEMOIRE:
      Memoire::step();
      if (Memoire::isCompleted()) {
        Serial.println("\nBravo, jeu terminé");
        delay(2000);
        currentGame = MENU;
        displayMenu();
      }
      break;

    case GAME_CORPS_HUMAIN:
      CorpsHumain::step();
      if (CorpsHumain::isCompleted()) {
        Serial.println("\nBravo, jeu terminé");
        delay(2000);
        currentGame = MENU;
        displayMenu();
      }
      break;

    case GAME_ASSISTANT:
      Assistant::step();
      if (Assistant::isCompleted()) {
        Serial.println("\nRetour au menu");
        delay(2000);
        currentGame = MENU;
        displayMenu();
      }
      break;

    default:
      break;
  }
}

// ================= MENU =================
void displayMenu() {
  Serial.println("Quel jeu veu-tu ?");
  Serial.println("Assistant");
  Serial.println("Jeu Mémoire");
  Serial.println("Corps Humain");

  Serial.println("je attents...");
}

// ================= ADC =================
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
