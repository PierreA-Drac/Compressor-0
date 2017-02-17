# Sortie du graphique
#set terminal x11 size 1280,720
set terminal svg size 1280,720
set output "benchmark_out.svg"

# Style du graphique
set style data histogram 
set style fill solid 1 border 0
set grid ytics

# Titre et légendes
set title "Statistiques de compression"
set ylabel "Pourcentage (plus faible est mieux)"
set key on outside horizontal center bottom spacing 5
set xtics rotate by -45

# Fichier de données
set datafile separator "|"

# Récupération des statistiques pour les calculs
stats 'benchmark_out.log' using 5:6 nooutput

# Échelle
unset autoscale y
set yrange [0:100]

# Plotting
plot 'benchmark_out.log' using ($4*100/$3):xticlabels(stringcolumn(1)." et ".stringcolumn(2)) title "Taux de compression" lt rgb "dark-red", \
     '' using ($5*100/STATS_max_x) title "Temps de compression" lt rgb "dark-gray", \
     '' using ($6*100/STATS_max_y) title "Espace memoire utilise" lt rgb "dark-plum"

### Si on est dans une fenêtre X11

# Sauvegarde en SVG (on ne pourra plus zoomer avec la fenêtre X11)
#set terminal svg size 1280,720
#set output "benchmark.svg"
#replot

# Sortie quand on appuie sur "q+ENTER"
#pause -1
