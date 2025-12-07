// // --- Includes ---
// #include <SPI.h>

// #include <MFRC522.h>

// // --- Pin Definitions ---
// #define RST_PIN 9
// #define SS_PIN 10
// #define BUTTON_1_PIN 2

// #define BUTTON_2_PIN 3

// #define BUTTON_3_PIN 4

// #define BUTTON_4_PIN 5

// // --- Global Variables ---
// MFRC522 mfrc522(SS_PIN, RST_PIN);
// int buttonState1 = 0;

// int buttonState2 = 0;

// int buttonState3 = 0;

// int buttonState4 = 0;
// bool isEasy = true;// set true for Easy mode, false for Hard mode
// bool gameStarted = false;
// bool gameFinished = false;
// static unsigned long lastBadgeTime = 0;
// int targetOrgan = 0;

// // --- Function: checkAnswer ---
// bool checkAnswer(int pressed) {

// if (pressed == targetOrgan) {

// Serial.println("MSG_CORRECT");
// return true; // Réponse correcte

// } else {

// Serial.println("MSG_ESSAYER_ENCORE");
// return false; // Réponse incorrecte, essayer encore

// }

// }

// // --- Function: checkBadge ---
// void checkBadge() {
//   // Vérifie si un badge est présent
//   if (!mfrc522.PICC_IsNewCardPresent()) {
//     return;
//   }
//   if (!mfrc522.PICC_ReadCardSerial()) {
//     return;
//   }

//   // Éviter de lire le même badge trop rapidement
//   unsigned long currentTime = millis();
//   if (currentTime - lastBadgeTime < 2000) {
//     mfrc522.PICC_HaltA();
//     mfrc522.PCD_StopCrypto1();
//     return;
//   }
//   lastBadgeTime = currentTime;

//   // Récupère l'UID du badge (HEX sans espaces)
//   String uidString = "";
//   for (byte i = 0; i < mfrc522.uid.size; i++) {
//     if (mfrc522.uid.uidByte[i] < 0x10) uidString += "0";
//     uidString += String(mfrc522.uid.uidByte[i], HEX);
//   }
//   uidString.toUpperCase();

//   Serial.print("Badge détecté : ");
//   Serial.println(uidString);

//   // 🔹 Gestion des badges
//   if (uidString == "ABB28950") {
//     Serial.println("MSG_BADGE_FACILE");
//     isEasy = true;
//     gameStarted = true;
//     gameFinished = false;
//   }
//   else if (uidString == "043F0C5E") {
//     Serial.println("MSG_BADGE_DIFFICILE");
//     isEasy = false;
//     gameStarted = true;
//     gameFinished = false;
//   }
//   else if (uidString == "1A6CCF9A") {
//     Serial.println("Bonjour Yasmina 😄 !");
//   } 
//   else if (uidString == "6590FB75") {
//     Serial.println("Salut Alex 👋 !");
//   } 
//   else {
//     Serial.println("Badge inconnu ❌");
//   }

//   // On arrête la communication
//   mfrc522.PICC_HaltA();
//   mfrc522.PCD_StopCrypto1();
// }

// // --- Function: setup() ---
// void setup() {

// Serial.begin(9600);
// while (!Serial);
// SPI.begin();
// mfrc522.PCD_Init();
// Serial.println("Lecteur RFID prêt !");
// Serial.println("Approche un badge...");

// pinMode(BUTTON_1_PIN, INPUT_PULLUP);

// pinMode(BUTTON_2_PIN, INPUT_PULLUP);

// pinMode(BUTTON_3_PIN, INPUT_PULLUP);

// pinMode(BUTTON_4_PIN, INPUT_PULLUP);
// randomSeed(analogRead(A0));

// }

// // --- Function: loop() ---
// void loop() {
// // Toujours vérifier les badges
// checkBadge();

// // Attendre qu'un badge de jeu soit scanné
// if (!gameStarted) {
//   delay(200);
//   return;
// }

// // Afficher le message de mode une fois
// static bool modeShown = false;
// static bool lastModeEasy = true;

// if (!modeShown || lastModeEasy != isEasy) {
//   Serial.println("MSG_DEBUT");
//   if (isEasy) {
//     Serial.println("MSG_MODE_FACILE");
//   } else {
//     Serial.println("MSG_MODE_DIFFICILE");
//   }
//   modeShown = true;
//   lastModeEasy = isEasy;
//   gameFinished = false; // Reset pour le nouveau mode
// }

// if (isEasy) {

// // Easy mode: show organ when button pressed

// buttonState1 = digitalRead(BUTTON_1_PIN);

// buttonState2 = digitalRead(BUTTON_2_PIN);

// buttonState3 = digitalRead(BUTTON_3_PIN);

// buttonState4 = digitalRead(BUTTON_4_PIN);

// if (buttonState1 == LOW) { Serial.println("MSG_COEUR"); delay(500); }
// if (buttonState2 == LOW) { Serial.println("MSG_POUMONS"); delay(500); }
// if (buttonState3 == LOW) { Serial.println("MSG_FOIE"); delay(500); }
// if (buttonState4 == LOW) { Serial.println("MSG_REIN"); delay(500); }

// delay(100);
// return;
// }
// // Hard mode: 3 unique questions

// if (gameFinished) return; // Attendre un nouveau badge

// bool asked[4] = {false, false, false, false};

// int askedCount = 0;
// while (askedCount < 3) {

// // pick an organ not asked yet

// do {

// targetOrgan = random(1, 5);

// } while (asked[targetOrgan - 1]);
// asked[targetOrgan - 1] = true;
// askedCount++;

// // send organ to find
// if (targetOrgan == 1) Serial.println("MSG_COEUR");
// if (targetOrgan == 2) Serial.println("MSG_POUMONS");
// if (targetOrgan == 3) Serial.println("MSG_FOIE");
// if (targetOrgan == 4) Serial.println("MSG_REIN");

// // wait for button press
// bool answered = false;
// while (!answered) {
//   buttonState1 = digitalRead(BUTTON_1_PIN);
//   buttonState2 = digitalRead(BUTTON_2_PIN);
//   buttonState3 = digitalRead(BUTTON_3_PIN);
//   buttonState4 = digitalRead(BUTTON_4_PIN);

//   if (buttonState1 == LOW) { 
//     answered = checkAnswer(1); 
//     delay(700); // délai pour éviter les rebonds
//   }
//   if (buttonState2 == LOW) { 
//     answered = checkAnswer(2); 
//     delay(700); 
//   }
//   if (buttonState3 == LOW) { 
//     answered = checkAnswer(3); 
//     delay(700); 
//   }
//   if (buttonState4 == LOW) { 
//     answered = checkAnswer(4); 
//     delay(700); 
//   }
// }
// }
// Serial.println("MSG_FIN");

// gameFinished = true; // Marquer le jeu comme terminé, permettre un nouveau badge

// }



// // #include <Arduino.h>

// // // Nouvelle séquence : A1, A2, A3, A4, 6, 7, A5
// // int leds[] = {A1, A2, A3, A4, 6, A5, 7};
// // int count = sizeof(leds) / sizeof(leds[0]);

// // void setup() {
// //     for (int i = 0; i < count; i++) {
// //         pinMode(leds[i], OUTPUT);
// //         digitalWrite(leds[i], LOW);
// //     }
// // }

// // void loop() {
// //     for (int i = 0; i < count; i++) {
// //         // sécurité : tout éteindre avant d’allumer
// //         for (int j = 0; j < count; j++) {
// //             digitalWrite(leds[j], LOW);
// //         }

// //         digitalWrite(leds[i], LOW);
// //         delay(300);
// //     }
// // }

// #include <SPI.h>
// #include <MFRC522.h>
// #define RESOLUTION 10
// #define MAXVALUE (1 << RESOLUTION)
// #define PRECISION 10
// #define ADPIN A0

// uint32_t ADKeyVal[10] = {0};
// uint32_t ADCKeyIn = 0;

// // Prototypes obligatoires pour PlatformIO
// void ADKeybegin();
// int8_t getKey();

// void setup() {
//   Serial.begin(115200);
//   ADKeybegin();
// }

// void loop() {
//   ADCKeyIn = analogRead(ADPIN);

//   int8_t key = getKey();
//   if (key != -1) {
//     // Si la touche 0 est l’index 0 → afficher 0, sinon afficher le numéro
//     Serial.print("Touche : ");
//     Serial.println(key);  
//   }

//   delay(150);
// }

// void ADKeybegin() {
//   float RESValue[10] = {0, 3, 6.2, 9.1, 15, 24, 33, 51, 100, 220};

//   for (uint8_t i = 0; i < 10; i++) {
//     ADKeyVal[i] = RESValue[i] / (RESValue[i] + 22) * MAXVALUE;
//   }
// }

// int8_t getKey() {
//   for (uint8_t i = 0; i < 10; i++) {
//     if (abs((int)ADCKeyIn - (int)ADKeyVal[i]) < PRECISION) {
//       return i;   // retourne 0..9
//     }
//   }
//   return -1;      // aucune touche
// }


// #include <Arduino.h>
// #include <SPI.h>
// #include <MFRC522.h>

// #define RESOLUTION 10
// #define MAXVALUE (1 << RESOLUTION)
// #define PRECISION 10
// #define ADPIN A0

// // === LEDs ===
// int leds[] = {A1, A2, A3, A4, 6, 7, A5};
// int ledCount = sizeof(leds) / sizeof(leds[0]);

// // === Boutons ===
// uint32_t ADKeyVal[10] = {0};
// uint32_t ADCKeyIn = 0;

// // Pattern de jeu
// const int PATTERN_LENGTH = 5;   // longueur du pattern aléatoire
// int pattern[PATTERN_LENGTH] = {0};
// int playerIndex = 0;

// // Prototypes
// void ADKeybegin();
// int8_t getKey();
// void playPattern();
// bool checkPlayer(int8_t key);
// void lightLed(int ledIndex, int duration);

// void setup() {
//   Serial.begin(115200);

//   // Setup LEDs
//   for (int i = 0; i < ledCount; i++) {
//     pinMode(leds[i], OUTPUT);
//     digitalWrite(leds[i], LOW);
//   }

//   // Setup boutons
//   ADKeybegin();

//   delay(500); // petit délai avant le début

//   // Générer pattern aléatoire
//   randomSeed(analogRead(A0));
//   for (int i = 0; i < PATTERN_LENGTH; i++) {
//     pattern[i] = random(0, ledCount); // valeurs 0..6
//   }

//   Serial.println("=== Pattern à reproduire ===");
//   for (int i = 0; i < PATTERN_LENGTH; i++) {
//     Serial.print(pattern[i] + 1); Serial.print(" ");
//   }
//   Serial.println();

//   // Jouer le pattern initial
//   playPattern();
// }

// void loop() {
//   ADCKeyIn = analogRead(ADPIN);
//   int8_t key = getKey();

//   if (key >= 0 && key < 7) { // Ignorer touches > 7
//     Serial.print("Touche appuyée : "); Serial.println(key + 1);

//     lightLed(key, 200); // feedback LED

//     if (checkPlayer(key)) {
//       playerIndex++;
//       if (playerIndex >= PATTERN_LENGTH) {
//         Serial.println("Bravo! Pattern complet.");
//         delay(1000);
//         playerIndex = 0;
//         playPattern(); // rejouer pattern
//       }
//     } else {
//       Serial.println("Erreur! Recommencez.");
//       playerIndex = 0;
//       delay(1000);
//       playPattern(); // rejouer pattern
//     }
//   }
// }

// // === Fonctions ===

// void ADKeybegin() {
//   float RESValue[10] = {0, 3, 6.2, 9.1, 15, 24, 33, 51, 100, 220};
//   for (uint8_t i = 0; i < 10; i++) {
//     ADKeyVal[i] = RESValue[i] / (RESValue[i] + 22) * MAXVALUE;
//   }
// }

// int8_t getKey() {
//   for (uint8_t i = 0; i < 10; i++) {
//     if (abs((int)ADCKeyIn - (int)ADKeyVal[i]) < PRECISION) {
//       return i; // retourne 0..9
//     }
//   }
//   return -1; // aucune touche
// }

// void playPattern() {
//   Serial.println("Pattern :");
//   for (int i = 0; i < PATTERN_LENGTH; i++) {
//     lightLed(pattern[i], 400);
//     delay(200);
//   }
//   Serial.println("À vous de jouer!");
// }

// bool checkPlayer(int8_t key) {
//   return key == pattern[playerIndex];
// }

// void lightLed(int ledIndex, int duration) {
//   digitalWrite(leds[ledIndex], HIGH);
//   delay(duration);
//   digitalWrite(leds[ledIndex], LOW);
// }

// #include <SPI.h>
// #include <MFRC522.h>

// // --- RFID ---
// #define SS_PIN 10
// #define RST_PIN 9
// MFRC522 mfrc522(SS_PIN, RST_PIN);

// // --- Bouton ---
// #define BUTTON_ASSISTANT 8

// // --- États ---
// bool attenteTheme = false;
// bool lectureActive = false;
// unsigned long lastPhraseTime = 0;

// // --- UID ---
// #define UID_SPORT   "6590FB75"
// #define UID_CULTURE "1A6CCF9A"
// #define UID_SANTE   "1A5A349A"

// // --- LISTES DE PHRASES ---
// const char* sportList[] = {
//   "As-tu bouge aujourd’hui ?",
//   "Bouger c’est bon pour ta sante !",
//   "As-tu fait un peu de sport aujourd’hui ?",
//   "Faire du sport donne de l’energie !"
// };

// const char* cultureList[] = {
//   "Quel est le plus grand ocean du monde ?",
//   "Pourquoi le ciel est bleu ?",
//   "Quel animal vit la nuit ?",
//   "Ou se trouve la tour Eiffel ?"
// };

// const char* santeList[] = {
//   "As-tu bu de l’eau aujourd’hui ?",
//   "Pense a te laver les mains !",
//   "Bravo, prends soin de toi.",
//   "As-tu bien dormi cette nuit ?"
// };

// int nbSport = sizeof(sportList) / sizeof(sportList[0]);
// int nbCulture = sizeof(cultureList) / sizeof(cultureList[0]);
// int nbSante = sizeof(santeList) / sizeof(santeList[0]);

// // --- Variables du theme ---
// const char** themeActuel = nullptr;
// int nbPhrases = 0;

// // --- FONCTIONS ---
// void lireBadge();
// void choisirTheme(const char* uid);
// void direPhrase(const char* theme[], int count);


// void setup() {
//   Serial.begin(115200);
//   SPI.begin();
//   mfrc522.PCD_Init();

//   pinMode(BUTTON_ASSISTANT, INPUT_PULLUP);

//   Serial.println("Assistant prêt.");
//   Serial.println("Appuyez sur le bouton pour choisir un theme.");
// }

// void loop() {

//   // --- Appui bouton ---
//   if (digitalRead(BUTTON_ASSISTANT) == LOW) {
//     delay(200);

//     // Si on lit en boucle → le bouton STOP
//     if (lectureActive) {
//       lectureActive = false;
//       Serial.println("Lecture interrompue. Appuyez pour recommencer.");
//       return;
//     }

//     // Sinon → on demande le thème
//     Serial.println("Quel theme voulez-vous ?");
//     attenteTheme = true;
//     themeActuel = nullptr;
//   }

//   // --- En attente du badge ---
//   if (attenteTheme && themeActuel == nullptr) {
//     lireBadge();
//   }

//   // --- Lecture en boucle (1 phrase toutes les 4 sec) ---
//   if (lectureActive && themeActuel != nullptr) {
//     if (millis() - lastPhraseTime > 4000) {   // 4 secondes
//       direPhrase(themeActuel, nbPhrases);
//       lastPhraseTime = millis();
//     }
//   }
// }


// // ---------- RFID ----------
// void lireBadge() {
//   if (!mfrc522.PICC_IsNewCardPresent()) return;
//   if (!mfrc522.PICC_ReadCardSerial()) return;

//   // Lire UID
//   String uid = "";
//   for (byte i = 0; i < mfrc522.uid.size; i++) {
//     if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
//     uid += String(mfrc522.uid.uidByte[i], HEX);
//   }
//   uid.toUpperCase();

//   Serial.print("Badge detecte : ");
//   Serial.println(uid);

//   choisirTheme(uid.c_str());

//   mfrc522.PICC_HaltA();
//   mfrc522.PCD_StopCrypto1();
// }


// // ---------- CHOIX THEME ----------
// void choisirTheme(const char* uid) {

//   if (strcmp(uid, UID_SPORT) == 0) {
//     Serial.println("Theme : SPORT");
//     themeActuel = sportList;
//     nbPhrases = nbSport;
//   }
//   else if (strcmp(uid, UID_CULTURE) == 0) {
//     Serial.println("Theme : CULTURE");
//     themeActuel = cultureList;
//     nbPhrases = nbCulture;
//   }
//   else if (strcmp(uid, UID_SANTE) == 0) {
//     Serial.println("Theme : SANTE");
//     themeActuel = santeList;
//     nbPhrases = nbSante;
//   }
//   else {
//     Serial.println("Badge inconnu.");
//     return;
//   }

//   // On démarre la boucle infinie
//   lectureActive = true;
//   attenteTheme = false;
//   lastPhraseTime = millis() - 4000;  // pour afficher immédiatement
// }


// // ---------- PHRASE ALEATOIRE ----------
// void direPhrase(const char* theme[], int count) {
//   int index = random(count);
//   Serial.print("Assistant : ");
//   Serial.println(theme[index]);
// }

/* 
  Cube ludique - Mode MENU (trois jeux dans un seul firmware)
  - Bouton 8 : Assistant (demande theme -> scanne badge theme -> phrases toutes les 4s; toggle stop avec bouton 8)
  - Bouton A0 : Demarrer jeu memoire (pattern leds -> entrer via pavé analogique A3)
  - Bouton 2 : Demarrer jeu organes (boutons 3..6 pour choix). Difficulté changeable en scannant badge (sans appuyer).
  - RFID partagé pour thèmes assistant + badges difficulté organes
  PIN MAPPING (UNO):
    RFID: SDA=10, RST=9, MOSI=11, MISO=12, SCK=13, VCC=3.3V, GND=GND
    Assistant bouton: 8
    Memo start button: A0 (digital read)
    Memo ADC keypad: A3
    Memo LEDs: A1, A2, A4, A5
    Organ start button: 2
    Organ choice buttons: 3,4,5,6
*/

/*
  Cube ludique - Mode MENU (corrigé)
  - Bouton 8 : Assistant (toggle)
  - Bouton A0 : Demarrer jeu memoire (digitalRead)
  - Bouton 2 : Demarrer jeu organes
  - RFID partagee pour assistant theme + badges difficulte organes
  PIN MAPPING (UNO):
    RFID: SDA=10, RST=9, MOSI=11, MISO=12, SCK=13, VCC=3.3V, GND=GND
    Assistant bouton: 8
    Memo start button: A0 (digital)
    Memo ADC keypad: A3
    Memo LEDs: A1, A2, A4, A5
    Organ start button: 2
    Organ choice buttons: 3,4,5,6
*/

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// ---------- RFID ----------
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long lastBadgeTime = 0;

// ---------- Buttons & Pins ----------
#define PIN_ASSISTANT_BTN 8     // bouton pour assistant (toggle)
#define PIN_MEMO_START A0       // bouton pour démarrer jeu mémoire (digitalRead)
#define PIN_MEMO_ADC A3         // entrée analogique du pavé numérique (lecture)
#define PIN_MEMO_LED_0 A1
#define PIN_MEMO_LED_1 A2
#define PIN_MEMO_LED_2 A4
#define PIN_MEMO_LED_3 A5

#define PIN_ORGAN_START 2       // bouton pour démarrer jeu organes
#define PIN_ORGAN_BTN1 3
#define PIN_ORGAN_BTN2 4
#define PIN_ORGAN_BTN3 5
#define PIN_ORGAN_BTN4 6

// ---------- UIDs (HEX sans espaces, majuscules) ----------
const char* UID_SPORT      = "6590FB75";
const char* UID_CULTURE    = "1A6CCF9A";
const char* UID_SANTE      = "1A5A349A";
const char* UID_BADGE_EASY = "ABB28950";
const char* UID_BADGE_HARD = "043F0C5E";

// ---------- Assistant (thèmes) ----------
enum Theme { THEME_NONE, THEME_SPORT, THEME_CULTURE, THEME_SANTE };
Theme assistantTheme = THEME_NONE;
bool assistantAwaitingTheme = false;
bool assistantRunning = false;
unsigned long assistantLastSpeak = 0;

const char* sportList[] = {
  "As-tu bouge aujourd'hui ?",
  "Bouger, c'est bon pour la sante !",
  "As-tu fait un peu de sport aujourd'hui ?",
  "Faire du sport donne de l'energie !"
};
const char* cultureList[] = {
  "Quel est le plus grand ocean du monde ?",
  "Pourquoi le ciel est bleu ?",
  "Quel animal vit la nuit ?",
  "Ou se trouve la tour Eiffel ?"
};
const char* santeList[] = {
  "As-tu bu de l'eau aujourd'hui ?",
  "Pense a te laver les mains !",
  "Bravo, prends soin de toi.",
  "As-tu bien dormi cette nuit ?"
};
const int sportCount = sizeof(sportList) / sizeof(sportList[0]);
const int cultureCount = sizeof(cultureList) / sizeof(cultureList[0]);
const int santeCount = sizeof(santeList) / sizeof(santeList[0]);

// ---------- Memo (Simon-like) ----------
bool memoRunning = false;
const int MEMO_LED_PINS[4] = { PIN_MEMO_LED_0, PIN_MEMO_LED_1, PIN_MEMO_LED_2, PIN_MEMO_LED_3 };
const int MEMO_PATTERN_LEN = 4;
int memoPattern[MEMO_PATTERN_LEN];
int memoIndex = 0;
bool memoPlayingPattern = false;
unsigned long memoLastAction = 0;
int memoStep = 0; // 0=idle,1=playing pattern,2=waiting input

// --- ADC keypad calibration ---
#define RESOLUTION 10
#define MAXVALUE (1 << RESOLUTION)
#define PRECISION 10
uint32_t ADKeyVal[10] = {0};
void ADKeybegin() {
  float RESValue[10] = {0, 3, 6.2, 9.1, 15, 24, 33, 51, 100, 220};
  for (uint8_t i = 0; i < 10; i++) {
    ADKeyVal[i] = RESValue[i] / (RESValue[i] + 22) * MAXVALUE;
  }
}

// Improved ADC key detection (returns candidate index or -1)
int8_t getKeyCandidate(int adcVal) {
  for (uint8_t i = 0; i < 10; i++) {
    if (abs((int)adcVal - (int)ADKeyVal[i]) < PRECISION) return i;
  }
  return -1;
}

// stable-key globals for memo
int lastStableKey = -1;
unsigned long keyChangedAt = 0;
const unsigned long KEY_STABLE_MS = 60;   // ms to consider stable
const unsigned long KEY_DEBOUNCE_DELAY = 200; // ms between confirmed keys

// ---------- Organes ----------
bool organRunning = false;
bool organEasy = true; // default easy
bool organGameFinished = false;
int organTarget = 0;

// ---------- MODE management ----------
enum Mode { MODE_IDLE, MODE_ASSISTANT, MODE_MEMO, MODE_ORGAN };
Mode currentMode = MODE_IDLE;

// debounce
const unsigned long BUTTON_DEBOUNCE_MS = 120;

// ---------- Prototypes ----------
void processBadgeUID(const char* uid);
String readUidStringFromRFID();
void assistantAskTheme();
void assistantStartLoop();
void assistantStopLoop();
void assistantMaybeSpeak();

void memoStart();
void memoPlayPattern();
void memoHandleInput();

void organStart();
void organLoop();
bool organCheckAnswer(int pressed);

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  // while (!Serial); // optional on UNO

  SPI.begin();
  mfrc522.PCD_Init();

  // buttons
  pinMode(PIN_ASSISTANT_BTN, INPUT_PULLUP);
  pinMode(PIN_MEMO_START, INPUT_PULLUP); // A0 used as digital input for start
  pinMode(PIN_ORGAN_START, INPUT_PULLUP);

  // organ choice buttons
  pinMode(PIN_ORGAN_BTN1, INPUT_PULLUP);
  pinMode(PIN_ORGAN_BTN2, INPUT_PULLUP);
  pinMode(PIN_ORGAN_BTN3, INPUT_PULLUP);
  pinMode(PIN_ORGAN_BTN4, INPUT_PULLUP);

  // memo LEDs
  for (int i = 0; i < 4; ++i) {
    pinMode(MEMO_LED_PINS[i], OUTPUT);
    digitalWrite(MEMO_LED_PINS[i], LOW);
  }

  ADKeybegin(); // calib pavé analogique
  randomSeed(analogRead(A5));

  Serial.println(F("SYSTEME: Pret. Boutons -> 8:Assistant, A0:Memo, 2:Organes"));
  Serial.println(F("RFID: Approchez un badge pour changement de mode/difficulte selon contexte."));
}

// ---------- LOOP ----------
void loop() {
  // RFID check (non-blocking)
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uidStr = readUidStringFromRFID();
    if (uidStr.length() > 0) {
      processBadgeUID(uidStr.c_str());
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }

  // Assistant button (toggle start/stop)
  if (digitalRead(PIN_ASSISTANT_BTN) == LOW) {
    delay(150); // debounce
    if (currentMode == MODE_ASSISTANT && assistantRunning) {
      assistantStopLoop();
      currentMode = MODE_IDLE;
      Serial.println(F("Assistant arrete. Retour menu."));
    } else {
      currentMode = MODE_ASSISTANT;
      assistantAskTheme();
    }
    delay(300);
  }

  // Memo start button (A0)
  if (digitalRead(PIN_MEMO_START) == LOW) {
    delay(150);
    if (currentMode == MODE_MEMO && memoRunning) {
      memoRunning = false;
      currentMode = MODE_IDLE;
      Serial.println(F("Memo interrompu. Retour menu."));
    } else {
      currentMode = MODE_MEMO;
      memoStart();
    }
    delay(300);
  }

  // Organ start button
  if (digitalRead(PIN_ORGAN_START) == LOW) {
    delay(150);
    if (currentMode == MODE_ORGAN && organRunning) {
      organRunning = false;
      currentMode = MODE_IDLE;
      Serial.println(F("Jeu organes interrompu. Retour menu."));
    } else {
      currentMode = MODE_ORGAN;
      organStart();
    }
    delay(300);
  }

  // Assistant running: speak loop
  if (currentMode == MODE_ASSISTANT && assistantRunning) {
    assistantMaybeSpeak();
  }

  // Memo running: pattern or input
  if (currentMode == MODE_MEMO && memoRunning) {
    if (memoStep == 1) memoPlayPattern();
    else if (memoStep == 2) memoHandleInput();
  }

  // Organ running: easy/hard logic
  if (currentMode == MODE_ORGAN && organRunning) {
    organLoop();
  }

  delay(10);
}

// ---------- RFID helper ----------
String readUidStringFromRFID() {
  if (millis() - lastBadgeTime < 1200) return "";
  lastBadgeTime = millis();

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  Serial.print(F("RFID: UID = "));
  Serial.println(uid);
  return uid;
}

// ---------- Process badges globally ----------
void processBadgeUID(const char* uid) {
  // Difficulty badges (global)
  if (strcmp(uid, UID_BADGE_EASY) == 0) {
    organEasy = true;
    Serial.println(F("RFID: Mode organes = FACILE (badge)"));
    if (currentMode == MODE_ORGAN && organRunning) organGameFinished = false;
    return;
  }
  if (strcmp(uid, UID_BADGE_HARD) == 0) {
    organEasy = false;
    Serial.println(F("RFID: Mode organes = DIFFICILE (badge)"));
    if (currentMode == MODE_ORGAN && organRunning) organGameFinished = false;
    return;
  }

  // Assistant theme badges (only if assistant awaiting)
  if (assistantAwaitingTheme) {
    if (strcmp(uid, UID_SPORT) == 0) {
      assistantTheme = THEME_SPORT;
      Serial.println(F("ASSISTANT: Theme = SPORT"));
    } else if (strcmp(uid, UID_CULTURE) == 0) {
      assistantTheme = THEME_CULTURE;
      Serial.println(F("ASSISTANT: Theme = CULTURE"));
    } else if (strcmp(uid, UID_SANTE) == 0) {
      assistantTheme = THEME_SANTE;
      Serial.println(F("ASSISTANT: Theme = SANTE"));
    } else {
      Serial.println(F("ASSISTANT: badge inconnu pour theme."));
      return;
    }
    assistantAwaitingTheme = false;
    assistantStartLoop();
    return;
  }

  Serial.println(F("RFID: badge sans action contextuelle."));
}

// ---------- ASSISTANT functions ----------
void assistantAskTheme() {
  Serial.println(F("ASSISTANT: Quel theme voulez-vous ? (scan badge)"));
  assistantAwaitingTheme = true;
  assistantTheme = THEME_NONE;
  assistantRunning = false;
  assistantLastSpeak = 0;
}

void assistantStartLoop() {
  if (assistantTheme == THEME_NONE) {
    Serial.println(F("ASSISTANT: Aucun theme choisi."));
    return;
  }
  assistantRunning = true;
  assistantLastSpeak = millis() - 4000; // immediate speak
  Serial.println(F("ASSISTANT: demarrage boucle phrases (bouton 8 pour arreter)."));
}

void assistantStopLoop() {
  assistantRunning = false;
  assistantAwaitingTheme = false;
  assistantTheme = THEME_NONE;
}

void assistantMaybeSpeak() {
  if (!assistantRunning) return;
  if (millis() - assistantLastSpeak >= 4000) {
    int idx = 0;
    switch (assistantTheme) {
      case THEME_SPORT:
        idx = random(sportCount);
        Serial.print(F("Assistant (SPORT): "));
        Serial.println(sportList[idx]);
        break;
      case THEME_CULTURE:
        idx = random(cultureCount);
        Serial.print(F("Assistant (CULTURE): "));
        Serial.println(cultureList[idx]);
        break;
      case THEME_SANTE:
        idx = random(santeCount);
        Serial.print(F("Assistant (SANTE): "));
        Serial.println(santeList[idx]);
        break;
      default:
        break;
    }
    assistantLastSpeak = millis();
  }
}

// ---------- MEMO functions ----------
void memoStart() {
  for (int i = 0; i < MEMO_PATTERN_LEN; ++i) memoPattern[i] = random(0, 4);
  memoIndex = 0;
  memoPlayingPattern = true;
  memoStep = 1;
  memoLastAction = millis() - 500;
  memoRunning = true;
  Serial.println(F("MEMO: demarrage. Reproduisez la sequence via le pavé (A3)."));
}

void memoPlayPattern() {
  static int playPos = 0;
  static bool ledOn = false;
  if (!memoPlayingPattern) return;

  unsigned long now = millis();
  if (!ledOn) {
    if (now - memoLastAction >= 200) {
      int ledPin = MEMO_LED_PINS[memoPattern[playPos]];
      digitalWrite(ledPin, HIGH);
      ledOn = true;
      memoLastAction = now;
    }
  } else {
    if (now - memoLastAction >= 400) {
      int ledPin = MEMO_LED_PINS[memoPattern[playPos]];
      digitalWrite(ledPin, LOW);
      ledOn = false;
      memoLastAction = now;
      playPos++;
      if (playPos >= MEMO_PATTERN_LEN) {
        memoPlayingPattern = false;
        memoStep = 2;
        memoIndex = 0;
        Serial.println(F("MEMO: A vous de jouer (entrez la sequence sur le pavé)."));
        playPos = 0;
      }
    }
  }
}

void memoHandleInput() {
  static int lastKey = -1;
  static unsigned long stableTime = 0;
  static unsigned long lastMemoKeyTime = 0;

  int adc = analogRead(PIN_MEMO_ADC);
  int8_t candidate = getKeyCandidate(adc);

  // candidate must be 0..3
  if (candidate < 0 || candidate > 3) {
    lastKey = -1;
    return;
  }

  if (candidate != lastKey) {
    lastKey = candidate;
    stableTime = millis();
    return;
  }

  if (millis() - stableTime < KEY_STABLE_MS) return;

  // prevent rapid repeats
  if (millis() - lastMemoKeyTime < KEY_DEBOUNCE_DELAY) return;
  lastMemoKeyTime = millis();

  int key = lastKey;
  Serial.print(F("MEMO: touche valide -> "));
  Serial.println(key);

  int ledPin = MEMO_LED_PINS[key];
  digitalWrite(ledPin, HIGH);
  delay(150);
  digitalWrite(ledPin, LOW);

  if (key == memoPattern[memoIndex]) {
    memoIndex++;
    if (memoIndex >= MEMO_PATTERN_LEN) {
      Serial.println(F("MEMO: Bravo ! Sequence correcte. Nouvelle sequence..."));
      delay(500);
      memoStart();
    }
  } else {
    Serial.println(F("MEMO: Faux ! Rejouez la sequence."));
    delay(500);
    memoPlayingPattern = true;
    memoStep = 1;
    memoLastAction = millis() - 500;
  }

  lastKey = -1; // reset to require fresh press
  delay(200);
}

// ---------- ORGANS functions ----------
void organStart() {
  organRunning = true;
  organGameFinished = false;
  Serial.println(F("ORGANES: demarrage du jeu."));
  if (organEasy) {
    Serial.println(F("ORGANES: Mode FACILE (appuyez sur les boutons 3..6 pour voir les organes)."));
  } else {
    Serial.println(F("ORGANES: Mode DIFFICILE (3 questions a trouver)."));
  }
}

// organLoop: easy uses edge detection; hard does 3 questions, waits single response per question
void organLoop() {
  if (!organRunning) return;

  // Easy mode: one message per press (edge detection)
  if (organEasy) {
    static int last1 = HIGH, last2 = HIGH, last3 = HIGH, last4 = HIGH;
    int r1 = digitalRead(PIN_ORGAN_BTN1);
    int r2 = digitalRead(PIN_ORGAN_BTN2);
    int r3 = digitalRead(PIN_ORGAN_BTN3);
    int r4 = digitalRead(PIN_ORGAN_BTN4);

    if (last1 == HIGH && r1 == LOW) Serial.println(F("ORGANES: COEUR"));
    if (last2 == HIGH && r2 == LOW) Serial.println(F("ORGANES: POUMONS"));
    if (last3 == HIGH && r3 == LOW) Serial.println(F("ORGANES: FOIE"));
    if (last4 == HIGH && r4 == LOW) Serial.println(F("ORGANES: REIN"));

    last1 = r1; last2 = r2; last3 = r3; last4 = r4;
    return;
  }

  // Hard mode
  if (organGameFinished) return;

  bool asked[4] = {false,false,false,false};
  int askedCount = 0;
  while (askedCount < 3 && organRunning) {
    int t;
    do { t = random(1,5); } while (asked[t-1]);
    asked[t-1] = true;
    askedCount++;
    organTarget = t;
    if (organTarget == 1) Serial.println(F("Trouvez: COEUR"));
    if (organTarget == 2) Serial.println(F("Trouvez: POUMONS"));
    if (organTarget == 3) Serial.println(F("Trouvez: FOIE"));
    if (organTarget == 4) Serial.println(F("Trouvez: REIN"));

    // wait for single press then release
    bool answered = false;
    while (!answered && organRunning) {
      if (digitalRead(PIN_ORGAN_BTN1) == LOW) {
        answered = organCheckAnswer(1);
        while (digitalRead(PIN_ORGAN_BTN1) == LOW) delay(10); // wait release
        delay(BUTTON_DEBOUNCE_MS);
      }
      else if (digitalRead(PIN_ORGAN_BTN2) == LOW) {
        answered = organCheckAnswer(2);
        while (digitalRead(PIN_ORGAN_BTN2) == LOW) delay(10);
        delay(BUTTON_DEBOUNCE_MS);
      }
      else if (digitalRead(PIN_ORGAN_BTN3) == LOW) {
        answered = organCheckAnswer(3);
        while (digitalRead(PIN_ORGAN_BTN3) == LOW) delay(10);
        delay(BUTTON_DEBOUNCE_MS);
      }
      else if (digitalRead(PIN_ORGAN_BTN4) == LOW) {
        answered = organCheckAnswer(4);
        while (digitalRead(PIN_ORGAN_BTN4) == LOW) delay(10);
        delay(BUTTON_DEBOUNCE_MS);
      }
      delay(10);
    }
    if (!organRunning) break;
  }

  if (organRunning) {
    Serial.println(F("ORGANES: Session terminee."));
    organGameFinished = true;
  }
}

bool organCheckAnswer(int pressed) {
  if (pressed == organTarget) {
    Serial.println(F("ORGANES: Correct!"));
    return true;
  } else {
    Serial.println(F("ORGANES: Faux, essayez encore."));
    return false;
  }
}
// ---------- END OF FILE ----------



