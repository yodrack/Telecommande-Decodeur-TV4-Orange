# Telecommande Decodeur-TV4-Orange
Méthode pour télécommander le décodeur à partir de Google assistant exemple "Ok Google, zappe sur Arte"

Objectif du projet :

Avec les enceintes intelligentes comme Google Home ou Google Nest, il est facile d'effectuer des télécommandes par la voix de différents équipements dans la maison.
Cependant, ORANGE ne propose pas de méthode pour télécommander ses décodeurs TV, (certains tutos sur internet fournissent une solution mais après une mise à jour de Orange, ces solutions ne marchent plus.

L'objectif de ce projet est de réaliser une solution qui permet d'envoyer des commandes vocales au décodeur pour mettre sous tension et changer les chaines.

Principe : 

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

Récupération de l'adresse IP du décodeur :

Aller dans l'interface de gestion de la box "http://192.168.1.1/" et mettre l'adresse du décodeur à la place de 192.168.nnn.mmm
dans la ligne de commande au dessus.
