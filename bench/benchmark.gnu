# Paramètres utilisateur =======================================================

# Fichiers d'entrées/sorties. Possibilité de les passer en arguments.
if (!exists("file_in")) {
    file_in = 'benchmark_out.log'
}
if (!exists("file_out")) {
    file_out = 'benchmark_out.svg'
}

# Résolution du graphique de sortie.
adaptative_res = 1      # Résolution adaptive en fonction du nombre
                        # d'histogramme à afficher : 1 activé, 0 désactivé.
res_w_by_histo = 500    # Width par nombre d'histogramme (si activé).
res_w_min = 1280        # Width minimum (si activé).
res_h = 1080            # Height.
res_w = 1920            # Width.

# Colonnes.
col_file = 1        # Nom du fichier.
col_algo = 2        # Nom de l'algorithme.
col_size_orig = 3   # Taille original.
col_size_cmp = 4    # Taille compressé.
col_cmp_time = 5    # Temps de compression.
col_mem_space = 6   # Espace mémoire utilisé.

# Fonctions ====================================================================

# Renvoie x par rapport à y en pourcentage.
x_by_y(x, y) = x*100/y

# Script =======================================================================

# Paramètres du fichier de données.
set datafile separator '|'

# Récupération des statistiques pour les calculs : temps de compression,
# espace mémoire utilisé maximum, et nombre de lignes pour le calcul de la
# résolution.
stats file_in using col_cmp_time:col_mem_space nooutput
cmp_time_max = STATS_max_x
mem_space_max = STATS_max_y
nb_histo = STATS_records
# Un warning sera affiché si on a seulement une ou deux lignes dans le fichier
# de données, c'est normal.

# Redéfinition de res_w en fonction du nombre d'histogramme à produire.
if (adaptative_res == 1) {
    res_w = nb_histo*res_w_by_histo
    res_w = res_w >= res_w_min ? res_w : res_w_min
}

# Sortie du graphique.
#set terminal x11 size res_w,res_h
set terminal svg size res_w,res_h
set output file_out
set encoding utf8

# Style du graphique.
set style data histogram 
set style fill solid 1 border 0
set grid ytics

# Labels
lab_v_off = -6.5  # labels_vertical_offset
lab_rot = -45     # labels_rotation

# Titre et légendes.
set title 'Statistiques de compression'
set ylabel 'Pourcentage (plus faible est mieux)'
set key on outside horizontal center bottom spacing 5 autotitle columnhead
set xtics rotate by -45

# Échelle.
unset autoscale y
set yrange [0:100]

# Plotting.
plot file_in using (x_by_y(column(col_size_cmp), column(col_size_orig))) : xticlabels(stringcolumn(col_file).' et '.stringcolumn(col_algo)) \
         title 'Taille (kB) / Taux de compression (%)' lt rgb 'bisque', \
     '' using ($0 - 0.1) : (x_by_y(column(col_size_cmp), column(col_size_orig)) + lab_v_off) : (stringcolumn(col_size_cmp).' / '.stringcolumn(col_size_orig) \
         .'\n('.sprintf("%d\%", x_by_y(column(col_size_cmp), column(col_size_orig))).')') with labels rotate by lab_rot notitle, \
     '' using (x_by_y(column(col_cmp_time), cmp_time_max)) lt rgb 'dark-gray', \
     '' using ($0 + 0.1) : (x_by_y(column(col_cmp_time), cmp_time_max) + lab_v_off) : col_cmp_time with labels rotate by lab_rot notitle, \
     '' using (x_by_y(column(col_mem_space), mem_space_max)) lt rgb 'dark-plum', \
     '' using ($0 + 0.3) : (x_by_y(column(col_mem_space), mem_space_max) + lab_v_off) : col_mem_space with labels rotate by lab_rot notitle

# Si on est dans une fenêtre X11 ===============================================

# Sauvegarde en SVG (on ne pourra plus zoomer avec la fenêtre X11).
#set terminal svg size res_w,res_h
#set output file_out
#replot

# Sortie quand on appuie sur "q+ENTER".
#pause -1
