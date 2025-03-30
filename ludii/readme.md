# Exemple d'interface d'un joueur IA avec Ludii

Jeu : breakthrough 6x3 ou 6x10
Joueur cpp : rand_player_cmd.cpp (qui en réalité est rand_player en ligne de commande)

[prevu avec Ludii-1.3.11.jar]

En supposant que le Ludii-1.3.11.jar est dans le répertoire courant
ou en modifiant la variable LUDII_JAR_FILE dans les scripts bash

Pour jouer avec un programme qui s'excute à distance (remote en anglais)
Modifier le fichier SuperBkPlayerRem.java comme indiqué ci dessous pour jouer avec le joueur cpp

Pour jouer avec un programme qui s'execute localement (pour faire qqes tests)
Modifier le fichier SuperBkPlayer.java comme indiqué ci dessous pour jouer avec le joueur cpp

## Jouer avec le joueur cpp en EXECUTION LOCALE
Aller dans remote_cpp
Pour compiler le joueur rand_player_cmd : sh ./make_bin.sh
Revenir dans le repertoire principal
Verifier le chemin vers l'executable (ligne 32)
Verifier hauteur et largeur de board dans le fichier SuperBkPlayer.java (lignes 33-34 BOARDHEIGHT et BOARDWIDTH)
Construire le jar en utilisant l'IHM Ludii : sh ./make_jar.sh

## Jouer avec le joueur cpp en EXECUTION DISTANTE
Aller dans remote_cpp
Pour compiler le joueur rand_player_cmd : sh ./make_bin.sh
Placer le rand_player_cmd sur la machine distante
Revenir dans le repertoire principal
Verifier le nom de la machine distante (i.e. "remote_host_str" est ok)
Verifier que "remote_player_str" existe sur la machine distante
Verifier que le programme est interrogeable à distance via ssh sans passwd
Verifier hauteur et largeur de board dans SuperBKPlayerRem.java (lignes 37-38 BOARDHEIGHT et BOARDWIDTH)
Construire le jar en utilisant l'IHM Ludii : sh ./make_jar_remote.sh

## Jouer ( en EXECUTION LOCALE comme en EXECUTION DISTANTE)
Pour lancer l'IHM Ludii : sh ./make_run.sh
Puis sélectionner le bon jar (et préciser le bon descripteur de jeu FICHIER.lud si necessaire)


NB : make_run_may_games.sh et RunManyGames.java sont laissés tels que (mais à revoir pour les utiliser avec SuperBkPlayer)
