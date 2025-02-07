# TP Recherche IDS (Iterative Deepening Search)  

Définir :
- Une fonction heuristique pour la variante classique 
- Une recherche IDS en temps limité dans un fichier ids_player.cpp

Présenter les points particuliers de votre solution dans 3 semaines.

Fichiers fournis pour ce TP :
* mybt.h définit les structures bt_piece_t, bt_move_t et bt_t
* bt_piece_t qui modélise une piece
* bt_move_t qui modélise un coup
* bt_t qui modélise le plateau et ses contraintes
* rand_player.cpp est un joueur aléatoire qui supporte le breakthrough text protocol btp
* le protocol btp permet de controler un programme pour jouer a breakthrough
* game1.txt est un exemple de fichier de commandes btp
* run_many_games.pike est un programme pike permettant de faire jouer ensemble des programmes supportant le btp
* Makefile permet de compiler le rand_player
* mk_stats.sh permet de lancer plusieurs parties, stocker les logs et les stats

Pour le moment, on posera pour contrainte de répondre le coup à jouer en 1 sec
Au delà de 1 sec, l'absence de réponse sera considérée comme un abandon
