# Variables ====================================================================

## Algorithmes ................................................................:

ALGOS = LZMA

## Fichiers utilisés ..........................................................:

BENCH_SCRIPT = ./benchmark.sh
BENCH_OUTPUT_TYPE = log
BENCH_OUTPUT = $(BENCH_SCRIPT:%.sh=%_out.$(BENCH_OUTPUT_TYPE))

GNUPLOT_SCRIPT = ./benchmark.gnu
GNUPLOT_OUTPUT_TYPE = svg
GNUPLOT_OUTPUT = $(GNUPLOT_SCRIPT:%.gnu=%_out.$(GNUPLOT_OUTPUT_TYPE))

## Visionnage .................................................................:

SVG_VIEWER = firefox
#SVG_VIEWER = display
#SVG_VIEWER = gpicview

# Cibles =======================================================================

.PHONY : clean

## Visionnage .................................................................:

view : create
	@echo "--> Visionnage des statistiques du benchmark :"
	$(SVG_VIEWER) $(GNUPLOT_OUTPUT) &

## Création ...................................................................:

create : $(GNUPLOT_OUTPUT)

$(GNUPLOT_OUTPUT) : $(BENCH_OUTPUT)
	@echo "--> Génération d'histogramme à partir du benchmark :"
	gnuplot -e "file_in='$(BENCH_OUTPUT)'; file_out='$(GNUPLOT_OUTPUT)'" \
	    $(GNUPLOT_SCRIPT)

$(BENCH_OUTPUT) :
	@echo "--> Lancement du benchmark des algorithmes de $(PROJECT) :"
	$(BENCH_SCRIPT) "$(ALGOS)"

## Nettoyage ..................................................................:

clean :
	@echo "--> Suppression des benchmarks de $(PROJECT) :"
	rm -f *.$(BENCH_OUTPUT_TYPE) *.$(GNUPLOT_OUTPUT_TYPE)
