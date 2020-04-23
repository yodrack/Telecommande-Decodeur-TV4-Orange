# Telecommande Decodeur-TV4-Orange
Méthode pour télécommander le décodeur à partir de Google assistant exemple "Ok Google, zappe sur Arte"

# Objectif du projet :

Avec les enceintes intelligentes comme Google Home ou Google Nest, il est facile d'effectuer des télécommandes par la voix de différents équipements dans la maison.
Cependant, ORANGE ne propose pas de méthode pour télécommander ses décodeurs TV, (certains tutos sur internet fournissent une solution mais après une mise à jour de Orange, ces solutions ne marchent plus.

L'objectif de ce projet est de réaliser une solution qui permet d'envoyer des commandes vocales au décodeur pour mettre sous tension et changer les chaines.

# Principe : 

Le Decodeur TV4 se cpmporte comme un serveur web sur le réseau Wifi interne, il a une adresse IP internet et se commande par une ligne de commande web http://192.168.nnn.mmm:8080/remoteControl/cmd?operation=XXX&key=YYY&mode=ZZZ.

if faut envoyer les codes XXX, YYY et ZZZ au décodeur en fonction de la commande que l'on souhaite effectuer.
avec :

XXX est le code operation

01 : toute opération  ---  10 : demande d'état

YYY est le code de touche :

116 : ON/OFF  
512 à 521 : touche 0 à touche 9

402 : CH+  --- 403 : CH-   
115 : VOL+  ---  114 : VOL-  ---  113 : MUTE

103 : UP  ---  108 : DOWN  ---  105 : LEFT  ---  116 : RIGHT ---  352 : OK

158 : BACK ---   139 : MENU

164 : PLAY/PAUSE ---   168 : FBWD ---   159 : FFWD ---   167 : REC ---  393 : VOD

ZZZ est le numéro_mode :

0 : envoi unique de touche  ---  1 : appui prolongé de touche ---   2 : relacher la touche après un appui prolongé

Le decodeur n'accepte que des commandes issues du réseau interne Wifi, il faut prévoir un système client sur le même réseau qui fournit les commandes au Décodeur. Ce système reçoit ses commandes de Google Assistant par l'intermédiaire d'un MQTT.

## Récupération de l'adresse IP du décodeur :

Aller dans l'interface de gestion de la box "http://192.168.1.1/" et mettre l'adresse du décodeur à la place de 192.168.nnn.mmm
dans la ligne de commande au dessus.


![ip decodeur](https://user-images.githubusercontent.com/64202900/80109429-c4f4b880-857d-11ea-8944-d4afbb3d2994.PNG)

## Création d'un compte Adafruit et créer 4 feeds
bien noter le User_name et le clé key qui servira par la suite

![ada1](https://user-images.githubusercontent.com/64202900/80117684-c3c88900-8587-11ea-91a3-a2ce54f938f8.PNG)

![ada2](https://user-images.githubusercontent.com/64202900/80117786-e35fb180-8587-11ea-8698-38cb205be3b5.PNG)

### bien respecter les noms des feeds
![ada4](https://user-images.githubusercontent.com/64202900/80117950-15711380-8588-11ea-94df-634db9d3f4c3.PNG)

### il n'est pas nécessaire de créer un dashboard mais ça aide pour vérifier des commandes intermédiaires

![ada5](https://user-images.githubusercontent.com/64202900/80118063-32a5e200-8588-11ea-85d2-c7fa8448ff64.PNG)
## Creation d'un compte IFTTT

Aller dans https://ifttt.com/ et créer un compte 
bien noter le User_name qui servira par la suite

## Ensuite créer un nouvelle applet

![ifttt1](https://user-images.githubusercontent.com/64202900/80112156-13f01d00-8581-11ea-8163-270b14d93513.PNG)

## cliquer sur le + pour choisir le service 

![ifttt2](https://user-images.githubusercontent.com/64202900/80112341-4a2d9c80-8581-11ea-9f82-bb095d5859d0.PNG)

## puis choisir le type de service

![ifttt3](https://user-images.githubusercontent.com/64202900/80112669-a85a7f80-8581-11ea-83c0-c6f823d99f43.PNG)

## on choisit le service "dire une phrase avec un nombre"  et saisir les parametres du service

![ifttt4](https://user-images.githubusercontent.com/64202900/80112503-76491d80-8581-11ea-89e6-ead94fcbfa48.PNG)


### Attention
bien choisir les mots employés car Google Assistant est assez capricieux et fait des erreurs sur les termes comme télé, Tv, Video etc...

## appuyer sur le  second + et choisir le service adafruit
![ifttt6a](https://user-images.githubusercontent.com/64202900/80115475-105e9500-8585-11ea-9a87-2075478303aa.PNG)
![ifttt6](https://user-images.githubusercontent.com/64202900/80114260-a98cac00-8583-11ea-924e-d3933b59cc92.PNG)

# SUITE A VENIR ..............
