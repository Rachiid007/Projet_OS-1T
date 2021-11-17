/*  Projet C 2021 - 1TM-1   BELLAALI Abderrachid
Rajouter : A-S Stat pour avoir la taille exact du fichier !
problème avec la boucle qui boucle autant qu'il ya de fichier Shell, ex : si il ya "ls -l", elle va pas prendre le "-l"
dans le while on boucle temps qu'il y a quelque chose dans le fd
quand on arréte de Lire ? quand il y a plus rien : la variable vaut 0 ( vu que boolean de 0 c'est faux ) => donc on sort du while
A vérifier : je pense que le realloc() n'est pas bien fait, dans la fonction my_popen() : aucun printf() ne s'affiche ! et encore des petit truc à vérifier...
*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


void my_popen(char ligne[]); // prototype de my_popen()


#define SIZE_BUFFER 5

int main (int argc, char* argv[]) {

    ssize_t size_read; // ça va contenir cmb de charactères ont été lus


    if (argc == 1) { // au moins un argument
        printf("Pas assez d'arguments.\n");
        exit(0);
    }


    for (int j = 1; j < argc; ++j) // boucle tant qu'il ya des shellFile
    {
        //printf("%s\n", argv[j]);

        int fd = open(argv[j], O_RDONLY); // ouvrir le fichier shell

        if (fd == -1) { // vérifier que l'ouverture a marché
            perror("Probleme avec Open()");
            exit(EXIT_FAILURE);
        }

        /* Calculer les tailles des Fichiers */
        struct stat sb;

        if (stat(argv[j], &sb) == -1) {
            perror("Probleme avec Stat() !\n");
            exit(EXIT_FAILURE);
        }
        /* ----- ----- ----- ----- ----- */


        /* Lire le Fichier */
        char *file_contents = calloc(1, sb.st_size);
        if (file_contents == NULL) {
            perror("Problème avec Calloc() !");
            exit(EXIT_FAILURE);
        }

        size_read = read(fd, file_contents, sb.st_size);

        if (size_read == -1) { // si erreur lecture
            perror("Probleme avec Read()");
            exit(1);
        }
        /* ----- ----- ----- ----- ----- */


        if(close(fd) == -1) { // on a fini de travailler avec le fichier shell
            perror("Fermeture du fichier");
            exit(1);
        }

        //contenu[size_read] = '\0'; // read n'ajoute pas \0 à la fin du string, donc on doit l'ajouter pour que le string soit bien formé


        char * ligne = strtok ( file_contents, "\n" );
        while ( ligne != NULL ) {

            my_popen(ligne);

            // On demande le token suivant.
            ligne = strtok ( NULL, "\n" );
        }
    }


    exit(0);
}



void my_popen(char ligne[])
{
    pid_t pid;
    int pipeFd[2]; //file descriptors pour intéragir avec le pipe (0 lecture, 1 écriture)


    if (pipe(pipeFd) == -1) { //créer le pipe et vérifier qu'il n'y a pas d'erreur
        perror("Problème création du Pipe().");
        exit(1);
    }

    pid = fork(); // créer le processus fils

    if( pid == -1 ) {
        perror("Problème avec le Fork() !");
        exit(1);
    }

    else if ( pid >= 1 ) //dans le PARENT :
    {
        if (close(pipeFd[1]) == -1) { //le parent lis, donc on ferme celui de ecriture
            perror("Problème avec fermeture Pipe() du parent !");
            exit(1);
        }

        // la ont va lire ce qui est dans le pipe
        char buffer[SIZE_BUFFER]; // la variable dans laquel je vais stocker ce que j'ai lu
        ssize_t quantiteLu;

        // enregistre le resultat du read dans la variable...
        quantiteLu = read(pipeFd[0], buffer, SIZE_BUFFER);
        while(quantiteLu)
        {
            if (quantiteLu == -1){
                perror("Probleme avec le Read() !");
                exit(1);
            }

            if (write(STDOUT_FILENO, buffer, quantiteLu) == -1){ // ecrit dans STDOUT, quantiteLu (var) de caractere d'buffer
                perror("Probleme avec le Write() !");
                exit(1);
            }
            quantiteLu = read(pipeFd[0], buffer, SIZE_BUFFER);
        }


        if (close(pipeFd[0]) == -1) {// on ferme le pipe côté parent
            perror("Problème fermeture Pipe() du parent !");
            exit(1);
        }

        return; // psk void
    }


    else //dans le FILS :
    {
        char ** paramsList;

        int i = 0; // defini ici pour avoir accées en général !

        if (close(pipeFd[0]) == -1) { //on écrit dans le fils, donc on ferme le pipe en lecture
            perror("Problème fermeture Pipe() du fils !");
            exit(1);
        }


        char * mot = strtok ( ligne, " " );
        while ( mot != NULL ) // tant qu'il ya des mots
        {

            if (i == 0) {

                paramsList = (char**) calloc(2, sizeof(char*));
                if (paramsList == NULL)
                {
                    perror("Problème avec Calloc() :");
                    exit(1);
                }
                paramsList[0] = mot;
            }

            else {
                paramsList = realloc(paramsList, (i + 2) * sizeof(char*));
                paramsList[i] = mot;
            }

            paramsList[i] = mot;

            // On demande le token suivant
            mot = strtok ( NULL, " " );

            ++i;
        }


        paramsList[i] = NULL; //les fonctions execv demandent que la liste de paramètres termine par NULL.


        // si pas d'erreur avec dup2 -- file numéro 0
        if (dup2(pipeFd[1], STDOUT_FILENO) == -1) { // rediriger tous ce qui est dans le STDOUT dans pipeFd[1]
            perror("Probleme avec dup2() !");
            exit(1);
        }

        if (close(pipeFd[1]) == -1) { //on ferme le pipe côté fils
            perror("Problème fermeture Pipe() du fils !");
            exit(1);
        }


        // toujours exec A LA FIN !!!!
        if (execvp(paramsList[0], paramsList) == -1) {
            perror("Probleme avec Execvp() !");
            exit(1); // quite le programme mais ya une erreur
        }

        // pas de free(), psk quand un process meurt, la memoire est désalouée automatiquement !

        exit(0); // quite le programme et ya pas d'erreur
    }

}


/*  Crée un ou plusieurs ShellFile (.sh) -> possible qu'il faut leurs rajouter la pérmission x (executer) si pas par défaut !
Qui contienne des commande simple comme :
echo "Hello word"
ls -l
who
man
------------------------
pour compiler : gcc shellExec2.c -o shellExec
pour executer le programme : ./shellExec monShell.sh monShell2.sh
*/
