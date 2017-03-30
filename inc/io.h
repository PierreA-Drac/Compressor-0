/** 
 * \file io.h
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Entrées/sorties.
 * \details Module de gestion des entrées/sorties sur le disque.
 */

#ifndef __IO_H
#define __IO_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "common.h"

/* Macro-constantes publiques =============================================== */

/** Taille d'un bloc en byte. */
#define BLOCK_SIZE sizeof(block_t)
/** Longueur d'un bloc en bit. */
#define BLOCK_LENGHT BLOCK_SIZE*CHAR_BIT

/* Macro-fonctions publiques ================================================ */

/**
 * Récupère un bit à une position donnée.
 * Macro-fonction pour contourner le typage.
 * \param src Variable source contenant le bit à récupérer.
 * \param bit Entier dans laquelle sera stocké le bit (0 | 1).
 * \param pos Entier indiquant la position du bit à récupérer.
 */
#define GET_BIT(src, bit, pos) do { \
    assert((unsigned int)(pos) < sizeof(src)*CHAR_BIT); \
    (bit) = ((src) >> (pos)) & 0b1; \
    } while (0)

/**
 * Met un bit à une position donnée.
 * Macro-fonction pour contourner le typage.
 * \param dest Variable de destination du bit à ajouter.
 * \param bit Entier contenant le bit (0 | 1).
 * \param pos Entier indiquant la position de l'ajout du bit.
 */
#define PUT_BIT(dest, bit, pos) do { \
    assert((unsigned int)(pos) < sizeof(dest)*CHAR_BIT); \
    assert((bit) == 0 || (bit) == 1); \
    (dest) = ((dest) | ((uint64_t)(bit) << (pos))) & ~((~(uint64_t)(bit) & 0b1) << (pos)); \
    } while (0)

/* Types publiques ========================================================== */

typedef uint64_t block_t;

/* Structures publiques ===================================================== */

typedef struct cmp_file cmp_file_s;

/* Fonctions publiques ====================================================== */

/**
 * Initialise les flux vers les fichiers entrant et sortant, initialise la
 * structure pour qu'elle soit prête à être utilisée et assure les routines de
 * détection d'erreurs. Quitte le programme avec "EXIT_FAILURE" si une erreur
 * survient.
 * \param filepath_in Chemin vers le fichier entrant.
 * \param filepath_out Chemin vers le fichier sortant.
 * \return Pointeur vers la structure d'un fichier prêt à être traité.
 */
cmp_file_s *cmpf_open(const char *s_filepath_in, const char *s_filepath_out);

/**
 * Lit un bloc de donnée du fichier entrant depuis son buffer, et le stocke dans
 * un bloc.
 * \param cf Fichier source.
 * \param b Bloc à remplir.
 * \return 0 sur un succès, -1 sur une erreur et positionne "CMP_err" sur
 * l'erreur correspondante.
 * \error ERR_BAD_ADRESS si un pointeur est incorrect.
 * \error ERR_IO_FREAD si une erreur survient lors de la lecture.
 * \error ERR_IO_FREAD_EOF si on à déjà lu la fin du fichier.
 */
int cmpf_get_block(cmp_file_s * cf, block_t * b);

/**
 * Écris un bloc de donnée sur un buffer d'un fichier sortant.
 * \param cf Fichier sortant.
 * \param b Bloc à écrire.
 * \return 0 sur un succès, -1 sur une erreur et positionne "CMP_err" sur
 * l'erreur correspondante.
 * \error ERR_BAD_ADRESS si un pointeur est incorrect.
 * \error ERR_IO_FWRITE si une erreur survient lors de l'écriture.
 */
int cmpf_put_block(cmp_file_s * cf, block_t b);

/**
 * Vide le buffer d'écriture sur le disque, ferme les flux vers les fichiers
 * entrant et sortant, et libère la mémoire de la structure.
 * \param cf Fichier à fermer.
 * \return 0 sur un succès, ou -1 sur une erreur et positionne "CMP_err" à
 * l'erreur correspondante.
 * \error ERR_BAD_ADRESS si un pointeur est incorrect
 * \error ERR_IO_FWRITE si un problème survient lors du flush du buffer
 * d'écriture sur le disque.
 */
int cmpf_close(cmp_file_s * cf);

/**
 * Rembobine le fichier d'entrée.
 * \param cf Pointeur vers une structure contenant le fichier entrant à
 * rembobiner.
 */
void cmpf_rewind(cmp_file_s * cf);

/**
 * Récupère un byte depuis un bloc à une position donnée.
 * \param blck Bloc source.
 * \param pos Position pour récupérer le byte (bit de poids faible).
 * \return byte_t Byte récupérer.
 */
byte_t blck_get_byte(const block_t blck, const int pos);

/**
 * Ajoute un byte dans un bloc cible sur une position donnée. Le bloc n'a pas
 * besoin d'être initialisé à 0.
 * \param blck Bloc cible.
 * \param byte Byte à ajouter.
 * \param pos Position cible (bit de poids faible).
 * \return block_t Bloc modifié.
 */
block_t blck_put_byte(block_t blck, const byte_t byte, const int pos);

#endif
