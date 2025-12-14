import serial
import pyttsx3
import time
import serial.tools.list_ports

#python ./test/tts_reader.py

def find_arduino_port():
    """Trouve automatiquement le port COM de l'Arduino"""
    for port in serial.tools.list_ports.comports():
        if 'Arduino' in port.description or 'CH340' in port.description or 'USB' in port.description:
            return port.device
    return 'COM5'  # Port par défaut

def initialize_tts():
    """Initialise le moteur de synthèse vocale"""
    engine = pyttsx3.init()
    engine.startLoop(False)
    
    # Configuration de la voix française
    voices = engine.getProperty('voices')
    french_voice_found = False
    
    for voice in voices:
        if 'fr' in voice.id.lower() or 'french' in voice.name.lower():
            engine.setProperty('voice', voice.id)
            french_voice_found = True
            print(f"✅ Voix française sélectionnée : {voice.name}")
            break
    
    if not french_voice_found:
        print("⚠️ ATTENTION : Aucune voix française trouvée. Utilisation de la voix par défaut.")
    
    # Réglages
    engine.setProperty('rate', 200)  # Vitesse de lecture (150 = normal, 200 = rapide)
    engine.setProperty('volume', 0.9)  # Volume
    
    return engine

def process_message(line, engine):
    """Traite les messages reçus et décide quoi dire"""
    line = line.strip()
    
    # Ignorer les lignes vides ou trop courtes
    if len(line) < 3:
        return
    
    # Ignorer seulement le debug du pattern
    if "Pattern :" in line:
        return
    
    # Nettoyer : garder seulement les lettres et les espaces
    clean_line = ''.join(char for char in line if char.isalpha() or char.isspace() or char.isdigit())
    clean_line = clean_line.strip()
    
    # Ignorer les lignes vides après nettoyage
    if len(clean_line) < 3:
        return
    
    # Lire le message nettoyé
    engine.say(clean_line)


def main():
    """Fonction principale avec gestion d'erreurs et reconnexion automatique"""
    print("🎵 === SYSTÈME DE SYNTHÈSE VOCALE LUDICUBE ===")
    print("🔄 Recherche automatique du port Arduino...")
    
    # Initialisation du moteur TTS
    engine = initialize_tts()
    
    # Boucle principale avec reconnexion automatique
    while True:
        try:
            # Détection automatique du port
            port = find_arduino_port()
            print(f"🔌 Tentative de connexion sur {port}...")
            
            # Connexion série (115200 baud comme dans platformio.ini)
            ser = serial.Serial(port, 115200, timeout=1)
            time.sleep(2)
            print(f"✅ Connexion établie sur {port}")
            print("👂 En écoute des messages Arduino...")
            print("=" * 50)
            
            # Boucle de lecture
            while True:
                try:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    
                    if line:
                        # Afficher le message reçu
                        print(f"📨 {line}")
                        
                        # Traiter et parler si nécessaire
                        process_message(line, engine)
                    
                    # Traitement de la synthèse vocale
                    engine.iterate()
                    
                    if not engine.isBusy():
                        time.sleep(0.01)
                        
                except UnicodeDecodeError:
                    # Ignore les erreurs de décodage
                    continue
                except Exception as e:
                    print(f"❌ Erreur de lecture : {e}")
                    break
                    
        except serial.SerialException as e:
            print(f"❌ Erreur de connexion série : {e}")
            print("🔄 Nouvelle tentative dans 3 secondes...")
            time.sleep(3)
            continue
            
        except KeyboardInterrupt:
            print("\n👋 Arrêt demandé par l'utilisateur.")
            break
            
        except Exception as e:
            print(f"❌ Erreur inattendue : {e}")
            print("🔄 Redémarrage dans 5 secondes...")
            time.sleep(5)
            continue
        
        finally:
            try:
                ser.close()
                print("🔌 Connexion série fermée.")
            except:
                pass
            
            try:
                engine.endLoop()
            except:
                pass

if __name__ == "__main__":
    main()
