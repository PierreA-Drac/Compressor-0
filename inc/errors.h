/** 
 * \file errors.h
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Erreurs.
 * \details Module de gestion des erreurs. Contient les fonctions de gestion des
 * erreurs ainsi que les définitions des codes d'erreurs.
 */

#ifndef __ERRORS_H
#define __ERRORS_H

/* Énumérations publiques =================================================== */

typedef enum err_code err_code_e;

/** Liste les codes d'erreurs du programme. */
enum err_code {
    ERR_MISSING_OPTIONS,        /*!< Options nécéssaires au programme manquantes. */
    ERR_PRINT_STAT              /*!< Erreur pendant la récupération des
                                   statistiques. */
};

/* Fonctions publiques ====================================================== */

/**
 * Affiche le message d'erreur sur la sortie d'erreur en fonction du code
 * d'erreur spécifié.
 * \param err_code Code d'erreur.
 */
void p_error(err_code_e const err);

/**
 * Affiche l'aide sur le flux spécifié et quitte le programme avec le code
 * d'erreur spécifié.
 * \param stream Flux sur lequel écrire l'aide.
 * \param exit_code Code de sortie du programme.
 */
void print_help(FILE * const stream, const int exit_code);

#endif
