/**************************************************************************************
 **************************************************************************************
 ************* Interface entre Google Assistant et la box TV orange *******************
 **************utilisant adafrruit ****************************************************
 **************************************************************************************
 * ************Yodrack Avril 2020 *******************************************
*/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <WiFiUdp.h>
#include <SPI.h>
#include <ESP8266HTTPClient.h>

/*********************************** parametres du wifi *********************************/
#define WLAN_SSID       "votre SSID ici"
#define WLAN_PASS       "votre mot de passe ici"
 
/*********************************** parametres du serveur Adafruit  *********************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "votre nom de compte"              // mettre l'identifiant du compte
#define AIO_KEY         "votre cle de compte"  // mettre la clé du compte


#define SECONDE 1000                              // en milli secondes

#define appui_unique 0          //0 : envoi unique de touche
#define appui_long 1            //1 : appui prolongé de touche
#define relacher 2              //2 : relacher la touche après un appui prolongé


String url_livebox  = "l'adresse ip du decodeur";       // IP de la box TV a récuper sur le reseau interne à mettre en Ip fixe
int port = 8080;                             // port de commande de la box TV

/****************************** chaines de commande de la box TV *******************************/
String chaine_0 = "/remoteControl/cmd?operation=01&key=512&mode=0";       //key est le code interne à la box pour la chaine
String chaine_0M2 = "/remoteControl/cmd?operation=01&key=512&mode=2";
String chaine_1 = "/remoteControl/cmd?operation=01&key=513&mode=0";
String chaine_2 = "/remoteControl/cmd?operation=01&key=514&mode=0";
String chaine_3 = "/remoteControl/cmd?operation=01&key=515&mode=0";
String chaine_4 = "/remoteControl/cmd?operation=01&key=516&mode=0";
String chaine_5 = "/remoteControl/cmd?operation=01&key=517&mode=0";
String chaine_6 = "/remoteControl/cmd?operation=01&key=518&mode=0";
String chaine_7 = "/remoteControl/cmd?operation=01&key=519&mode=0";
String chaine_8 = "/remoteControl/cmd?operation=01&key=520&mode=0";
String chaine_9 = "/remoteControl/cmd?operation=01&key=521&mode=0";

String allumer_eteindre = "/remoteControl/cmd?operation=01&key=116&mode=0";

String monter_son = "/remoteControl/cmd?operation=01&key=115&mode=0";     //115 : VOL+

String baisser_son = "/remoteControl/cmd?operation=01&key=114&mode=0";     //114 : VOL-

#define DELAI_CHIFFRE2 300

/************ Création des structures de données ********************************************/
WiFiClient client;   
HTTPClient http;
 
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe nom_chaine = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/nom_chaine");
Adafruit_MQTT_Subscribe numero_chaine = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/numero_chaine");
Adafruit_MQTT_Subscribe power  = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/power");
Adafruit_MQTT_Subscribe son    = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/son");

/************ tableau des noms de chaines par numero d'ordre ********************************************
 ********************** "0" pour les chaines payantes non accessibles ***********************************
*********************** le nom doit correspondre à l'emplacement du numéro de chaine dans le tableau*****
*/
char* tab_nom_chaine[] = {"mire","tf 1","france 2","france 3","canal +","france 5","m 6","arte","C 8","W 9",
                          "TMC","TFX","NRJ","LCP","France 4","BFM","cnews","cstar","Gulli","France Ô",
                          "TF 1 serie film","l ' équipe","6 ter","RMC","RMC découverte","chérie","LCI","France Info"," 0"," 0",
                          "0","à la maison","0","0","TV5_MONDE","Teva","RTL 9","Paris Première","0","0",
                          "0","0","0","0","0","0","0","0","0","0",
                          "0","0","0","0","0","TCM","Paramount 1","Paramount 2","action","0","0" } ;

char* on_off [] = {"en marche", "arrêt"} ;

int numero_choisi;
int code_touche ;
int code_mode ;
int code_retour ; 

void MQTT_connect();
 
boolean sous_tension ;
int volume;

/************************************************************************************
********************* Début du code setup *******************************************
*************************************************************************************/
void setup() {
  Serial.begin(115200);
  Serial.println();

  connect_wifi();
  delay (100); 

  mqtt.subscribe(&nom_chaine);
  mqtt.subscribe(&numero_chaine);
  mqtt.subscribe(&power);
  mqtt.subscribe(&son); 
}  /////////////////////////////// fin du setup ///////////////////////////////


/************************************************************************************
********************* Début du code principal****************************************
*************************************************************************************/
void loop() {
  MQTT_connect();
  numero_choisi = 0;  // raz du numero de chaine  du son et power
  sous_tension = false;
  volume = 0; 
  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5*SECONDE))) {            // tempo de 5 secondes
   
     if (subscription == &nom_chaine) {                    // si on a reçu un nom de chaine
       String chaine_lue = (char *)nom_chaine.lastread ; // pour voir le texte que l'on a reçu
 //      Serial.println(chaine_lue);
       for (int index =0; index<=60; index++) {          // recherche du numero de la chaine 
         if (strcmp((char *)nom_chaine.lastread, tab_nom_chaine[index]) == 0 ) {  // comparaison des chaines entre reçu et tableau
            numero_choisi = index;                       // le nom doit correspondre au numéro dans le tableau
            break;
         } 
       }
     }
     if (subscription == &numero_chaine) {                    // si on a reçu un numero de chaine
       numero_choisi = atoi((char *)numero_chaine.lastread);  // on lit des ascii donc convertir en intéger
       Serial.print("commande numero de chaine reçu ");  Serial.println(numero_choisi);  
     }
    
     if (subscription == &power) {                    // si on a reçu l'ordre de mise sous tension ou hors tension
       if (strcmp((char *)power.lastread, on_off[0]) == 0 ) {  // test si en marche
            Serial.print("on a trouvé en marche   ");
            sous_tension = true ;
       }
       if (strcmp((char *)power.lastread, on_off[1]) == 0 ) {  // test si arret
            Serial.print("on a trouvé arret   ");
            sous_tension = true ;
       }
     }

     if (subscription == &son) {                    // si on a reçu une commande de son
        volume = atoi((char *)son.lastread);
        Serial.print("commande volume ");   Serial.println(volume);
     }
     if( subscription == 0) {  Serial.println("pas de commande ");  // on a rien reçu de correct
      numero_choisi = 0;
      }

 }

/*********************** on ne programme que les chaines de 1 à 59 *********************************/

 if (numero_choisi >=1  && numero_choisi <10 ) { commande0_tele(numero_choisi); }
 if (numero_choisi >=10 && numero_choisi <20 ) { commande1_tele(numero_choisi); }
 if (numero_choisi >=20 && numero_choisi <30 ) { commande2_tele(numero_choisi); }
 if (numero_choisi >=30 && numero_choisi <40 ) { commande3_tele(numero_choisi); }
 if (numero_choisi >=40 && numero_choisi <50 ) { commande4_tele(numero_choisi); }
 if (numero_choisi >=50 && numero_choisi <60 ) { commande5_tele(numero_choisi); }

/******************* la solution de commande marche arret n'est pas terrible car flip/ flop **************/
 if (sous_tension) {
   http.begin(url_livebox, port, allumer_eteindre );
   code_retour = http.GET();                                        //envoi de la commande à l'url de la box TV
 }
/******************* la solution de commande de son n'est pas terrible mais j'ai pas trouvé mieux **************/
 if (volume > 0) {                                         
      for (int vol = 16; vol>= 0; vol-- ){                // on commence par ramener le son a zero car volume max = 16
         http.begin(url_livebox, port, baisser_son );
         code_retour = http.GET();                        //envoi de la commande à l'url de la box TV
         delay(50);
      }
      for (int vol = 1; vol <= volume*2; vol++ ){         // on envoi le son au numero choisi 
                                                          // voir pourquoi il faut multiplier le nombre par 2
         http.begin(url_livebox, port, monter_son );
         code_retour = http.GET();                        //envoi de la commande à l'url de la box TV
         Serial.print("commande monter volume ");   Serial.println(volume);
         delay(50);
      }
 }
}          // >>>>>>>>>>>>>>>>> fin de loop >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void commande0_tele(int num) {
     selectionne_unite (num);
     }

void commande1_tele(int num) {
     if (num == 10) {
        http.begin(url_livebox, port, chaine_0M2 );             // préparation commande à deux chiffres
        code_retour = http.GET();        
        delay (DELAI_CHIFFRE2);
        selectionne_unite (1);                                  // premier chiffre des dizaines soit 1
        delay (DELAI_CHIFFRE2);
        selectionne_unite (0);                                  // second chiffre soit 0
     } else {                                                   // de 11 a 19
        selectionne_unite (1);                                  // premier chiffre des dizaines soit 1
        delay (DELAI_CHIFFRE2);
        selectionne_unite (num-10);                             // second chiffre 1 à 9
     } 
}

void commande2_tele(int num) {
     if (num == 20) { 
        http.begin(url_livebox, port, chaine_0M2 );
        code_retour = http.GET();                     
        delay (DELAI_CHIFFRE2);
        selectionne_unite (2);                                  // premier chiffre des dizaines soit 2
        delay (DELAI_CHIFFRE2);
        selectionne_unite (0);                                  // second chiffre soit 0
     } else {                                                   // de 21 a 29
        selectionne_unite (2);                                  // premier chiffre des dizaines soit 2
        delay (DELAI_CHIFFRE2);
        selectionne_unite (num-10);                             // second chiffre 1 à 9
     }
}
void commande3_tele(int num) {
     if (num == 30) {
        http.begin(url_livebox, port, chaine_0M2 );
        code_retour = http.GET();                     
        delay (DELAI_CHIFFRE2);
        selectionne_unite (3);                                   // premier chiffre des dizaines soit 3
        delay (DELAI_CHIFFRE2);
        selectionne_unite (0);                                   // second chiffre soit 0
     } else {                                                    // de 31 a 39
        selectionne_unite (3);                                   // premier chiffre des dizaines soit 3
        delay (DELAI_CHIFFRE2);
        selectionne_unite (num-10);                              // second chiffre 1 à 9
     }
}
void commande4_tele(int num) {
     if (num == 40) {
        http.begin(url_livebox, port, chaine_0M2 );
        code_retour = http.GET();                     
        delay (DELAI_CHIFFRE2);
        selectionne_unite (4);                                   // premier chiffre des dizaines soit 4
        delay (DELAI_CHIFFRE2);
        selectionne_unite (0);                                   // second chiffre soit 0
     } else {                                                    // de 41 a 49   
        selectionne_unite (4);                                   // premier chiffre des dizaines soit 4
        delay (DELAI_CHIFFRE2);
        selectionne_unite (num-10);                              // second chiffre 1 à 9
     }
}
void commande5_tele(int num) {
     if (num == 50) {
        http.begin(url_livebox, port, chaine_0M2 );
        code_retour = http.GET();                     
        delay (DELAI_CHIFFRE2);
        selectionne_unite (5);                                   // premier chiffre des dizaines soit 5
        delay (DELAI_CHIFFRE2);
        selectionne_unite (0);                                   // second chiffre soit 0
     } else {                                                    // de 51 a 59
        selectionne_unite (5);                                   // premier chiffre des dizaines soit 5
        delay (DELAI_CHIFFRE2);
        selectionne_unite (num-10);                              // second chiffre 1 à 9
     }
}
void selectionne_unite (int num) {                                // envoi de 0 a 9
     switch (num) {
      case 0: 
           http.begin(url_livebox, port, chaine_0 ); break ;   
      case 1: 
           http.begin(url_livebox, port, chaine_1 ); break ;   
      case 2: 
           http.begin(url_livebox, port, chaine_2 ); break ;   
      case 3: 
           http.begin(url_livebox, port, chaine_3 ); break ;   
      case 4: 
           http.begin(url_livebox, port, chaine_4 ); break ;   
      case 5: 
           http.begin(url_livebox, port, chaine_5 ); break ;   
      case 6: 
           http.begin(url_livebox, port, chaine_6 ); break ;   
      case 7: 
           http.begin(url_livebox, port, chaine_7 ); break ;
      case 8: 
           http.begin(url_livebox, port, chaine_8 ); break ;
      case 9: 
           http.begin(url_livebox, port, chaine_9 ); break ;
      }
      code_retour = http.GET();                                             // envoi de la commande sur le GET            
}



void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("en cours de connexion MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("nouvel essai de connexion au MQTT dans 5 secondes");
       mqtt.disconnect();
       delay(5*SECONDE);
       retries--;
       if (retries == 0) {
         return ;                                 // ça n'a pas marche, on plante, on recommence
       }
  }
  Serial.println("MQTT Connecté!");
}


/******************  connexion WIFI *************************************************
*/
void connect_wifi (){
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {   // attente de la connexion
    delay(500);
    Serial.print(".");  } 
    Serial.println(""); Serial.print("Connected to "); Serial.println(WLAN_SSID); Serial.print("IP address: "); Serial.println(WiFi.localIP());
}
