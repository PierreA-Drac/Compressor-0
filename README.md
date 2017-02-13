# Compressor-0

## Statut

Ce répertoire contient le programme de compression **Compressor-0**,
actuellement en *cours de développement*.

## Objectifs

Le but de ce projet est de développer un logiciel en ligne commande qui permet
de compresser et de décompresser des fichiers. Les fichiers peuvent être de type
variés, mais nous traiterons dans un premier temps les fichiers textes. Ce
programme doit permettre l'utilisation de plusieurs algorithmes différents avec
différentes implémentations variées, l'objectif étant de trouver le meilleur
compromis entre le temps de compression et la taille de l'objet compressé.

## Fonctionnalités implémentées

* Récupération de statistiques sur la compression :
    * Temps d'exécution du programme.
    * Espace en mémoire utilisée.
    * Taille du fichier d'entrée.
    * Taille du fichier en sortie.

## Fonctionnalités à implémenter

* Implémentations de plusieurs algorithmes :
    * À déterminer.
* Génération d'histogramme avec Gnuplot contenant les informations suivantes :
    * Informations du log brut.
    * Rapport entre la taille du fichier d'entrée et la taille du fichier en
      sortie.
    * Rapport entre le temps d'exécution et l'espace mémoire utilisé.

## Description

**Compressor-0** permet de compresser et décompresser des fichiers. Ce programme
ne traite pas l'archivage, il n'opère donc que sur un seul fichier à la fois.
Pour la compression, il suffit de spécifier l'algorithme cible à utiliser par un
flag. Pour la décompression, il n'est pas nécessaire de préciser l'algorithme.

## Utilisation

### Syntaxe

> <b>compressor-0 -c</b>|<b>-d -i</b> <i>INPUT FILE</i> 
> [<b>-o</b> <i>OUTPUT FILE</i>] [<i>ALGORITHM FLAG</i>] [<b>-s</b>] [<b>-h</b>]


### Options

> <b>-h</b>, <b>\-\-help</b> <br/>
Affiche l'aide sur la sortie standard. <br/>
> <b>-c</b>, <b>\-\-compress</b> <br/>
Mode de compression du fichier entrant. Implique de préciser un algorthime.
<br/>
> <b>-d</b>, <b>\-\-decompress</b> <br/>
Mode de décompression du fichier entrant. <br/>
> <b>-s</b>, <b>\-\-statistics</b> <br/>
Affiche les statistiques de la compression ou de la décompression effectuée sur
la sortie standard. <br/>
> <b>-i</b> <i>INPUT FILE</i>, <b>\-\-input=</b><i>INPUT FILE</i> <br/>
Chemin vers le fichier entrant à traiter. <br/>
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

> <b>compressor -c -i</b> <i>env/corpus/text.txt</i> <b>-o</b> <i>text.cmp</i>
> <b>\-\-LZMA -s</b>

> <b>compressor \-\-decompress \-\-input=</b><i>"text.cmp"</i>
> <b>\-\-output=</b><i>"text.txt"</i>
