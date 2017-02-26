/** 
 * \file io.h
 * \author AYOUB Pierre
 * \date 29 janvier 2017
 *
 * \brief Entrées/sorties.
 * \details Module de gestion des entrées/sorties sur le disque.
 */

#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include "io.h"
#include "errors.h"
#include "common.h"

/* Fonctions privées ======================================================== */

/* Lit le fichier source de cf depuis le disque et le stocke dans son buffer de
 * lecture. Renvoie 0 sur un succès, ERR_FREAD_EOF si la fin du fichier à été
 * atteinte, ou -1 si une erreur de fwrite s'est produite et affiche le message
 * correspondant. */
static int cmpf_read_file(cmp_file_s * cf)
{
    assert(cf);
    assert(cf->fp_in);
    assert(cf->a_read_stream);
    /* Padding à 0, car sinon il peut rester des anciens bits sur les blocs non
     * complètement remplis. */
    for (int i = 0; i < BUFFER_SIZE; i++)
        cf->a_read_stream[i] = 0;
    /* Lecture sur le disque. */
    if (!(cf->nb_bytes = fread(cf->a_read_stream, sizeof(byte_t),
                               BLOCK_SIZE * BUFFER_SIZE, cf->fp_in))) {
        if (feof(cf->fp_in))    /* Si on était déjà à la fin du fichier. */
            return ERR_FREAD_EOF;
        else if (ferror(cf->fp_in))     /* Si une erreur s'est produite. */
            return perror("fread in cmpf_read_file"), -1;
    }
    cf->nb_blocks = cf->nb_bytes >> 3;  /* log(BLOCK_SIZE) en base 2 : pos bit le
                                           plus à gauche */
    if (cf->nb_bytes & (BLOCK_SIZE - 1))        /* Si division pas entière. */
        cf->nb_blocks++;
    /* Réinitialisation du pointeur de lecture. */
    cf->p_read = cf->a_read_stream;
    assert(cf->nb_bytes);
    assert(cf->nb_blocks);
    assert(cf->p_read);
    return 0;
}

/* Écris le bloc pointé par p_blck sur le fichier pointé par p_stream en
 * supprimant les octets égaux à 0. Renvoie 0 sur un succès,
 * -1 sur une erreur de fwrite et affiche le message correspondant. */
static int block_write_parse(block_t * p_blck, FILE * p_stream)
{
    assert(p_blck);
    assert(p_stream);
    block_t mask = 0xFF;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        if (*p_blck & mask) {
            if (!fwrite(p_blck, sizeof(byte_t), 1, p_stream))
                return perror("fwrite in block_write_parse"), -1;
        }
        *p_blck >>= CHAR_BIT;
    }
    return 0;
}

/* Vide le buffer d'écriture du fichier de sortie de cf sur le disque. Renvoie 0
 * sur un succès, ou -1 sur une erreur de fwrite et affiche le message
 * correspondant. */
static int cmpf_write_file(cmp_file_s * cf)
{
    assert(cf);
    assert(cf->a_write_stream);
    assert(cf->p_write);
    assert(cf->fp_out);
    /* Écriture sur le disque. */
    if (!fwrite(cf->a_write_stream, sizeof(block_t),
                cf->p_write - cf->a_write_stream - 1, cf->fp_out) &&
        (cf->p_write - cf->a_write_stream - 1) != 0) {
        return perror("fwrite in cmpf_write_file"), -1;
    }
    /* Écris le dernier bloc sans les bits à 0 en trop. */
    if (block_write_parse(cf->p_write - 1, cf->fp_out))
        return -1;
    /* Réinitialisation du pointeur d'écriture. */
    cf->p_write = cf->a_write_stream;
    return 0;
}

/* Fonctions publiques ====================================================== */

cmp_file_s cmpf_open(const char *s_filepath_in, const char *s_filepath_out)
{
    /* Ouverture des fichiers. */
    cmp_file_s cf;
    if (!(cf.fp_in = fopen(s_filepath_in, "rb")) ||
        !(cf.fp_out = fopen(s_filepath_out, "wb"))) {
        perror("fopen in init_cmp_file");
        exit(EXIT_FAILURE);
    }
    /* Initilisation des variables. */
    cf.nb_blocks = cf.nb_bytes = cf.a_read_stream[0] = cf.a_write_stream[0] = 0;
    cf.p_read = cf.p_write = NULL;
    assert(cf.fp_in);
    assert(cf.fp_out);
    return cf;
}

int cmpf_get_block(cmp_file_s * cf, block_t * b)
{
    if (!cf || !b)
        return p_error(ERR_BAD_ADRESS);
    assert(cf->a_read_stream);
    /* Si c'est la première lecture dans ce buffer ou que l'on arrive à la fin,
     * on le remplis de nouveau. */
    if (!cf->p_read || (cf->p_read == &(cf->a_read_stream[BUFFER_SIZE]))) {
        if (cmpf_read_file(cf))
            return p_error(ERR_FREAD);
    }
    /* Si on déjà lu la fin du fichier dans le buffer. */
    else if (cf->p_read == &(cf->a_read_stream[cf->nb_blocks]))
        return ERR_FREAD_EOF;
    /* Lit le bloc du buffer et met à jours l'adresse du prochain bloc à lire. */
    *b = *cf->p_read;
    cf->p_read++;
    assert(cf->p_read);
    return 0;
}

int cmpf_put_block(cmp_file_s * cf, const block_t b)
{
    if (!cf || !b)
        return p_error(ERR_BAD_ADRESS);
    assert(cf->a_write_stream);
    /* Si c'est la première écriture dans ce buffer. */
    if (!cf->p_write)
        cf->p_write = cf->a_write_stream;
    /* Si le buffer est plein, on le vide sur le disque. */
    if (cf->p_write == &(cf->a_write_stream[BUFFER_SIZE])) {
        if (cmpf_write_file(cf))
            return p_error(ERR_FWRITE);
    }
    /* Écris le bloc dans le buffer et met à jours l'adresse du prochain bloc
     * à écrire. */
    *cf->p_write = b;
    cf->p_write++;
    /* Padding à 0 du bloc après le dernier bloc. */
    if (cf->p_write != &(cf->a_write_stream[BUFFER_SIZE]))
        *cf->p_write = 0;
    assert(cf->p_write);
    return 0;
}

int cmpf_close(cmp_file_s * cf)
{
    if (!cf)
        return p_error(ERR_BAD_ADRESS);
    /* Vide le buffer avant la fermeture des flux. */
    if (cmpf_write_file(cf))
        return p_error(ERR_FWRITE);
    /* Ferme les fichiers. */
    fclose(cf->fp_in);
    fclose(cf->fp_out);
    /* Réinitialisation des variables. */
    cf->fp_in = cf->fp_out = NULL;
    cf->p_read = cf->p_write = NULL;
    cf->nb_bytes = cf->nb_blocks = 0;
    cf->a_read_stream[0] = cf->a_write_stream[0] = 0;
    return 0;
}

void cmpf_rewind(cmp_file_s * cf)
{
    rewind(cf->fp_in);
    cf->p_read = NULL;
}
