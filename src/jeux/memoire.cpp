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
int leds[] = {A2, A3, A4, A5}; // A2=0, A3=1, A4=2, A5=3
int ledCount = sizeof(leds) / sizeof(leds[0]);

// === Mapping boutons => LED ===
int keyToLedMap[10]; // indice = key renvoyé par getKey()

void initKeyToLedMap() {
    for(int i=0;i<10;i++) keyToLedMap[i]=-1;
    keyToLedMap[7] = 1; // bouton 7 → LED A3
    keyToLedMap[9] = 0; // bouton 9 → LED A2
    keyToLedMap[1] = 2; // bouton 1 → LED A4
    keyToLedMap[3] = 3; // bouton 3 → LED A5
}

// === Boutons ===
uint32_t ADKeyVal[10] = {0};
uint32_t ADCKeyIn = 0;

// Pattern de jeu
const int PATTERN_LENGTH = 4;
const int ROUNDS_TO_WIN = 1;
const int MAX_ERRORS = 3; // nombre max d'essais
int pattern[PATTERN_LENGTH] = {0};
int playerIndex = 0;
int roundsCompleted = 0;
int errorCount = 0;
bool gameCompleted = false;
bool waitingForRelease = true;
bool messageShown = false;

// Prototypes
void ADKeybegin();
int8_t getKey();
void playPattern();
bool checkPlayer(int8_t key);
void lightLedByKey(int key, int duration);

void begin() {
    playerIndex = 0;
    roundsCompleted = 0;
    errorCount = 0;
    gameCompleted = false;
    waitingForRelease = true;
    messageShown = false;

    // Setup LEDs
    for(int i=0;i<ledCount;i++){
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i], LOW);
    }

    // Setup boutons
    ADKeybegin();
    initKeyToLedMap();

    delay(500);

    // Générer pattern aléatoire uniquement parmi les boutons valides
    int validKeys[] = {7,9,1,3};
    int numValidKeys = sizeof(validKeys)/sizeof(validKeys[0]);
    randomSeed(analogRead(A0));
    for(int i=0;i<PATTERN_LENGTH;i++){
        int r = random(0,numValidKeys);
        pattern[i] = validKeys[r];
    }

    Serial.println("=== Pattern à reproduire ===");
    for(int i=0;i<PATTERN_LENGTH;i++){
        Serial.print(pattern[i]); Serial.print(" ");
    }
    Serial.println();

    playPattern();
}

void step() {
    ADCKeyIn = analogRead(ADPIN);
    int8_t key = getKey();

    if(waitingForRelease){
        if(key==-1){
            waitingForRelease=false;
            if(!messageShown){
                Serial.println("À vous de jouer!");
                messageShown=true;
            }
        }
        return;
    }

    // Boutons valides
    int validKeys[] = {7,9,1,3};
    bool keyIsValid = false;
    for(int i=0;i<4;i++){
        if(key==validKeys[i]){ keyIsValid=true; break; }
    }
    if(!keyIsValid || key==-1) return;

    Serial.print("Touche appuyée : "); Serial.println(key);
    lightLedByKey(key,200);

    if(checkPlayer(key)){
        playerIndex++;
        if(playerIndex>=PATTERN_LENGTH){
            roundsCompleted++;
            Serial.print("Bravo! Pattern complet. Round ");
            Serial.print(roundsCompleted);
            Serial.print("/");
            Serial.println(ROUNDS_TO_WIN);

            if(roundsCompleted >= ROUNDS_TO_WIN){
                Serial.println("=== VICTOIRE! Jeu Mémoire terminé! ===");

                // Allumer les deux LEDs 1 et 9 en même temps
                int led1 = keyToLedMap[1];
                int led9 = keyToLedMap[9];
                if(led1 != -1) digitalWrite(leds[led1], HIGH);
                if(led9 != -1) digitalWrite(leds[led9], HIGH);
                delay(1000);
                if(led1 != -1) digitalWrite(leds[led1], LOW);
                if(led9 != -1) digitalWrite(leds[led9], LOW);

                gameCompleted = true;
                delay(2000);
                return;
            }

            delay(1000);
            playerIndex=0;
            playPattern();
        }
    } else {
        errorCount++;
        Serial.print("Erreur! Recommencez. Essai ");
        Serial.print(errorCount);
        Serial.print("/");
        Serial.println(MAX_ERRORS);

        playerIndex=0;
        delay(1000);
        playPattern();

        if(errorCount >= MAX_ERRORS){
            Serial.println("=== ÉCHEC! Vous avez épuisé vos 3 essais ===");

            // Allumer les deux LEDs 7 et 3 en même temps
            int led7 = keyToLedMap[7];
            int led3 = keyToLedMap[3];
            if(led7 != -1) digitalWrite(leds[led7], HIGH);
            if(led3 != -1) digitalWrite(leds[led3], HIGH);
            delay(1000);
            if(led7 != -1) digitalWrite(leds[led7], LOW);
            if(led3 != -1) digitalWrite(leds[led3], LOW);

            gameCompleted = true;
            delay(2000);
        }
    }
}

// === Fonctions ===
void ADKeybegin(){
    float RESValue[10] = {0,3,6.2,9.1,15,24,33,51,100,220};
    for(uint8_t i=0;i<10;i++){
        ADKeyVal[i] = RESValue[i]/(RESValue[i]+22)*MAXVALUE;
    }
}

int8_t getKey(){
    for(uint8_t i=0;i<10;i++){
        if(abs((int)ADCKeyIn-(int)ADKeyVal[i])<PRECISION) return i;
    }
    return -1;
}

void playPattern(){
    Serial.println("Pattern :");
    for(int i=0;i<PATTERN_LENGTH;i++){
        lightLedByKey(pattern[i],400);
        delay(200);
    }
    Serial.println("À vous de jouer!");
}

bool checkPlayer(int8_t key){
    return key==pattern[playerIndex];
}

void lightLedByKey(int key,int duration){
    int ledIndex = keyToLedMap[key];
    if(ledIndex==-1) return;
    digitalWrite(leds[ledIndex],HIGH);
    delay(duration);
    digitalWrite(leds[ledIndex],LOW);
}

bool isCompleted(){
    return gameCompleted;
}

} // namespace Memoire
