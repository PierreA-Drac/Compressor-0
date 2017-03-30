/** 
 * \file init.c
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Initialisation.
 * \details Module de gestion de l'initialisation du programme, notamment du
 * traitement des arguments.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <errno.h>
#include <sys/stat.h>
#include "init.h"
#include "errors.h"
#include "common.h"

/* Macros-constantes privées ================================================ */

#define DEF_OUT_PATH "out/"

/* Fonctions privées ======================================================== */

/* Initialise une variable de type prog_info_s à 0. */
static prog_info_s init_prog_info()
{
    prog_info_s pi;
    pi.stat = FALSE;
    pi.mode = MODE_NONE;
    pi.algo = ALGO_NONE;
    pi.s_prog_name = NULL;
    pi.s_input_file = NULL;
    pi.s_output_file[0] = '\0';
    return pi;
}

/* Récupère les arguments en ligne de commande et les stockes dans P. Quitte le
 * programme si une erreur survient. */
static prog_info_s get_args(prog_info_s pi, const int argc, char *const *argv)
{
    /* Stockage de l'argument en cours de traitement. */
    char curr_arg = 0;

    /* Chaîne de caractère contenant les lettres courtes d'options. */
    const char *s_short_options = "hcdsi:o:";

    /* Structure définissant les options longues. */
    const struct option long_options[] = {
        {"help", 0, NULL, 'h'},
        {"compress", 0, NULL, 'c'},
        {"decompress", 0, NULL, 'd'},
        {"statistics", 0, NULL, 's'},
        {"input", 1, NULL, 'i'},
        {"output", 1, NULL, 'o'},
        {"RLE", 0, NULL, ALGO_RLE},
        {NULL, 0, NULL, 0}
    };

    pi.s_prog_name = argv[0];
    do {
        curr_arg = getopt_long(argc, argv, s_short_options, long_options, NULL);
        switch (curr_arg) {
            case 'c':
                pi.mode = MODE_COMPRESS;
                break;
            case 'd':
                pi.mode = MODE_DECOMPRESS;
                break;
            case 's':
                pi.stat = TRUE;
                break;
            case 'i':
                pi.s_input_file = optarg;
                break;
            case 'o':
                strcat(pi.s_output_file, optarg);
                break;
            case ALGO_RLE:
                pi.algo = ALGO_RLE;
                break;
            case 'h':
                help_print(stdout, EXIT_SUCCESS);
            case '?':          /* Option non reconnue. */
                help_print(stdout, EXIT_FAILURE);
            case -1:           /* Lecture terminée. */
                break;
            default:           /* Erreur dans la fonction. */
                abort();
        }
    } while (curr_arg != -1);
    return pi;
}

/* Fonctions publiques ====================================================== */

prog_info_s init_prog(const int argc, char *const *argv)
{
    /* Initialisation du stockage des informations finales. */
    prog_info_s pinfo = init_prog_info();

    /* Récupération des arguments bruts. */
    pinfo = get_args(pinfo, argc, argv);

    /* Test que les options obligatoires ont bien étés passées. */
    if ((pinfo.mode == MODE_COMPRESS && !pinfo.algo) || !pinfo.mode ||
        !pinfo.s_input_file) {
        err_print(ERR_INIT_MISSING_OPTIONS);
        help_print(stderr, EXIT_FAILURE);
    }

    /* Met un nom par défaut au fichier de sortie si non spécifié. */
    if (!pinfo.s_output_file[0]) {
        assert(pinfo.s_input_file);
        /* Ajoute le répertoire du programme suivis du répertoire de sortie par
         * défaut (spécifié par le define DEF_OUT_PATH). */
        char *s_ptr_tmp = strrchr(pinfo.s_prog_name, '/') + 1;
        strncat(pinfo.s_output_file, pinfo.s_prog_name,
                strlen(pinfo.s_prog_name) - strlen(s_ptr_tmp));
        strcat(pinfo.s_output_file, DEF_OUT_PATH);
        /* Crée le répertoire de sortie par défaut si besoin. */
        if (mkdir(pinfo.s_output_file, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                  S_IROTH) && errno != EEXIST) {
            perror("mkdir for initialization");
            exit(EXIT_FAILURE);
        }
        /* Ajoute le nom du fichier source. */
        s_ptr_tmp = strrchr(pinfo.s_input_file, '/');
        s_ptr_tmp = s_ptr_tmp ? s_ptr_tmp + 1 : pinfo.s_input_file;
        strcat(pinfo.s_output_file, s_ptr_tmp);
    }
    assert(pinfo.s_output_file[0]);
    return pinfo;
}
