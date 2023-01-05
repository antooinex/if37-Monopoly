//TODO
//6 cartes lieu
//1 carte chance
//4 cartes joueur
//1 carte impot
//1 carte communauté

//pour une action le joueur scanne le lieu puis son joueur ou l'inverse
//ensuite choix de l'action à faire


#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <SPI.h>
#include <MFRC522.h>
#include <millisDelay.h>
#include "Lieu.h"

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 2;  // Connects to module's RX
static const uint8_t PIN_MP3_RX = 3;  // Connects to module's TX
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

// Create the Player object
DFRobotDFPlayerMini player;

//Create the timer object
millisDelay timer;

//définition des variables de lecture des boutons
String uid = "";
int btnVal = 0;
const int btnAvant = A5;
int btnValAvant = 0;
const int btnArriere = A4;
int btnValArriere = 0;
bool appui = false;
bool appuiAvant = false;
bool appuiArriere = false;
int piste = 1;
const int btnPin = A0;

//défnition des pistes audio
//nos noms, inutiles sauf pour tester
int koffi = 100;
int marielle = 101;
int tony = 102;
int antoine = 103;
//dé
int un = 201;
int deux = 202;
int trois = 203;
int quatre = 204;
int cinq = 205;
int six = 206;
int annonceDeDouble = 207;
int et = 208;
//menu
int annonceLancerDes = 300;
int annonceLieu = 301;
int annonceAcheter = 302;
int annonceVendre = 303;
int annonceEchange = 304;
int annonceFinTour = 305;
int indexIncorrect = 306;
//joueurs
int annonceJ1 = 401;
int annonceJ2 = 402;
int annonceJ3 = 403;
int annonceJ4 = 404;
int annoncePersonne = 405;
//actions
int annonceResultatDes = 500;
int annoncePrison = 501;
int annonceLancerImpossible = 502;
int annonceScannerCarte = 503;
//caisse de communauté
int caisseUn = 600;   
int caisseDeux = 601; 
int annonceCarteCommunaute = 602;
//chance
int chanceUn = 700;  
int chanceDeux = 701; 
int annonceCarteChance = 702;
//frais de scolarité
int annonceFraisScolarite = 800;

//lieu
int mindTechNom = 801;    
int mindLabNom = 802;     
int laRueNom = 803;       
int bdeNom = 804;         
int foyerNom = 895;       
int gareTroyesNom = 805;  
int annonceProprio = 806;     
int annonceLibre = 405;       //même chose que annoncePersonne
int annonceDejaAchete = 808;  
int annonceAchat = 809;       
int annonceVendu = 810;      
int annonceNonProprio = 811;  
int NonValide = 812;          
int prixLieu60 = 813;         
int prixLieu100 = 814;        
int prixLieu120 = 815;        
int prixLieu200 = 816;        
int annonceDebutJeu = 817;
int annoncePasserBadge = 818;
int hypotheque30M = 819;
int hypotheque50M = 820;
int hypotheque60M = 821;
int hypotheque100M = 822;
int prixMaisonHotel = 823;

int mindTechInfo = 901;    
int mindLabInfo = 902;     
int laRueInfo = 903;       
int bdeInfo = 903;         //Même chose que la rue
int foyerInfo = 904;       
int gareTroyesInfo = 905;  

//Carte avec uid
String caisse = " 04 2c";  //A faire les choses ci dessous
String chance = " 04 30";
String fraisScolarite = " 04 a2";
//instance des objet lieu
Lieu mindTech(" 19 16");  //A ajouter remplacer uid
Lieu mindLab(" 04 fd");
Lieu laRue(" 04 ec");
Lieu bde(" 04 84");
Lieu foyer(" 04 5b");
Lieu gareTroyes(" 04 01");

//définition du nom des cartes
String joueur1 = " 19 dd";
String joueur2 = " da f3";
String joueur3 = " ab 60";
String joueur4 = " 04 4e";

//définition autres variables globales
int menu = 0;
int ancienneValeur = menu;
bool finTour = false;
int prison = 0;
bool rejouer = false;
bool premierTour = true;
String joueurCourant;

//fonctions du programme en dessous
void attendreEtLireAudio(int audio) {
  player.playMp3Folder(audio);
  delay(1000);
}

//retourne true si une carte est présente sur le capteur et affecte sa valeur à uid
bool lireCarte() {
  uid = "";
  if (rfid.PICC_IsNewCardPresent()) {  // new tag is available
    if (rfid.PICC_ReadCardSerial()) {  // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      for (int i = 0; i < 2; i++) {
        //Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        //Serial.print(rfid.uid.uidByte[i], HEX);
        uid.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
        uid.concat(String(rfid.uid.uidByte[i], HEX));
      }
      Serial.print("uid:");
      Serial.println(uid);
      rfid.PICC_HaltA();       // halt PICC
      rfid.PCD_StopCrypto1();  // stop encryption on PCD
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

//bouton appuyer qui donne le numero de la selection
int appuiBtnPrincipal() {
  btnVal = analogRead(btnPin);
  if (btnVal < 200) {
    if (timer.remaining() == 0) {
      //Serial.print(F("Appui bouton principal"));
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

//bouton tourner qui change la valeur piste, retourne void
int btnTourner(int numero) {
  //Read bouton avant avec sécurité timer
  if (analogRead(btnAvant) < 200 && timer.remaining() == 0) {
    //Serial.println(F("Appui Bouton Avant "));
    //Serial.println("+1");
    numero += 1;
    timer.start(400);
    //Serial.println(numero);
    return numero;
  }
  //Read bouton arriere avec sécurité timer
  else if (analogRead(btnArriere) < 200 && timer.remaining() == 0) {
    //Serial.println(F("Appui Bouton Arriere "));
    //Serial.println("-1");
    numero -= 1;
    timer.start(400);
    //Serial.println(numero);
    return numero;
  }
}

//annonce les items du menu
void annonceMenu(int menu) {
  Serial.print("Menu : ");
  switch (menu) {
    case 0:
      Serial.println("Lancer les dés");
      attendreEtLireAudio(annonceLancerDes);
      break;

    case 1:
      Serial.println("Informations sur un lieu");
      attendreEtLireAudio(annonceLieu);
      break;

    case 2:
      Serial.println("Acheter");
      attendreEtLireAudio(annonceAcheter);
      break;

    case 3:
      Serial.println("Vendre");
      attendreEtLireAudio(annonceVendre);
      break;

    case 4:
      Serial.println("Echange");
      attendreEtLireAudio(annonceEchange);
      break;

    case 5:
      Serial.println("Fin de tour");
      attendreEtLireAudio(annonceFinTour);
      break;

    default:
      Serial.println("Index du menu incorrect");
      break;
  }
}

//met l'audio selon le resultat d'un dé
void audioDe(int chiffreDe) {
  switch (chiffreDe) {

    case 1:
      attendreEtLireAudio(un);  //audio dé "1"
      break;

    case 2:
      attendreEtLireAudio(deux);  //audio dé "2"
      break;

    case 3:
      attendreEtLireAudio(trois);  //audio dé "3"
      break;

    case 4:
      attendreEtLireAudio(quatre);  //audio dé "4"
      break;

    case 5:
      attendreEtLireAudio(cinq);  //audio dé "5"
      break;

    case 6:
      attendreEtLireAudio(six);  //audio dé "6"
      break;
  }
}

void audioCaisse(int chiffreCaisse) {
  switch (chiffreCaisse) {

    case 1:
      attendreEtLireAudio(caisseUn);  //audio caisse de communauté
      delay(5000);
      break;

    case 2:
      attendreEtLireAudio(caisseDeux);  //audio caisse de communauté different du premier
      delay(5000);
      break;
  }
}

void audioChance(int chiffreChance) {
  switch (chiffreChance) {

    case 1:
      attendreEtLireAudio(chanceUn);  //audio chance
      delay(3000);
      break;

    case 2:
      attendreEtLireAudio(chanceDeux);  //audio chance different du premier
      delay(6000);
      break;
  }
}

int audioNumberJoueur(String stringJoueur) {
  if (stringJoueur == "joueur1") {
    return annonceJ1;
  } else if (stringJoueur == "joueur2") {
    return annonceJ2;
  } else if (stringJoueur == "joueur3") {
    return annonceJ3;
  } else if (stringJoueur == "joueur4") {
    return annonceJ4;
  }
}


void setup() {
  // Init USB serial port for debugging
  Serial.begin(9600);
  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
    // Set volume to maximum (0 to 30).
    player.volume(30);
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }

  //Init btn
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(btnAvant, INPUT_PULLUP);
  pinMode(btnArriere, INPUT_PULLUP);

  SPI.begin();      // init SPI bus
  rfid.PCD_Init();  // init MFRC522

  Serial.println("Début du jeu");
  attendreEtLireAudio(annonceDebutJeu);
  Serial.println("Scannez un pion joueur");
  attendreEtLireAudio(annoncePasserBadge);
}

void loop() {

  //lire l'uid d'une carte et mettre sa valeur dans uid
  if (lireCarte()) {
    //remise à zéro des variables globales
    finTour = false;
    prison = 0;
    rejouer = false;
    premierTour = true;
    menu = 0;
    ancienneValeur = 0;

    // détection d'un joueur
    if (uid == joueur1) {
      joueurCourant = "joueur1";
      //annonce du joueur sélectionné
      Serial.println("Joueur 1");
      attendreEtLireAudio(annonceJ1);
      //boucle tant que le joueur ne finit pas son tour
      while (!finTour) {
        //entrée dans le menu
        annonceMenu(menu);
        while (!appuiBtnPrincipal()) {
          ancienneValeur = menu;
          menu = btnTourner(menu);
          if (menu > 5) {
            //remettre la valeur min du menu si supérieur au max
            menu = 0;
          } else if (menu < 0) {
            //remettre la veleur max du menu si inférieur au min
            menu = 5;
          }
          if (menu != ancienneValeur) {
            //Serial.print("menu : ");
            //Serial.println(menu);
            //Serial.print("ancienneValeur : ");
            //Serial.println(ancienneValeur);
            //annonce de la nouvelle sélection dans le menu
            annonceMenu(menu);
          }
        }
        switch (menu) {
          case 0:
            //lancé de dé
            if (premierTour || rejouer) {
              premierTour = false;
              rejouer = false;
              int de1 = random(1, 7);
              int de2 = random(1, 7);
              attendreEtLireAudio(annonceResultatDes);  //audio "Vous avez fait..."
              Serial.print("Dé 1 : ");
              Serial.println(de1);
              Serial.print("Dé 2 : ");
              Serial.println(de2);
              audioDe(de1);
              attendreEtLireAudio(et);
              audioDe(de2);
              if (de1 == de2) {
                Serial.println("Vous avez fait un double.");
                attendreEtLireAudio(annonceDeDouble);  //audio "Vous avez fait un double..."
                prison += 1;
                rejouer = true;
                if (prison == 3) {
                  Serial.println("Vous allez en prison.");
                  attendreEtLireAudio(annoncePrison);  //audio "Vous allez en prison"
                  delay(2000);
                  rejouer = false;
                }
              }
            } else {
              Serial.println("Vous ne pouvez plus lancer les dés pour ce tour.");
              attendreEtLireAudio(annonceLancerImpossible);  //audio "tentative de lancer de dé insuffisant"
              delay(1000);
            }
            break;

          case 1:
            //info carte nfc
            Serial.println("Veuillez scanner une carte.");
            attendreEtLireAudio(annonceScannerCarte);  //veuillez scanner la carte du lieu
            while (!lireCarte()) {
              //attente d'une carte
            }
            //si caisse de communauté
            if (uid == caisse) {
              int chiffreCaisse = random(1, 3);
              Serial.print("Caisse de communauté n°");
              Serial.println(chiffreCaisse);
              audioCaisse(chiffreCaisse);
            } else if (uid == chance) {
              int chiffreChance = random(1, 3);
              Serial.print("Carte chance n°");
              Serial.println(chiffreChance);
              audioChance(chiffreChance);
            } else if (uid == fraisScolarite) {
              attendreEtLireAudio(annonceFraisScolarite);  //Veuillez payer 200M
              delay(3700);
              Serial.println("Frais de scolarité : payer 200M");
            } else if (uid == mindTech.getUid()) {
              attendreEtLireAudio(mindTechNom);  //nom du lieu
              delay(1000);
              Serial.print("Lieu scanné : ");
              Serial.println("Le MindTech");
              attendreEtLireAudio(annonceProprio);
              delay(800);  //Le priopriétaire du lieu est ...
              Serial.print("Le propriétaire du lieu est : ");
              if (mindTech.getProprio() == "joueur1") {
                Serial.println("Joueur 1");
                attendreEtLireAudio(annonceJ1);  //joueur 1
              } else if (mindTech.getProprio() == "joueur2") {
                Serial.println("Joueur 2");
                attendreEtLireAudio(annonceJ2);  //joueur 2
              } else if (mindTech.getProprio() == "joueur3") {
                Serial.println("Joueur 3");
                attendreEtLireAudio(annonceJ3);  //joueur 3
              } else if (mindTech.getProprio() == "joueur4") {
                Serial.println("Joueur 4");
                attendreEtLireAudio(annonceJ4);  //joueur 4
              } else {
                Serial.println("personne !");
                attendreEtLireAudio(annonceLibre);  //personne
              }
              attendreEtLireAudio(mindTechInfo);  //info Mind Tech
              delay(14000);
              attendreEtLireAudio(hypotheque30M);
              delay(1500);  
              attendreEtLireAudio(prixMaisonHotel);
              delay(4000);
            } else if (uid == mindLab.getUid()) {
              Serial.print("Lieu scanné : ");
              Serial.println("Le MindLab");
              attendreEtLireAudio(mindLabNom);  //nom du lieu
              delay(1000);
              Serial.print("Le propriétaire du lieu est : ");
              attendreEtLireAudio(annonceProprio);  //Le priopriétaire du lieu est ...
              delay(800);
              if (mindLab.getProprio() == "joueur1") {
                Serial.println("Joueur 1");
                attendreEtLireAudio(annonceJ1);  //joueur 1
              } else if (mindLab.getProprio() == "joueur2") {
                Serial.println("Joueur 2");
                attendreEtLireAudio(annonceJ2);  //joueur 2
              } else if (mindLab.getProprio() == "joueur3") {
                Serial.println("Joueur 3");
                attendreEtLireAudio(annonceJ3);  //joueur 3
              } else if (mindLab.getProprio() == "joueur4") {
                Serial.println("Joueur 4");
                attendreEtLireAudio(annonceJ4);  //joueur 4
              } else {
                Serial.println("personne !");
                attendreEtLireAudio(annonceLibre);  //personne
              }
              attendreEtLireAudio(mindLabInfo);  //info Mind Lab
              delay(14000);
              attendreEtLireAudio(hypotheque30M);
              delay(1500);  
              attendreEtLireAudio(prixMaisonHotel);
              delay(4000);

            } else if (uid == laRue.getUid()) {
              Serial.print("Lieu scanné : ");
              Serial.println("La Rue");
              attendreEtLireAudio(laRueNom);  //nom du lieu
              delay(1000);
              Serial.print("Le propriétaire du lieu est : ");
              attendreEtLireAudio(annonceProprio);  //Le priopriétaire du lieu est ...
              delay(800);
              if (laRue.getProprio() == "joueur1") {
                Serial.println("Joueur 1");
                attendreEtLireAudio(annonceJ1);  //joueur 1
              } else if (laRue.getProprio() == "joueur2") {
                Serial.println("Joueur 2");
                attendreEtLireAudio(annonceJ2);  //joueur 2
              } else if (laRue.getProprio() == "joueur3") {
                Serial.println("Joueur 3");
                attendreEtLireAudio(annonceJ3);  //joueur 3
              } else if (laRue.getProprio() == "joueur4") {
                Serial.println("Joueur 4");
                attendreEtLireAudio(annonceJ4);  //joueur 4
              } else {
                Serial.println("personne !");
                attendreEtLireAudio(annonceLibre);  //personne
              }
              attendreEtLireAudio(laRueInfo);  //info la Rue
              delay(14000);
              attendreEtLireAudio(hypotheque50M);
              delay(1500);  
              attendreEtLireAudio(prixMaisonHotel);
              delay(4000);
            } else if (uid == bde.getUid()) {
              Serial.print("Lieu scanné : ");
              Serial.println("Le BDE");
              attendreEtLireAudio(bdeNom);  //nom du lieu
              delay(1000);
              Serial.print("Le propriétaire du lieu est : ");
              attendreEtLireAudio(annonceProprio);  //Le priopriétaire du lieu est ...
              delay(800);
              if (bde.getProprio() == "joueur1") {
                Serial.println("Joueur 1");
                attendreEtLireAudio(annonceJ1);  //joueur 1
              } else if (bde.getProprio() == "joueur2") {
                Serial.println("Joueur 2");
                attendreEtLireAudio(annonceJ2);  //joueur 2
              } else if (bde.getProprio() == "joueur3") {
                Serial.println("Joueur 3");
                attendreEtLireAudio(annonceJ3);  //joueur 3
              } else if (bde.getProprio() == "joueur4") {
                Serial.println("Joueur 4");
                attendreEtLireAudio(annonceJ4);  //joueur 4
              } else {
                Serial.println("personne !");
                attendreEtLireAudio(annonceLibre);  //personne
              }
              attendreEtLireAudio(bdeInfo);  //info bde
              delay(14000);
              attendreEtLireAudio(hypotheque50M);
              delay(1500);  
              attendreEtLireAudio(prixMaisonHotel);
              delay(4000);
            } else if (uid == foyer.getUid()) {
              Serial.print("Lieu scanné : ");
              Serial.println("Le Foyer");
              attendreEtLireAudio(foyerNom);  //nom du lieu
              delay(1000);
              Serial.print("Le propriétaire du lieu est : ");
              attendreEtLireAudio(annonceProprio);  //Le priopriétaire du lieu est ...
              delay(800);
              if (foyer.getProprio() == "joueur1") {
                Serial.println("Joueur 1");
                attendreEtLireAudio(annonceJ1);  //joueur 1
              } else if (foyer.getProprio() == "joueur2") {
                Serial.println("Joueur 2");
                attendreEtLireAudio(annonceJ2);  //joueur 2
              } else if (foyer.getProprio() == "joueur3") {
                Serial.println("Joueur 3");
                attendreEtLireAudio(annonceJ3);  //joueur 3
              } else if (foyer.getProprio() == "joueur4") {
                Serial.println("Joueur 4");
                attendreEtLireAudio(annonceJ4);  //joueur 4
              } else {
                Serial.println("personne !");
                attendreEtLireAudio(annonceLibre);  //personne
              }
              attendreEtLireAudio(foyerInfo);  //info foyer
              delay(8000);
              attendreEtLireAudio(hypotheque60M);
              delay(1500);  
              attendreEtLireAudio(prixMaisonHotel);
              delay(4000);
            } else if (uid == gareTroyes.getUid()) {
              Serial.print("Lieu scanné : ");
              Serial.println("La Gare de Troyes");
              attendreEtLireAudio(gareTroyesNom);  //nom du lieu
              delay(1000);
              Serial.print("Le propriétaire du lieu est : ");
              delay(800);
              attendreEtLireAudio(annonceProprio);  //Le priopriétaire du lieu est ...
              delay(800);
              if (gareTroyes.getProprio() == "joueur1") {
                attendreEtLireAudio(annonceJ1);  //joueur 1
              } else if (gareTroyes.getProprio() == "joueur2") {
                attendreEtLireAudio(annonceJ2);  //joueur 2
              } else if (gareTroyes.getProprio() == "joueur3") {
                attendreEtLireAudio(annonceJ3);  //joueur 3
              } else if (gareTroyes.getProprio() == "joueur4") {
                attendreEtLireAudio(annonceJ4);  //joueur 4
              } else {
                Serial.println("personne !");
                attendreEtLireAudio(annonceLibre);  //personne
              }
              attendreEtLireAudio(gareTroyesInfo);  //info gare de Troyes
              delay(9000);
              attendreEtLireAudio(hypotheque100M);
              delay(1500); 
            } else {
              Serial.println("Carte inconnue.");
              attendreEtLireAudio(NonValide);  //carte scanné non valide
            }
            break;

          case 2:
            //acheter
            Serial.println("Veuillez scanner une carte.");
            attendreEtLireAudio(annonceScannerCarte);  //veuillez scanner la carte du lieu ( peut etre plus specific en precisant que la carte scanné sera pour acheter ( donc faire un new audio ))
            while (!lireCarte()) {
              //attente d'une carte
            }
            if (uid == mindTech.getUid()) {
              if (mindTech.getEstAchete() == true) {
                Serial.print("Ce lieu est déjà possédé par : ");
                attendreEtLireAudio(annonceDejaAchete);  //Ce lieu est deja acheté par...
                delay(1500);
                Serial.println(mindTech.getProprio());
                attendreEtLireAudio(audioNumberJoueur(mindTech.getProprio()));  //nom du joueur
              } else {
                Serial.print("Le MindTech ");
                attendreEtLireAudio(mindTechNom);  //nom du lieu
                delay(1000);
                Serial.print("est acheté au");
                attendreEtLireAudio(annonceAchat);  //Vous avez acheté...
                Serial.print("prix : ");
                attendreEtLireAudio(prixLieu60);  //pour 60M
                delay(2000);
                Serial.println(prixLieu60);
                mindTech.setAchete(joueurCourant);
              }
            } else if (uid == mindLab.getUid()) {
              if (mindLab.getEstAchete() == true) {
                Serial.print("Ce lieu est déjà possédé par : ");
                attendreEtLireAudio(annonceDejaAchete);  //Ce lieu est deja acheté par...
                delay(1500);
                Serial.println(mindLab.getProprio());
                attendreEtLireAudio(audioNumberJoueur(mindLab.getProprio()));  //nom du joueur
              } else {
                Serial.print("Le MindLab ");
                attendreEtLireAudio(mindLabNom);  //nom du lieu
                delay(1000);
                Serial.print("Vous avez acheté : ");
                attendreEtLireAudio(annonceAchat);  //Vous avez acheté...
                Serial.print(", à un prix de : ");
                attendreEtLireAudio(prixLieu60);
                delay(2000);
                Serial.println(prixLieu60);
                mindLab.setAchete(joueurCourant);
              }
            } else if (uid == laRue.getUid()) {
              if (laRue.getEstAchete() == true) {
                Serial.print("Ce lieu est déjà possédé par : ");
                attendreEtLireAudio(annonceDejaAchete);  //Ce lieu est deja acheté par...
                delay(1500);
                Serial.println(laRue.getProprio());
                attendreEtLireAudio(audioNumberJoueur(laRue.getProprio()));  //nom du joueur
              } else {
                Serial.print("La Rue ");
                attendreEtLireAudio(laRueNom);  //nom du lieu
                delay(1000);
                Serial.print("Vous avez acheté : ");
                attendreEtLireAudio(annonceAchat);  //Vous avez acheté...
                Serial.print(", à un prix de : ");
                attendreEtLireAudio(prixLieu100);
                delay(2000);
                Serial.println(prixLieu100);
                laRue.setAchete(joueurCourant);
              }
            } else if (uid == bde.getUid()) {
              if (bde.getEstAchete() == true) {
                Serial.print("Ce lieu est déjà possédé par : ");
                attendreEtLireAudio(annonceDejaAchete);  //Ce lieu est deja acheté par...
                delay(1500);
                Serial.println(bde.getProprio());
                attendreEtLireAudio(audioNumberJoueur(bde.getProprio()));  //nom du joueur
              } else {
                Serial.print("BDE");
                attendreEtLireAudio(bdeNom);  //nom du lieu
                delay(1000);
                Serial.print("Vous avez acheté : ");
                attendreEtLireAudio(annonceAchat);  //Vous avez acheté...
                Serial.print(", à un prix de : ");
                attendreEtLireAudio(prixLieu100);
                delay(2000);
                Serial.println(prixLieu100);
                bde.setAchete(joueurCourant);
              }
            } else if (uid == foyer.getUid()) {
              if (foyer.getEstAchete() == true) {
                Serial.print("Ce lieu est déjà possédé par : ");
                attendreEtLireAudio(annonceDejaAchete);  //Ce lieu est deja acheté par...
                delay(1500);
                Serial.println(foyer.getProprio());
                attendreEtLireAudio(audioNumberJoueur(foyer.getProprio()));  //nom du joueur
              } else {
                Serial.print("Le Foyer");
                attendreEtLireAudio(foyerNom);  //nom du lieu
                delay(1000);
                Serial.print("Vous avez acheté : ");
                attendreEtLireAudio(annonceAchat);  //Vous avez acheté...
                Serial.print(", à un prix de : ");
                attendreEtLireAudio(prixLieu120);
                delay(2000);
                Serial.println(prixLieu120);
                foyer.setAchete(joueurCourant);
              }
            } else if (uid == gareTroyes.getUid()) {
              if (gareTroyes.getEstAchete() == true) {
                Serial.print("Ce lieu est déjà possédé par : ");
                attendreEtLireAudio(annonceDejaAchete);  //Ce lieu est deja acheté par...
                delay(1500);
                Serial.println(gareTroyes.getProprio());
                attendreEtLireAudio(audioNumberJoueur(gareTroyes.getProprio()));  //nom du joueur
              } else {
                Serial.print("Gare de Troyes");
                attendreEtLireAudio(gareTroyesNom);  //nom du lieu
                delay(1000);
                Serial.print("Vous avez acheté : ");
                attendreEtLireAudio(annonceAchat);  //Vous avez acheté...
                Serial.print(", à un prix de : ");
                attendreEtLireAudio(prixLieu200);
                delay(2000);
                Serial.println(prixLieu200);
                gareTroyes.setAchete(joueurCourant);
              }
            } else {
              Serial.println("Carte inconnue.");
              attendreEtLireAudio(NonValide);  //carte scanné non valide
              delay(1000);
            }
            break;

          case 3:
            //vendre
            Serial.println("Veuillez scanner une carte.");
            attendreEtLireAudio(annonceScannerCarte);  //veuillez scanner la carte du lieu ( peut etre plus specific en precisant que la carte scanné sera pour le vendre ( donc faire un new audio ))
            while (!lireCarte()) {
              //attente d'une carte
            }
            if (uid == mindTech.getUid()) {
              if (mindTech.getProprio() == joueurCourant) {
                Serial.print("Vous avez vendu : ");
                attendreEtLireAudio(mindTechNom);  //nom du lieu
                delay(1000);
                Serial.print("Le MindTech ");
                attendreEtLireAudio(annonceVendu);  //...est vendu pour
                Serial.print(", à un prix de : ");
                attendreEtLireAudio(prixLieu60);  //60M
                delay(2000);
                Serial.println(prixLieu60);
                mindTech.setVendre();
              } else {
                Serial.print("Vous n'êtes pas le propriétaire du lieu : ");
                attendreEtLireAudio(annonceNonProprio);  //Vous n'etes pas le proprio du lieu
                delay(1000);
                Serial.println("Le MindTech");
                attendreEtLireAudio(mindTechNom);  //nom du lieu
                delay(1000);
              }
            } else if (uid == mindLab.getUid()) {
              if (mindLab.getProprio() == joueurCourant) {
                attendreEtLireAudio(mindLabNom);    //nom du lieu
                delay(1000);
                attendreEtLireAudio(annonceVendu);  //...est vendu pour
                attendreEtLireAudio(prixLieu60);
                delay(2000);
                mindLab.setVendre();
              } else {
                attendreEtLireAudio(annonceNonProprio);  //Vous n'etes pas le proprio du lieu
                delay(1000);
                attendreEtLireAudio(mindLabNom);         //nom du lieu
                delay(1000);
              }
            } else if (uid == laRue.getUid()) {
              if (laRue.getProprio() == joueurCourant) {
                attendreEtLireAudio(laRueNom);      //nom du lieu
                delay(1000);
                attendreEtLireAudio(annonceVendu);  //...est vendu pour
                attendreEtLireAudio(prixLieu100);
                delay(2000);
                laRue.setVendre();
              } else {
                attendreEtLireAudio(annonceNonProprio);  //Vous n'etes pas le proprio du lieu
                delay(1000);
                attendreEtLireAudio(laRueNom);           //nom du lieu
                delay(1000);
              }
            } else if (uid == bde.getUid()) {
              if (bde.getProprio() == joueurCourant) {
                attendreEtLireAudio(bdeNom);        //nom du lieu
                delay(1000);
                attendreEtLireAudio(annonceVendu);  //...est vendu pour
                attendreEtLireAudio(prixLieu100);
                delay(2000);
                bde.setVendre();
              } else {
                attendreEtLireAudio(annonceNonProprio);  //Vous n'etes pas le proprio du lieu
                delay(1000);
                attendreEtLireAudio(bdeNom);             //nom du lieu
                delay(1000);
              }
            } else if (uid == foyer.getUid()) {
              if (foyer.getProprio() == joueurCourant) {
                attendreEtLireAudio(foyerNom);      //nom du lieu
                delay(1000);
                attendreEtLireAudio(annonceVendu);  //...est vendu pour
                attendreEtLireAudio(prixLieu120);
                delay(2000);
                foyer.setVendre();
              } else {
                attendreEtLireAudio(annonceNonProprio);  //Vous n'etes pas le proprio du lieu
                delay(1000);
                attendreEtLireAudio(foyerNom);           //nom du lieu
                delay(1000);
              }
            } else if (uid == gareTroyes.getUid()) {
              if (gareTroyes.getProprio() == joueurCourant) {
                attendreEtLireAudio(gareTroyesNom);  //nom du lieu
                delay(1000);
                attendreEtLireAudio(annonceVendu);   //...est vendu pour
                attendreEtLireAudio(prixLieu200);
                delay(2000);
                gareTroyes.setVendre();
              } else {
                attendreEtLireAudio(annonceNonProprio);  //Vous n'etes pas le proprio du lieu
                delay(1000);
                attendreEtLireAudio(gareTroyesNom);      //nom du lieu
                delay(1000);
              }
            } else {
              attendreEtLireAudio(NonValide);  //carte scanné non valide
              delay(1000);
            }
            break;

          case 4:
            //echange
            Serial.println("Echange");
            break;

          case 5:
            //fin de tour
            Serial.println("Fin de tour");
            attendreEtLireAudio(annoncePasserBadge);
            finTour = true;
            break;
        }
      }
    } else if (uid == joueur2) {
      joueurCourant = "joueur2";
      Serial.println("Joueur 2");
      attendreEtLireAudio(annonceJ2);
      //pareil que joueur 1
    } else if (uid == joueur3) {
      joueurCourant = "joueur3";
      Serial.println("Joueur 3");
      attendreEtLireAudio(annonceJ3);
      //pareil que joueur 1
    } else if (uid == joueur4) {
      joueurCourant = "joueur4";
      Serial.println("Joueur 4");
      attendreEtLireAudio(annonceJ4);
      //pareil que joueur 1
    } else {
      Serial.println("Veuillez scanner un pion joueur.");
    }
  }
}