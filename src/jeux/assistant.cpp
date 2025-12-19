#include "../include/jeux/assistant.h"
#include <SPI.h>
#include <MFRC522.h>

// --- Pin Definitions ---
#define RST_PIN 9
#define SS_PIN 10

// --- RFID partagé (défini globalement dans corps_humain.cpp) ---
extern MFRC522 mfrc522;

namespace Assistant {

// --- Variables ---
static unsigned long lastBadgeTime = 0;

// --- UIDs des badges utilisateurs ---
#define UID_YASMINA "1A6CCF9A"  // Thème Santé
#define UID_ALEX "6590FB75"      // Thème Sport

// --- Phrases par thème (stockées en PROGMEM pour économiser la RAM) ---
const char sante0[] PROGMEM = "Buvé une litre d'eau par jour.";
const char sante1[] PROGMEM = "Dormé entre 7 et 9 heures chaque nuit.";
const char sante2[] PROGMEM = "Mangé 5 fruits.";

const char* const santeList[] PROGMEM = {sante0, sante1, sante2};
const int nbSante = 3;

const char sport0[] PROGMEM = "l'équipe de Football contient 11 joueurs.";
const char sport1[] PROGMEM = "la Natation renforce les muscles.";
const char sport2[] PROGMEM = "le Basketball a été inventé en 1891.";

const char* const sportList[] PROGMEM = {sport0, sport1, sport2};
const int nbSport = 3;

// --- État ---
const char* const* themeActuel = nullptr;
static int nbPhrases = 0;
static bool lectureActive = false;
static bool attenteTheme = true;
static bool assistantCompleted = false;
static unsigned long lastPhraseTime = 0;
static int phrasesAffichees = 0;  // Compteur de phrases
static int MAX_PHRASES = 3;  // 3 phrases par thème
static int phrasesOrdre[3];  // Ordre d'affichage des phrases (sans répétition)

// --- Prototypes ---
void lireBadge();
void choisirTheme(const char* uid);
void direPhrase(const char* const* theme, int count);

// --- Function: begin() ---
void begin() {
  // Initialiser SPI et RFID
  SPI.begin();
  mfrc522.PCD_Init();
  
  Serial.println("\n=== Bonjour Je suis Imane Votre assistante ===");
  Serial.println("Scannez un badge pour choisir un thème :");

  
  randomSeed(analogRead(A0));
  
  attenteTheme = true;
  lectureActive = false;
  themeActuel = nullptr;
  phrasesAffichees = 0;
  assistantCompleted = false;
}

// --- Function: step() ---
void step() {
  // --- En attente du badge ---
  if (attenteTheme && themeActuel == nullptr) {
    lireBadge();
  }
  // --- Lecture (1 phrase toutes les 1 sec, max 3 phrases) ---
  if (lectureActive && themeActuel != nullptr) {
    if (millis() - lastPhraseTime > 1000) {   // 1 secondes
      if (phrasesAffichees < MAX_PHRASES) {
        direPhrase(themeActuel, nbPhrases);
        phrasesAffichees++;
        lastPhraseTime = millis();
        
        if (phrasesAffichees >= MAX_PHRASES) {
          Serial.println("\n=== Fin du theme ===");
          lectureActive = false;
          attenteTheme = true;
          themeActuel = nullptr;
          phrasesAffichees = 0;
          assistantCompleted = true;  // Marquer comme terminé
        }
      }
    }
  }
}

// ---------- RFID ----------
void lireBadge() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;
  
  // Éviter de lire le même badge trop rapidement
  unsigned long currentTime = millis();
  if (currentTime - lastBadgeTime < 2000) {
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }
  lastBadgeTime = currentTime;
  
  // Lire UID
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  
  Serial.print("Badge détecté : ");
  
  choisirTheme(uid.c_str());
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// ---------- CHOIX THEME ----------
void choisirTheme(const char* uid) {
  
  if (strcmp(uid, UID_YASMINA) == 0) {
    Serial.println("🌸 Bonjour Yasmina ! Thème : SANTÉ");
    themeActuel = santeList;
    nbPhrases = nbSante;
  }
  else if (strcmp(uid, UID_ALEX) == 0) {
    Serial.println("⚽ Salut Joris ! Thème : SPORT");
    themeActuel = sportList;
    nbPhrases = nbSport;
  }
  else {
    Serial.println("Badge non reconnu");
    return;
  }
  
  // Mélanger l'ordre des phrases (shuffle)
  for (int i = 0; i < nbPhrases; i++) {
    phrasesOrdre[i] = i;
  }
  for (int i = nbPhrases - 1; i > 0; i--) {
    int j = random(i + 1);
    int temp = phrasesOrdre[i];
    phrasesOrdre[i] = phrasesOrdre[j];
    phrasesOrdre[j] = temp;
  }
  
  // On démarre la lecture
  lectureActive = true;
  attenteTheme = false;
  phrasesAffichees = 0;
  lastPhraseTime = millis() - 1000;  // pour afficher immédiatement
}

// ---------- PHRASE ALEATOIRE ----------
void direPhrase(const char* const* theme, int count) {
  int index = phrasesOrdre[phrasesAffichees];  // Utiliser l'ordre mélangé
  char buffer[50];
  strcpy_P(buffer, (char*)pgm_read_word(&(theme[index])));
  Serial.println(buffer);
}

bool isCompleted() {
  return assistantCompleted;
}

} // namespace Assistant
