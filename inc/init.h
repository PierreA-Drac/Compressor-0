/** 
 * \file init.h
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Initialisation.
 * \details Module de gestion de l'initialisation du programme, notamment du
 * traitement des arguments.
 */

#ifndef __INIT_H
#define __INIT_H

/* Énumérations publiques ==================================================== */

typedef enum mode mode_e;
typedef enum algo algo_e;

/** Liste les modes possibles de fonctionnement du programme. */
enum mode {
    MODE_NONE,
    MODE_COMPRESS,              /*!< Mode de compression de fichier. */
    MODE_DECOMPRESS             /*!< Mode de décompression de fichier. */
};

/** Liste les algorithmes disponibles pour la compression d'un fichier. */
enum algo {
    ALGO_NONE,
    ALGO_LZMA                   /*!< Valeur de test avant implémentation des
                                   algorithmes. */
};

/* Structures publiques ===================================================== */

typedef struct prog_info prog_info_s;

/** Informations sur l'instance du programme. */
struct prog_info {
    char stat;                  /*!< Flag, afficher les statistiques. */
    mode_e mode;                /*!< Mode d'exécution. */
    algo_e algo;                /*!< Algorithme à utiliser. */
    char *s_prog_name;          /*!< Nom du programme. */
    char *s_input_file;         /*!< Nom du fichier entrant. */
    char s_output_file[256];    /*!< Nom du fichier sortant. */
};

/* Fonctions publiques ====================================================== */

/**
 * Initialise le programme en récupérant les arguments sur la ligne de commande,
 * vérifie si les paramètres obligatoires sont passés et met les paramètres
 * non-spécifié par défaut. Quitte le programme avec EXIT_FAILURE si une erreur
 * survient.
 * \param argv Nombre d'argument (>= 1)
 * \param argv Tableau de chaîne de caractères contenant les arguments.
 * \return program_info_s contenant les informations nécéssaire au
 * fonctionnement du programme.
 */
prog_info_s init_prog(const int argc, char *const *argv);

#endif
