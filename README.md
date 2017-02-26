# Compressor-0

## Statut

Ce répertoire contient le programme de compression **Compressor-0**,
actuellement en *cours de développement*.

## Objectifs

Le but de ce projet est de développer un logiciel en ligne commande qui permet
de compresser et de décompresser des fichiers. Les fichiers peuvent être de type
différents, mais nous traiterons dans un premier temps les fichiers textes. Ce
programme doit permettre l'utilisation de plusieurs algorithmes différents avec
différentes implémentations variées, l'objectif étant de trouver le meilleur
compromis entre le temps de compression et la taille de l'objet compressé.

## Fonctionnalités à implémenter

* Fonctions d'entrées/sorties génériques (en cours).
* Fonctions de modification et lecture bit à bit.
* Implémentation d'arbre binaire.
* Implémentation de tas binaire.
* Implémentations de plusieurs algorithmes :
    * Codage par répetition.
    * À déterminer.
* Histogrammes : plusieurs modes différents (ex. : regrouper chacune des données
par algo).

## Description

**Compressor-0** permet de compresser et décompresser des fichiers. Ce programme
ne traite pas l'archivage, il n'opère donc que sur un seul fichier à la fois.
Pour la compression, il suffit de spécifier l'algorithme cible à utiliser par un
flag. Pour la décompression, il n'est pas nécessaire de préciser l'algorithme.

## Utilisation

### Syntaxe

> $ <b>compressor-0 -c</b>|<b>-d -i</b> <i>INPUT FILE</i> 
> [<b>-o</b> <i>OUTPUT FILE</i>] [<i>ALGORITHM FLAG</i>] [<b>-s</b>] [<b>-h</b>]


### Options

> <b>-h</b>, <b>\-\-help</b> <br/>

Affiche l'aide sur la sortie standard.

> <b>-c</b>, <b>\-\-compress</b> <br/>

Mode de compression du fichier entrant. Implique de préciser un algorthime.

> <b>-d</b>, <b>\-\-decompress</b> <br/>

Mode de décompression du fichier entrant.

> <b>-s</b>, <b>\-\-statistics</b> <br/>

Affiche les statistiques de la compression ou de la décompression effectuée sur
la sortie standard.

> <b>-i</b> <i>INPUT FILE</i>, <b>\-\-input=</b><i>INPUT FILE</i> <br/>

Chemin vers le fichier entrant à traiter.

> <b>-o</b> <i>OUTPUT FILE</i>, <b>\-\-output=</b><i>OUTPUT FILE</i> <br/>

Chemin vers le fichier sortant résultant. Si non spécifié, le fichier sortant
gardera le nom du fichier source et sera écrit dans le répertoire "out/" situé
dans le répertoire de l'exécutable.

#### Algorithmes

> <b>\-\-LZMA</b> <br/>

Compresse le fichier en utilisant l'algorithme LZMA. (Ici à titre d'exemple,
algorithme non implémenté).

### Statut de sortie

Retourne la valeur 0 si la compression s'est bien effectuée.

### Exemples

> $ <b>compressor-0 -c -i</b> <i>env/corpus/text.txt</i> <b>-o</b> <i>text.cmp</i>
> <b>\-\-LZMA -s</b>

> $ <b>compressor-0 \-\-decompress \-\-input=</b><i>"text.cmp"</i>
> <b>\-\-output=</b><i>"text.txt"</i>

## Make instructions

Il est possible de switcher la variable DEBUG du Makefile, de 1, pour activer les
flags de débuggage, à 0, pour les désactiver et compiler en mode release.

> $ <b>make</b> [<b>run</b>] [<b>ARGS=</b><i>ARGUMENTS</i>] <br/>

Lance le programme avec les arguments de la variable ARGS.

> $ <b>make</b> <b>benchmark</b> <br/>

Lance les benchmarks sur les algorithmes spécifiés dans le Makefile du dossier
"bench/" et les fichiers présents dans le répertoire "./env/". Affiche le
résultat sous forme de texte sur la sortie standard et les histogrammes sur une
image vectorielle svg.

> $ <b>make compil</b> <br/>

Compile le programme.

> $ <b>make clean</b> <br/>

Nettoie les fichiers temporaires et de sauvegarde du dossier de travail.

> $ <b>make mrproper</b> <br/>

Effectue le nettoyage d'un "clean" ainsi que l'exécutable, les fichiers
produits par lui-même, les benchmarks et la documentation.

> $ <b>make gdb</b> [<b>ARGS=</b><i>ARGUMENTS</i>] <br/>

Lance gdb pour débugguer avec les arguments de la variable ARGS.

> $ <b>make valgrind-p</b>[<b>1</b>|<b>2</b>] [<b>ARGS=</b><i>ARGUMENTS</i>] <br/>

Lance valgrind avec les arguments de la variables ARGS avec le profil p1 (plus
rapide mais moins précis) ou le profil p2 (plus lent mais plus précis).

> $ <b>make indent</b> <br/>

Lance le progamme indent sur les fichiers sources et headers avec les paramètres
du fichier .indent.pro.

> $ <b>make doc</b> <br/>

Affiche la documentation du projet (lance la génération si non générée
précédemment).

> $ <b>make man</b> <br/>

Affiche le manuel du programme.
