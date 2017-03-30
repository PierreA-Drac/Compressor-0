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

/* Types publiques ========================================================== */

typedef enum err_code err_code_e;

/* Variales globales ======================================================== */

/**
 * Variable mise à la disposition des fonctions pour y inscrire leur
 * code d'erreur.
 */
err_code_e CMP_err;

/* Énumérations publiques =================================================== */

/**
 * Liste des codes d'erreurs du programme. Utilisé dans le retour des
 * fonctions et dans la correspondance erreur <=> message. Le code 0 signifie
 * (sauf cas particulier) qu'aucune erreur ne s'est produite.
 */
enum err_code {
    ERR_OTHER = -1,             /*!< Erreur quelconque. */
    ERR_NONE = 0,               /*!< Aucune erreur. */
    ERR_BAD_ADRESS,             /*!< Pointeur null ou invalide. */
    ERR_INIT_MISSING_OPTIONS,   /*!< Options nécéssaires au programme
                                   manquantes. */
    ERR_STAT,                   /*!< Erreur pendant la récupération des
                                   statistiques. */
    ERR_IO_FREAD,               /*!< Erreur pendant la lecture du fichier. */
    ERR_IO_FREAD_EOF,           /*!< Erreur pendant la lecture du fichier car la
                                   fin à déjà été atteinte. */
    ERR_IO_FWRITE,              /*!< Erreur pendant l'écriture du fichier. */
    ERR_IO_FCLOSE,              /*!< Erreur pendant la fermeture du fichier. */
    ERR_COMPRESSION_FAILED,     /*!< Erreur durant la compression. */
    ERR_DECOMPRESSION_FAILED    /*!< Erreur durant la décompression. */
};

/* Fonctions publiques ====================================================== */

/**
 * Affiche le message d'erreur sur la sortie d'erreur en fonction du code
 * d'erreur spécifié.
 * \param err Code d'erreur.
 */
void err_print(const err_code_e err);

/**
 * Affiche l'aide sur le flux spécifié et quitte le programme avec le code
 * d'erreur spécifié.
 * \param p_stream Flux sur lequel écrire l'aide.
 * \param exit_code Code de sortie du programme.
 * \param s_name Pointeur vers la chaîne de caractère contenant le nom du
 * programme.
 */
void help_print(FILE * const p_stream, const int exit_code, const char *s_name);

#endif
