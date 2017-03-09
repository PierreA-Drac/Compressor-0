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

/* Portabilité entre compilateur. */
#ifndef __GNUC__
#define  __attribute__(x)       /* NOTHING. */
#endif

/* Macro-constantes publiques =============================================== */

/** Taille d'un tableau contenant un fichier chargé en mémoire. */
#define BUFFER_SIZE 2048

/** Taille d'un bloc en byte. */
#define BLOCK_SIZE sizeof(block_t)
/** Longueur d'un bloc en bit. */
#define BLOCK_LENGHT BLOCK_SIZE*CHAR_BIT

/** Aligmement des grosses structures en mémoire. */
#define ALIGN 64

/* Macro-fonctions publiques ================================================ */

#define GET_BIT(src, bit, pos) do { \
    assert((unsigned int)(pos) < sizeof(src)*CHAR_BIT); \
    (bit) = ((src) >> (pos)) & 0b1; \
    } while (0)

#define PUT_BIT(dest, bit, pos) do { \
    assert((unsigned int)(pos) < sizeof(dest)*CHAR_BIT); \
    assert((bit) == 0 || (bit) == 1); \
    (dest) = ((dest) | ((uint64_t)(bit) << (pos))) & ~((~(uint64_t)(bit) & 0b1) << (pos)); \
    } while (0)

/* Types publiques ========================================================== */

typedef uint64_t block_t;

/* Structures publiques ===================================================== */

typedef struct cmp_file cmp_file_s;

/** Correspond à un fichier en cours de traitement. */
struct cmp_file {
    FILE *fp_in;                /*!< Fichier entrant. */
    FILE *fp_out;               /*!< Fichier sortant. */
    int nb_blocks;              /*!< Nombre de bloc chargé dans "a_read_stream". */
    int nb_bytes;               /*!< Nombre de byte chargé dans "a_read_stream". */
    block_t a_read_stream[BUFFER_SIZE]; /*!< Flux contenant les données à lire. */
    block_t a_write_stream[BUFFER_SIZE];        /*!< Flux contenant les données à
                                                   écrire. */
    block_t *p_read;            /*!< Pointeur sur le prochain bloc à lire. */
    block_t *p_write;           /*!< Pointeur sur le prochain bloc à écrire. */
} __attribute__ ((aligned(ALIGN)));

/* Fonctions publiques ====================================================== */

/**
 * Initialise les flux vers les fichiers entrant et sortant, initialise la
 * structure pour qu'elle soit prête à être utilisée et assure les routines de
 * détection d'erreurs. Quitte le programme avec EXIT_FAILURE si une erreur
 * survient.
 * \param filepath_in Chemin vers le fichier entrant.
 * \param filepath_out Chemin vers le fichier sortant.
 * \return Structure d'un fichier prêt à être traité.
 */
cmp_file_s cmpf_open(const char *s_filepath_in, const char *s_filepath_out);

/**
 * Lit un bloc de donnée du fichier entrant pointé par cf depuis son buffer, et
 * le stocke dans le bloc pointé par b.
 * \param cf Fichier traité.
 * \param b Bloc à remplir.
 * \return 0 sur un succès, ERR_BAD_ADRESS si un pointeur est incorrect (affiche
 * un message), ERR_IO_FREAD si une erreur survient lors de la lecture (affiche
 * un message), et ERR_IO_FREAD_EOF si on à déjà lu la fin du fichier.
 */
int cmpf_get_block(cmp_file_s * cf, block_t * b);

/**
 * Écris le bloc de donnée pointé par b sur le buffer du fichier sortant pointé
 * par cf.
 * \param cf Fichier traité.
 * \param b Bloc à écrire.
 * \return 0 sur un succès, ERR_BAD_ADRESS si un pointeur est incorrect (affiche
 * un message), ou ERR_IO_FWRITE si une erreur survient lors de l'écriture
 * (affiche un message).
 */
int cmpf_put_block(cmp_file_s * cf, block_t b);

/**
 * Vide le buffer d'écriture sur le disque, ferme les flux vers les fichiers
 * entrant et sortant, et réinitialise la structure comme lors de la création.
 * \param cf Fichier à fermer.
 * \return 0 sur un succès, ERR_BAD_ADRESS si un pointeur est incorrect (affiche
 * un message), ou ERR_IO_FWRITE si un problème survient lors du flush du buffer
 * d'écriture (affiche un message).
 */
int cmpf_close(cmp_file_s * cf);

/**
 * Rembobine le fichier d'entrée de cf au début.
 */
void cmpf_rewind(cmp_file_s * cf);

byte_t blck_get_byte(const block_t blck, const char pos);
block_t blck_put_byte(block_t blck, const byte_t byte, const char pos);

#endif
