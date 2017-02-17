/** 
 * \file io.c
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Entrées/sorties.
 * \details Module de gestion des entrées/sorties, sur le disque ou sur les
 * entrée/sortie standards (hormis les erreurs).
 */

#include <stdio.h>
#include <stdlib.h>
/*#include <string.h>*/
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/time.h>
#include "io.h"
#include "errors.h"

/* Variables globales privées =============================================== */

/* Contiendra le temps CPU à l'initialisation du programme. */
static clock_t t;

/* Fonctions privées ======================================================== */

/* Affiche la taille d'un fichier sur la sortie standard. Si succès renvoie 0,
 * si erreur renvoie -1. */
static int print_file_stat(const char *s_pathname)
{
    /* Récupération des statistiques. */
    struct stat file_stat;
    if (stat(s_pathname, &file_stat))
        return perror("stat in print_file_stat"), -1;
    /*[> Récupération du nom du fichier sans le chemin. <] */
    /*const char *s_ptr_tmp = strrchr(s_pathname, '/'); */
    /*s_pathname = s_ptr_tmp ? s_ptr_tmp + 1 : s_pathname; */
    /* Affichage des informations. */
    printf("Taille de %s : %ld kB.\n", s_pathname, file_stat.st_size / 1000);
    return 0;
}

/* Affiche les statistiques d'exécution du programme sur la sortie standard. Si succès
 * renvoie 0, si erreur renvoie -1. */
static int print_prog_stat()
{
    /* Temps CPU utilisé (mode user et kernel). */
    t = clock() - t;
    printf("Temps CPU (user & kernel) du programme : %f s.\n",
           ((float)t) / CLOCKS_PER_SEC);
    /* Consommation en mémoire. */
    struct rusage rus;
    if (getrusage(RUSAGE_SELF, &rus))
        return perror("getrusage in print_prog_stat"), -1;
    printf("Espace mémoire utilisé (resident set size) : %ld kB.\n",
           rus.ru_maxrss);
    return 0;
}

/* Fonctions publiques ====================================================== */

compress_file_s init_cmp_file(const char *s_filepath_in,
                              const char *s_filepath_out)
{
    compress_file_s cf;
    cf.p_stream = NULL;
    if (!(cf.fp_in = fopen(s_filepath_in, "rb")) ||
        !(cf.fp_out = fopen(s_filepath_out, "wb"))) {
        perror("fopen in init_cmp_file");
        exit(EXIT_FAILURE);
    }
    return cf;
}

void init_stat()
{
    t = clock();
}

int print_stat(const char *s_filepath_in, const char *s_filepath_out)
{
    if (print_file_stat(s_filepath_in) || print_file_stat(s_filepath_out))
        return -1;
    if (print_prog_stat())
        return -1;
    return 0;
}
