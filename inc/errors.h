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

/** Liste des codes d'erreurs du programme. Utilisé dans le retour des
 * fonctions et dans la correspondance erreur <=> message. Le code 0 signifie
 * (sauf cas particulier) qu'aucune erreur ne s'est produite. */
enum err_code {
    ERR_NONE = 0,               /*!< Aucune erreur. */
    ERR_BAD_ADRESS,             /*!< Pointeur null ou invalide. */
    ERR_INIT_MISSING_OPTIONS,        /*!< Options nécéssaires au programme
                                   manquantes. */
    ERR_STAT_PRINT,             /*!< Erreur pendant la récupération des
                                   statistiques. */
    ERR_IO_FREAD,                  /*!< Erreur pendant la lecture du fichier. */
    ERR_IO_FREAD_EOF,              /*!< Erreur pendant la lecture du fichier car la
                                   fin à déjà été atteinte. */
    ERR_IO_FWRITE,                 /*!< Erreur pendant l'écriture du fichier. */
    ERR_IO_FCLOSE                  /*!< Erreur pendant la fermeture du fichier. */
};

/* Fonctions publiques ====================================================== */

/**
 * Affiche le message d'erreur sur la sortie d'erreur en fonction du code
 * d'erreur spécifié et renvoie ce même code d'erreur.
 * \param err Code d'erreur.
 * \return Code d'erreur entré.
 */
err_code_e err_print(const err_code_e err);

/**
 * Affiche l'aide sur le flux spécifié et quitte le programme avec le code
 * d'erreur spécifié.
 * \param stream Flux sur lequel écrire l'aide.
 * \param exit_code Code de sortie du programme.
 */
void help_print(FILE * const stream, const int exit_code);

#endif
