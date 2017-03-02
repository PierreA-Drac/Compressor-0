/**
 * \file stats.c
 * \author AYOUB Pierre
 * \date 20 février 2017
 *
 * \brief Statistiques.
 * \details Module de génération et d'affichage des statistiques sur le
 * processus et les fichiers.
 */

#include <stdio.h>
/*#include <string.h>*/
#include <time.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include "stats.h"

/* Variables globales privées =============================================== */

/* Contiendra le temps CPU à l'initialisation du programme. */
static clock_t t;

/* Fonctions privées ======================================================== */

/* Affiche la taille d'un fichier sur la sortie standard. Si succès renvoie 0,
 * si erreur renvoie -1. */
static int stat_print_file(const char *s_pathname)
{
    /* Récupération des statistiques. */
    struct stat file_stat;
    if (stat(s_pathname, &file_stat))
        return perror("stat in stat_print_file"), -1;
    /*[> Récupération du nom du fichier sans le chemin. <] */
    /*const char *s_ptr_tmp = strrchr(s_pathname, '/'); */
    /*s_pathname = s_ptr_tmp ? s_ptr_tmp + 1 : s_pathname; */
    /* Affichage des informations. */
    printf("Taille de %s : %ld kB.\n", s_pathname, file_stat.st_size / 1000);
    return 0;
}

/* Affiche les statistiques d'exécution du programme sur la sortie standard. Si succès
 * renvoie 0, si erreur renvoie -1. */
static int stat_print_prog()
{
    /* Temps CPU utilisé (mode user et kernel). */
    t = clock() - t;
    printf("Temps CPU (user & kernel) du programme : %f s.\n",
           ((float)t) / CLOCKS_PER_SEC);
    /* Consommation en mémoire. */
    struct rusage rus;
    if (getrusage(RUSAGE_SELF, &rus))
        return perror("getrusage in stat_print_prog"), -1;
    printf("Espace mémoire utilisé (resident set size) : %ld kB.\n",
           rus.ru_maxrss);
    return 0;
}

/* Fonctions publiques ====================================================== */

void stat_init()
{
    t = clock();
}

int stat_print(const char *s_filepath_in, const char *s_filepath_out)
{
    if (stat_print_file(s_filepath_in) || stat_print_file(s_filepath_out))
        return -1;
    if (stat_print_prog())
        return -1;
    return 0;
}
