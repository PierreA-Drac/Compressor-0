# Variables ====================================================================

## Algorithmes ................................................................:

ALGOS = LZMA

## Fichiers utilisés ..........................................................:

BENCH_SCRIPT = ./benchmark.sh
BENCH_TYPE_OUTPUT = log
BENCH_OUTPUT = $(BENCH_SCRIPT:%.sh=%_out.$(BENCH_TYPE_OUTPUT))

GNUPLOT_SCRIPT = ./benchmark.gnu
GNUPLOT_TYPE_OUTPUT = svg
GNUPLOT_OUTPUT = $(GNUPLOT_SCRIPT:%.gnu=%_out.$(GNUPLOT_TYPE_OUTPUT))

## Visionnage .................................................................:

SVG_VIEWER = firefox
#SVG_VIEWER = display
#SVG_VIEWER = gpicview

# Cibles =======================================================================

.PHONY : clean

## Visionnage .................................................................:

view : create
	@echo "--> Visionnage des statistiques du benchmark :"
	$(SVG_VIEWER) $(GNUPLOT_SCRIPT:%.gnu=%_out.$(GNUPLOT_TYPE_OUTPUT)) &

## Création ...................................................................:

create : $(GNUPLOT_OUTPUT)

$(GNUPLOT_OUTPUT) : $(BENCH_OUTPUT)
	@echo "--> Génération d'histogramme à partir du benchmark :"
	gnuplot $(GNUPLOT_SCRIPT)

$(BENCH_OUTPUT) :
	@echo "--> Lancement du benchmark des algorithmes de $(PROJECT) :"
	$(BENCH_SCRIPT) "$(ALGOS)"

## Nettoyage ..................................................................:

clean :
	@echo "--> Suppression des benchmarks de $(PROJECT) :"
	rm -f *.$(BENCH_TYPE_OUTPUT) *.$(GNUPLOT_TYPE_OUTPUT)
