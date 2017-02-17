#!/bin/bash

# Ce script permet d'automatiser les benchmarks pour tester les algorithmes sur
# les fichiers. Il suffit de passer en argument à ce script les algorithmes
# disponible. Ainsi, le script automatise la compression de tout les fichiers
# présents dans $files_path par tout les algorithmes, puis la génération et mise
# en forme de statistiques.

# Variables ====================================================================

## Structure du projet ........................................................:

# Dossier racine du projet.
root_path='./../'
# Dossier contenant les fichiers à chercher.
files_path='env/text/'

## Algorithmes et fichiers ....................................................:

# Liste des algorithmes disponibles.
algos=("$1")
# Liste des fichiers à compresser.
files=(`find "$root_path$files_path"*`)

## Fichiers générés ...........................................................:

# Fichier final généré contenant les statistiques.
stat_file="`echo $0 | sed -e "s/\(.*\)\..*/\1/g"`_out.log"
# Suffixe pour le fichier temporaire.
tmp_suf='.tmp'
# Fichier de statistiques temporaire.
stat_file_tmp="$stat_file$tmp_suf"

# Script =======================================================================

if [ -z "$algos" ]
then
    echo -e "Erreur : aucun algorithme à tester." \
        "\nUtilisation : $0 \"ALGO_1 ALGO_2 ...\""
    exit -1
else
    if [ -z "$files" ]
    then
        echo "Erreur : aucun fichier à compresser."
        exit -1
    else
        echo "Compilation..."
        make compil --directory=$root_path > /dev/null
        if [ $? -ne 0 ]
        then
            echo "Erreur : compilation échouée."
            exit -1
        fi
        # Inscrit le nom des colonnes.
        echo "#Fichier|Algorithme|Taille avant (kB)|Taille après" \
           "(kB)|Temps de compression (s)|Espace mémoire utilisé (kB)" \
            > $stat_file_tmp
        # Boucles générant les compressions et les statistiques.
        for file in ${files[*]}
        do
            for algo in ${algos[*]}
            do
                file=`echo "$file" | sed -e "s/[\.|\/]*\(.*\)/\1/g"`
                echo "Compression de $file avec $algo..."
                # Inscris le nom de l'algorithme et du fichier traité.
                echo "`echo $file | sed -e "s/.*\/\(.*\)/\1/g"`|$algo" \
                    | tr '\n' '|' >> $stat_file_tmp
                # Lance la compression, supprime les messages de make, isole les
                # données importantes et les mets en forme.
                make --directory=$root_path --no-print-directory \
                    ARGS="-c -i \"$file\" -s --$algo"\
                    | sed -n 3,6p \
                    | sed -e "s/.* : //g" -e "s/\([0-9.]*\).*/\1/g" \
                    | tr '\n' '|' >> $stat_file_tmp
                echo -e "\n" >> $stat_file_tmp
            done
        done
        echo -e "Résultats :\n"
        # Supprime les lignes vides du fichier, enregistre dans le fichier
        # final, enlève les commentaires et ajuste les colonnes pour
        # l'affichage.
        sed '/^$/d' $stat_file_tmp | tee $stat_file | sed -e "s/#\(.*\)/\1/g" \
            | column -s '|' -t
        rm $stat_file_tmp
        echo ""
    fi
fi