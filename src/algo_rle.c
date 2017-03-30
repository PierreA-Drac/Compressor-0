/**
 * \file algo_rle.c
 * \author AYOUB Pierre
 * \date 02 mars 2017
 *
 * \brief RLE.
 * \details Module de l'algorithme RLE (Run-Lenght Encoding).
 */

/* Fonctionnement détaillé de l'algorithme : On lit les caractères du fichier
 * entrant sur 8 octets. On remarque que les caractères de la table ASCII
 * sont < 128, donc que les octets des caractères ont leurs bit de poids fort à
 * 0. Si on détecte plusieurs caractères identiques à la suite, on ne va noter
 * qu'un seul de ces caractères précédé par un bit à 1 puis d'un code de
 * répétition sur 2 bits indiquant le nombre d'apparition du caractère. Ce
 * premier bit à 1 permettra d'identifier un code de répétition par rapport à un
 * caractère codé sur 8 bits.
 * L'algorithme nécessite des fichiers encodés en ASCII pour fonctionner. */

#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "errors.h"
#include "io.h"
#include "common.h"

/* Macro-constantes privées ================================================= */

/* Valeur maximal du code de répétition. */
#define REP_CODE_MAX ((0b1 << REP_CODE_LENGHT)-1)
/* Nombre de bit du code de répétition (sans compter le premier bit ID).
 * 2 <= REP_CODE_LENGHT <= 7. */
#define REP_CODE_LENGHT 3       /* Valeur optimale déterminée empiriquement. */

/* Utilisés pour gérer le déplacement dans les blocs en fonction de la
 * compression ou de la décompression. */
#define RLE_MODE_COMPRESS 0
#define RLE_MODE_DECOMPRESS 1

/* Variables globales privées =============================================== */

/* Mode de déplacement sur les blocs lors de l'écriture et de la lecture. 0
 * permet une lecture du bit de poids faible vers le bit de fort du bloc et une
 * écriture du bit de poids fort vers le bit de poids faible du bloc, et 1
 * permet une lecture du bit de poids fort vers le bit de poids faible du bloc
 * et une écriture du bit de poids faible vers le bit de poids fort du bloc. */
static int RLE_MODE_MOV;

/* Fonctions privées ======================================================== */

/* # Écriture =============================================================== */

/* Fonctions de manipulation de bloc par RLE. Ces fonctions sont spécifiques à
 * cet algorithme car RLE utilise un traitement de bloc particulier (écrit dans
 * le sens inverse de la lecture pour la compression, gère donc ses indices de
 * position différemment). Les prototypes sont rajouté car obligatoire pour
 * l'inlining (sinon, gcc annonce qu'il manque des références sur ces
 * fonctions). */

/* Vide le bloc "blck" dans la structure "cf". Réinitialise le bloc à 0 et la
 * position "pos" pour la prochaine écriture.
 * Renvoie 0 sur un succès, -1 sur une erreur et positionne "CMP_err" sur
 * l'erreur correspondante.
 * Erreurs : ERR_BAD_ADRESS si un pointeur est incorrect, ERR_IO_FWRITE si une
 * erreur survient lors de l'écriture. */
static int rle_blck_flush(cmp_file_s * cf, block_t * blck, int *pos);
static inline int rle_blck_flush(cmp_file_s * cf, block_t * blck, int *pos)
{
    assert(cf && blck && pos);
    block_t blck_tmp = *blck;
    *blck = 0;
    *pos = !RLE_MODE_MOV ? BLOCK_LENGHT : 0;
    return cmpf_put_block(cf, blck_tmp);
}

/* Ajoute un bit à la position "pos" au bloc "blck". Le bloc sera vidé dans "cf"
 * automatiquement pour l'écriture du bit.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et "CMP_err" sera positionné
 * sur l'erreur correspondante.
 * Erreurs : ERR_BAD_ADRESS si un pointeur est incorrect, ERR_IO_FWRITE si une
 * erreur survient lors de l'écriture. */
static int rle_blck_put_bit(cmp_file_s * cf, block_t * blck,
                            const int bit, int *pos);
static inline int rle_blck_put_bit(cmp_file_s * cf, block_t * blck,
                                   const int bit, int *pos)
{
    assert(cf && blck && pos);
    assert(bit == 0 || bit == 1);
    /* Cas où le bloc est plein. */
    if (*pos + RLE_MODE_MOV == 0 || *pos + RLE_MODE_MOV == BLOCK_LENGHT + 1) {
        if ((rle_blck_flush(cf, blck, pos)))
            return -1;
    }
    /* Déplacement avant l'écriture si on compresse. */
    *pos -= 1 - RLE_MODE_MOV;
    PUT_BIT(*blck, bit, *pos);
    /* Déplacement après l'écriture si on décompresse. */
    *pos += RLE_MODE_MOV;
    return 0;
}

/* Écris le mot "byte" de longueur "w_len" bit par bit sur le bloc "blck" à la
 * position "pos" (bit de poids faible). Le bloc sera vidé automatiquement dans
 * "cf" s'il n'y a plus de place pour le mot. La position sera modifiée
 * automatiquement pour l'écriture du mot.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et "CMP_err" sera positionné
 * sur l'erreur correspondante.
 * Erreurs : ERR_BAD_ADRESS si un pointeur est incorrect, ERR_IO_FWRITE si une
 * erreur survient lors de l'écriture. */
static int rle_blck_put_word_by_bit(cmp_file_s * cf, block_t * blck, int *pos,
                                    const byte_t byte, const int w_len);
static inline int rle_blck_put_word_by_bit(cmp_file_s * cf, block_t * blck,
                                           int *pos, const byte_t byte,
                                           const int w_len)
{
    assert(cf && blck && pos);
    assert((unsigned int)w_len <= CHAR_BIT);
    for (int i = (1 - RLE_MODE_MOV) * (w_len - 1); (unsigned int)i < w_len;
         i += -(1 - RLE_MODE_MOV) + RLE_MODE_MOV) {
        byte_t bit;
        GET_BIT(byte, bit, i);
        if (rle_blck_put_bit(cf, blck, bit, pos))
            return -1;
    }
    return 0;
}

/* Ajoute le mot "byte" de longueur "w_len" au block "blck" à la position "pos"
 * (bit de poids faible). Écris le mot d'un seul coup s'il est d'une longueur de
 * "CHAR_BIT" (meilleur performance), ou bit par bit s'il est plus petit. Le
 * bloc sera vidé automatiquement dans "cf" s'il n'y a plus de place pour le
 * mot. La position sera modifiée automatiquement pour l'écriture du mot.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et "CMP_err" sera positionné
 * sur l'erreur correspondante.
 * Erreurs : ERR_BAD_ADRESS si un pointeur est incorrect, ERR_IO_FWRITE si une
 * erreur survient lors de l'écriture. */
static int rle_blck_put_word(cmp_file_s * cf, block_t * blck, int *pos,
                             const byte_t byte, const int w_len);
static inline int rle_blck_put_word(cmp_file_s * cf, block_t * blck, int *pos,
                                    const byte_t byte, const int w_len)
{
    assert(cf && blck && pos);
    assert((unsigned int)w_len <= CHAR_BIT);
    /* Cas où le mot est plus petit que 8 bits (code de répétition), ou qu'il
     * n'y a pas la place pour écrire le mot. */
    if (w_len < CHAR_BIT || (!RLE_MODE_MOV && *pos < CHAR_BIT)
        || (RLE_MODE_MOV && *pos > BLOCK_LENGHT - CHAR_BIT)) {
        if (rle_blck_put_word_by_bit(cf, blck, pos, byte, w_len))
            return -1;
    } else {
        *pos -= (1 - RLE_MODE_MOV) * CHAR_BIT;  /* Déplacement compression. */
        *blck = blck_put_byte(*blck, byte, *pos);
        *pos += RLE_MODE_MOV * CHAR_BIT;        /* Déplacement décompression. */
    }
    return 0;
}

/* # Lecture ================================================================ */

/* Recharge le bloc "blck" depuis la structure "cf". Réinitialise la position
 * "pos" pour la prochaine lecture.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et "CMP_err" sera positionné
 * sur l'erreur correspondante.
 * Erreurs : ERR_BAD_ADRESS si un pointeur est incorrect, ERR_IO_FREAD si une
 * erreur survient lors de la lecture, ERR_IO_FREAD_EOF si on à déjà lu la fin
 * du fichier. */
static int rle_blck_reload(cmp_file_s * cf, block_t * blck, int *pos);
static inline int rle_blck_reload(cmp_file_s * cf, block_t * blck, int *pos)
{
    assert(cf && blck && pos);
    *pos = RLE_MODE_MOV ? BLOCK_LENGHT : 0;
    return cmpf_get_block(cf, blck);
}

/* Récupère un bit à la position "pos" au bloc "blck". Le bloc sera recharger
 * depuis "cf" si besoin. La position sera modifiée automatiquement pour la
 * lecture du bit.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et "CMP_err" sera positionné
 * sur l'erreur correspondante.
 * Erreurs : ERR_BAD_ADRESS si un pointeur est incorrect, ERR_IO_FREAD si une
 * erreur survient lors de la lecture, ERR_IO_FREAD_EOF si on à déjà lu la fin
 * du fichier. */
static int rle_blck_get_bit(cmp_file_s * cf, block_t * blck,
                            int *bit, int *pos);
static inline int rle_blck_get_bit(cmp_file_s * cf, block_t * blck,
                                   int *bit, int *pos)
{
    assert(cf && blck && bit && pos);
    /* Cas où le bloc est vide. */
    if (*pos + (1 - RLE_MODE_MOV) == 0
        || *pos + (1 - RLE_MODE_MOV) == BLOCK_LENGHT + 1) {
        if (rle_blck_reload(cf, blck, pos))
            return -1;
    }
    *pos -= RLE_MODE_MOV;
    GET_BIT(*blck, *bit, *pos);
    *pos += 1 - RLE_MODE_MOV;
    assert(*bit == 0 || *bit == 1);
    return 0;
}

/* Récupère un mot de longueur "w_len" bit par bit sur le bloc "blck" à la
 * position "pos" (bit de poids faible) et le stocke dans "byte". Le bloc sera
 * recharger automatiquement dans "cf" si besoin. La position sera modifiée
 * automatiquement pour la lecture du mot.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et "CMP_err" sera positionné
 * sur l'erreur correspondante.
 * Erreurs : ERR_BAD_ADRESS si un pointeur est incorrect, ERR_IO_FREAD si une
 * erreur survient lors de la lecture, ERR_IO_FREAD_EOF si on à déjà lu la fin
 * du fichier. */
static int rle_blck_get_word_by_bit(cmp_file_s * cf, block_t * blck, int *pos,
                                    byte_t * byte, const int w_len);
static inline int rle_blck_get_word_by_bit(cmp_file_s * cf, block_t * blck,
                                           int *pos, byte_t * byte,
                                           const int w_len)
{
    assert(cf && blck && pos);
    assert((unsigned int)w_len <= CHAR_BIT);
    for (int i = (RLE_MODE_MOV) * (w_len - 1), bit = 0; (unsigned int)i < w_len;
         i -= -(1 - RLE_MODE_MOV) + RLE_MODE_MOV) {
        if (rle_blck_get_bit(cf, blck, &bit, pos))
            return -1;
        PUT_BIT(*byte, bit, i);
    }
    return 0;
}

/* Récupère un mot de longueur "w_len" dans le bloc "blck" à la position "pos"
 * (bit de poids faible), et le stocke dans "byte". Lit le mot d'un seul coup si
 * possible, ou si le mot est coupé entre deux bloc ou plus petit que 8 bits, on
 * lit le mot bit à bit et récupère un nouveau bloc dans "cf" si besoin. Modifie
 * la position pour la lecture du mot.
 * Renvoie 0 sur un succès, ou -1 sur une erreur et "CMP_err" sera positionné
 * sur l'erreur correspondante.
 * Erreurs : ERR_BAD_ADRESS si un pointeur est incorrect, ERR_IO_FREAD si une
 * erreur survient lors de la lecture, ERR_IO_FREAD_EOF si on à déjà lu la fin
 * du fichier. */
static int rle_blck_get_word(cmp_file_s * cf, block_t * blck, int *pos,
                             byte_t * byte, const int w_len);
static inline int rle_blck_get_word(cmp_file_s * cf, block_t * blck, int *pos,
                                    byte_t * byte, const int w_len)
{
    assert(cf && blck && pos && byte);
    assert((unsigned int)w_len <= CHAR_BIT);
    /* Cas où le mot est coupé entre deux blocs, ou qu'il est plus petit que 8
     * bits : il faut le lire bit par bit. */
    if (w_len < CHAR_BIT || (RLE_MODE_MOV && *pos < CHAR_BIT)
        || (!RLE_MODE_MOV && *pos > BLOCK_LENGHT - CHAR_BIT)) {
        if (rle_blck_get_word_by_bit(cf, blck, pos, byte, w_len))
            return -1;
    } else {
        *pos -= RLE_MODE_MOV * CHAR_BIT;
        *byte = blck_get_byte(*blck, *pos);
        *pos += (1 - RLE_MODE_MOV) * CHAR_BIT;
    }
    /* Si byte == 0, on est à la fin de notre bloc. */
    if (!(*byte))
        return CMP_err = ERR_IO_FREAD_EOF, -1;
    return 0;
}

/* Fonctions publiques ====================================================== */

/* N.B. : L'indice d'écriture commence à BLOCK_LENGHT car on écris les caractères dans
 * le sens inverse duquel ont les à lu, pour pouvoir à la décompression
 * détecter les bits de poids fort à 0 d'un caractère et le bit à 1 d'un
 * code de répétition. On pouvait soit les écrires de gauche à droite, soit
 * les écrires en big endian. Je pense que de gauche à droite doit être plus
 * performant car on change de sens des groupes de bits et non chaueque bits
 * un à un. */

int rle_compress(cmp_file_s * cf)
{
    if (!cf)
        return CMP_err = ERR_BAD_ADRESS, -1;
    CMP_err = ERR_NONE;

    RLE_MODE_MOV = RLE_MODE_COMPRESS;   /* Déplacement pour ce mode (compression). */
    block_t blck_in = 0, blck_out = 0;  /* Blocs de données. */
    byte_t byte_1 = 0, byte_2 = 0;      /* Octets temporaires pour comparaisons. */
    int count = 1, ind_in = BLOCK_LENGHT, ind_out = BLOCK_LENGHT;       /* Compteur et indice. */

    rle_blck_get_word(cf, &blck_in, &ind_in, &byte_2, CHAR_BIT);
    /* Parsing des blocs de données entrant (récupération des blocs
     * automatiques). */
    while (!CMP_err) {
        /* Switch, relecture, comptage. */
        byte_1 = byte_2;
        rle_blck_get_word(cf, &blck_in, &ind_in, &byte_2, CHAR_BIT);
        count += (byte_1 == byte_2);
        /* Cas sans répétition, écriture du caractère. */
        if (count == 1)
            rle_blck_put_word(cf, &blck_out, &ind_out, byte_1, CHAR_BIT);
        /* Cas avec répétition terminée ou code de répétition plein. */
        else if (byte_1 != byte_2 || count == REP_CODE_MAX) {
            /* Switch pour forcer la terminaison de la répétition. */
            if (count == REP_CODE_MAX) {
                byte_1 = byte_2;
                rle_blck_get_word(cf, &blck_in, &ind_in, &byte_2, CHAR_BIT);
            }
            /* Éciture de l'ID d'un code : 1 bit à 1. */
            PUT_BIT(count, 1, REP_CODE_LENGHT);
            /* Écriture du code de répétition. */
            rle_blck_put_word(cf, &blck_out, &ind_out, count,
                              REP_CODE_LENGHT + 1);
            /* Écriture du caractère à répeter et reset du compteur. */
            rle_blck_put_word(cf, &blck_out, &ind_out, byte_1, CHAR_BIT);
            count = 1;

        }
        /* Sinon, on est dans une répétition non terminée et count s'est
         * juste incrémenté de 1. */
    }
    /* Écriture du dernier byte du bloc s'il n'a pas été comparé (passer dans
     * byte_1). */
    if (byte_2)
        rle_blck_put_word(cf, &blck_out, &ind_out, byte_2, CHAR_BIT);
    /* Si erreur pendant la compression ou l'écriture du dernier bloc. */
    if ((CMP_err != ERR_IO_FREAD_EOF && CMP_err)
        || cmpf_put_block(cf, blck_out))
        return err_print(CMP_err), CMP_err = ERR_COMPRESSION_FAILED, -1;
    return 0;
}

        }
        byte_1 = byte_2;        /* Switch avant la lecture du premier caractère. */
    }
    return 0;
}
