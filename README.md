# PROJET OS 1T - JUIN 2021

### BELLAALI Abderrachid, POURBAIX Michaël, NECHIFOR Bogdan, HAKIZIMANA Aymar Davy

Partie 1

Ecrire un programme (en C sous Linux !!!) qui :

Reçoit via la ligne de commande une commande avec ses options et/ou paramètres
Passe la commande à un processus fils
Le fils exécute la commande et renvoie les résultats au processus père
Le père affiche les résultats à l’écran.
Comme toujours, lorsqu’on aborde un travail d’une certaine ampleur, il est conseillé de décomposer le problème en plusieurs étapes. (Mélanger les différentes difficultés a souvent pour conséquence qu’on ne sait pas d’où proviennent les erreurs.) Je vous conseillerais ici de diviser le travail en 3 étapes :

Récupérer la commande depuis la ligne de commande. Créer un processus fils et lui envoyer la commande. Le processus fils affiche simplement la commande.

Le fils exécute la commande et affiche lui-même les résultats.

Le fils revoie les résultats au père et le père les affiche.

  Partie 2

Le programme reçoit maintenant en paramètre (via la ligne de commande) un shell script (shell file), c’est-à-dire un fichier contenant une suite de commandes shell. Le fichier

Ne comporte pas de programmation
Se limite à la syntaxe du shell vue au cours (y compris les pipes, redirections, combinaisons de commandes, wildcarts, …) Chacune des commandes du fichier doit être exécutée par un fils, qui renvoie les résultats au père via le pipe et le père affichera les résultats à l’écran (comme pour la partie 1).
Bon courage !

Execution :
gcc main.c -o prog

./prog shellFile.sh
