/** 
 * \file io.c
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Entrées/sorties.
 * \details Module de gestion des entrées/sorties sur le disque.
 */

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include "io.h"
#include "errors.h"
#include "common.h"

/* Portabilité entre compilateur. */
#ifndef __GNUC__
#define  __attribute__(x)       /* Nothing. */
#endif

/* Macro-constantes privées ================================================= */

/* Taille d'un tableau contenant un fichier chargé en mémoire. */
#define IO_BUFFER_SIZE 2048     /* Optimal après tests empiriques. */

/* Aligmement des grosses structures en mémoire. */
#define IO_ALIGN 64             /* Cacheline. */

/* Structures privées ======================================================= */

/* Correspond à un fichier en cours de traitement. */
struct cmp_file {
    FILE *fp_in;                /* Fichier entrant. */
    FILE *fp_out;               /* Fichier sortant. */
    int nb_blocks;              /* Nombre de bloc chargé dans "a_read_stream". */
    int nb_bytes;               /* Nombre de byte chargé dans "a_read_stream". */
    block_t a_read_stream       /* Flux contenant les données à lire. */
        [IO_BUFFER_SIZE];
    block_t a_write_stream      /* Flux contenant les données à écrire. */
        [IO_BUFFER_SIZE];
    block_t *p_read;            /* Pointeur sur le prochain bloc à lire. */
    block_t *p_write;           /* Pointeur sur le prochain bloc à écrire. */
} __attribute__ ((aligned(IO_ALIGN)));

/* Fonctions privées ======================================================== */

/* Lit le fichier source de "cf" depuis le disque et le stocke dans son buffer de
 * lecture.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et positionne "CMP_err"
 * sur l'erreur produite.
 * Erreurs : ERR_IO_FREAD si une erreur intervient pendant "fread", ou
 * ERR_IO_FREAD_EOF si on essaye de lire tandis que la fin du fichier à déjà été
 * atteinte. */
static int cmpf_read_file(cmp_file_s * cf)
{
    assert(cf && cf->fp_in && cf->a_read_stream);
    /* Padding à 0, car sinon il peut rester des anciens bits sur les blocs non
     * complètement remplis. */
    memset(cf->a_read_stream, '\0', IO_BUFFER_SIZE * BLOCK_SIZE);
    /* Lecture sur le disque. */
    if (!(cf->nb_bytes = fread(cf->a_read_stream, sizeof(byte_t),
                               BLOCK_SIZE * IO_BUFFER_SIZE, cf->fp_in))) {
        /* Si on était déjà à la fin du fichier. */
        if (feof(cf->fp_in))
            CMP_err = ERR_IO_FREAD_EOF;
        /* Si une erreur s'est produite. */
        else if (ferror(cf->fp_in))
            perror("fread"), CMP_err = ERR_IO_FREAD;
        return -1;
    }
    cf->nb_blocks = cf->nb_bytes >> 3;  /* log(BLOCK_SIZE) en base 2 : pos bit le
                                           plus à gauche */
    /* Si division pas entière. */
    if (cf->nb_bytes & (BLOCK_SIZE - 1))
        cf->nb_blocks++;
    /* Réinitialisation du pointeur de lecture. */
    cf->p_read = cf->a_read_stream;
    assert(cf->nb_bytes && cf->nb_blocks && cf->p_read);
    return 0;
}

/* Écris le bloc "blck" sur le fichier pointé par "p_stream" en supprimant les
 * octets égaux à 0 en fin de bloc.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et postionne "CMP_err" sur
 * l'erreur produite.
 * Erreurs : ERR_IO_FWRITE si une erreur survient pendant l'écriture avec
 * "fwrite". */
static int blck_write_parse(block_t blck, FILE * p_stream)
{
    assert(p_stream);
    block_t mask = 0xFF;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        if (blck & mask || blck) {
            if (!fwrite(&blck, sizeof(byte_t), 1, p_stream))
                return CMP_err = ERR_IO_FWRITE, perror("fwrite"), -1;
        }
        blck >>= CHAR_BIT;
    }
    return 0;
}

/* Vide le buffer d'écriture du fichier de sortie de "cf" sur le disque.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et positionne "CMP_err" sur
 * l'erreur correspondante.
 * Erreurs : ERR_IO_FWRITE si une erreur survient pendant l'écriture avec
 * "fwrite". */
static int cmpf_write_file(cmp_file_s * cf)
{
    assert(cf && cf->a_write_stream && cf->p_write && cf->fp_out);
    /* Écriture sur le disque sans le dernier bloc. */
    if ((cf->p_write - cf->a_write_stream - 1) &&
        !fwrite(cf->a_write_stream, sizeof(block_t),
                cf->p_write - cf->a_write_stream - 1, cf->fp_out)) {
        return CMP_err = ERR_IO_FWRITE, perror("fwrite"), -1;
    }
    /* Écris le dernier bloc sans les bits à 0 en trop. */
    if (blck_write_parse(*(cf->p_write - 1), cf->fp_out))
        return -1;
    /* Réinitialisation du pointeur d'écriture. */
    cf->p_write = cf->a_write_stream;
    return 0;
}

/* Fonctions publiques ====================================================== */

cmp_file_s *cmpf_open(const char *s_filepath_in, const char *s_filepath_out)
{
    /* Ouverture des fichiers. */
    cmp_file_s *cf = malloc(sizeof(cmp_file_s));
    if (!(cf->fp_in = fopen(s_filepath_in, "rb")) ||
        !(cf->fp_out = fopen(s_filepath_out, "wb"))) {
        perror("fopen for file initialization");
        exit(EXIT_FAILURE);
    }
    /* Initilisation des variables. */
    cf->nb_blocks = cf->nb_bytes = cf->a_read_stream[0] =
        cf->a_write_stream[0] = 0;
    cf->p_read = cf->p_write = NULL;
    assert(cf->fp_in && cf->fp_out);
    return cf;
}

inline int cmpf_get_block(cmp_file_s * cf, block_t * b)
{
    if (!cf || !b)
        return CMP_err = ERR_BAD_ADRESS, -1;
    assert(cf->a_read_stream);
    /* Si c'est la première lecture dans ce buffer ou que l'on arrive à la fin,
     * on le remplis de nouveau. */
    if (!cf->p_read || (cf->p_read == &(cf->a_read_stream[IO_BUFFER_SIZE]))) {
        if (cmpf_read_file(cf))
            return -1;
    }
    /* Si on déjà lu la fin du fichier dans le buffer. */
    else if (cf->p_read == &(cf->a_read_stream[cf->nb_blocks]))
        return CMP_err = ERR_IO_FREAD_EOF, -1;
    /* Lit le bloc du buffer et met à jours l'adresse du prochain bloc à lire. */
    *b = *(cf->p_read++);
    assert(cf->p_read);
    return 0;
}

inline int cmpf_put_block(cmp_file_s * cf, const block_t b)
{
    if (!cf)
        return CMP_err = ERR_BAD_ADRESS, -1;
    assert(cf->a_write_stream);
    /* Si c'est la première écriture dans ce buffer. */
    if (!cf->p_write)
        cf->p_write = cf->a_write_stream;
    /* Si le buffer est plein, on le vide sur le disque. */
    if (cf->p_write == &(cf->a_write_stream[IO_BUFFER_SIZE])
        && cmpf_write_file(cf))
        return -1;
    /* Écris le bloc dans le buffer et met à jours l'adresse du prochain bloc
     * à écrire. */
    *(cf->p_write++) = b;
    /* Padding à 0 du bloc après le dernier bloc. */
    if (cf->p_write != &(cf->a_write_stream[IO_BUFFER_SIZE]))
        *cf->p_write = 0;
    assert(cf->p_write);
    return 0;
}

int cmpf_close(cmp_file_s * cf)
{
    if (!cf)
        return CMP_err = ERR_BAD_ADRESS, -1;
    /* Vide le buffer avant la fermeture des flux. */
    if (cf->p_write && cmpf_write_file(cf))
        return -1;
    /* Ferme les fichiers. */
    fclose(cf->fp_in);
    fclose(cf->fp_out);
    /* Libère la mémoire. */
    free(cf), cf = NULL;
    return 0;
}

void cmpf_rewind(cmp_file_s * cf)
{
    assert(cf && cf->fp_in);
    rewind(cf->fp_in);
    cf->p_read = NULL;
}

inline byte_t blck_get_byte(const block_t blck, const int pos)
{
    /* Unsigned => nombre négatif devient très grand entier ( > BLOCK_LENGHT). */
    assert((unsigned char)pos <= BLOCK_LENGHT - CHAR_BIT);
    return (blck >> pos) & 0xFF;
}

inline block_t blck_put_byte(block_t blck, const byte_t byte, const int pos)
{
    assert((unsigned char)pos <= BLOCK_LENGHT - CHAR_BIT);
    return (blck | ((uint64_t) byte << pos)) & ~((~(uint64_t) byte & 0xFF) <<
                                                 pos);
}
