# Variables ====================================================================

## Structure du projet ........................................................:

export PROJECT = Compressor-0
EXE_NAME = compressor-0

export WD_PATH = $(shell pwd)/
export SRC_PATH = src/
export INC_PATH = inc/
OBJ_PATH = obj/
DOC_PATH = doc/
MAN_PATH = man/
EXE_PATH = exe/
OUT_PATH = $(EXE_PATH)out/
BENCH_PATH = bench/

EXEC = $(EXE_PATH)$(EXE_NAME)
export SRC = $(shell find $(SRC_PATH)*.c)
export INC = $(shell find $(INC_PATH)*.h)
OBJ = $(SRC:$(SRC_PATH)%.c=$(OBJ_PATH)%.o)

## Compilation ................................................................:

DEBUG = 1

CC = gcc
INC_FLAGS = -I$(INC_PATH)
DEP_FLAGS = -MMD -MP
PROF_FLAGS = -pg

CFLAGS = $(INC_FLAGS) $(DEP_FLAGS)
LDFLAGS =
ifeq ($(DEBUG), 1)
    CFLAGS += -g3 -Wall -Og
    LDFLAGS +=
else
    CFLAGS += -flto -DNDEBUG -O2
    LDFLAGS += -flto -s
endif

## Lancement ..................................................................:

FILE_DIR = env/text/
FILE_NAME = Shakespeare.txt
FILE_ORIG = '$(FILE_DIR)$(FILE_NAME)'
FILE_CMPR = '$(OUT_PATH)$(FILE_NAME).cmp'
FILE_DCMP = '$(OUT_PATH)$(FILE_NAME)'

ARGS = $(ARGS_CMPR)
ARGS_CMPR = -c -i $(FILE_ORIG) -o $(FILE_CMPR) --RLE -s
ARGS_DCMP = -d -i $(FILE_CMPR) -o $(FILE_DCMP) --RLE -s

# Cibles =======================================================================

.PHONY : clean mrproper indent doc man

## Lancement ..................................................................:

run : compil 
	@echo "--> Lancement de '$(EXEC)' :"
	$(EXEC) $(ARGS)

test : compil
	@echo "--> Fichier original :"
	@head $(FILE_ORIG)
	@tail $(FILE_ORIG)
	@echo "--> Compression :"
	$(EXEC) $(ARGS_CMPR)
	@echo "--> Décompression :"
	$(EXEC) $(ARGS_DCMP)
	@echo "--> Fichier décompressé :"
	@head $(FILE_DCMP)
	@tail $(FILE_DCMP)
	@echo "--> Statistiques des fichiers :"
	@wc $(FILE_ORIG)
	@wc $(FILE_CMPR)
	@wc $(FILE_DCMP)

## Benchmark ..................................................................:

benchmark :
	make --directory="$(BENCH_PATH)" --no-print-directory

## Compilation ................................................................:

compil : $(EXEC)

$(EXEC) : $(OBJ) 
	@echo "--> Édition des liens dans '$(EXEC)' :"
	$(CC) $^ -o $(EXEC) $(LDFLAGS)

$(OBJ_PATH)%.o : $(SRC_PATH)%.c
	@echo "--> Compilation de '$<' :"
	$(CC) -c $< -o $@ $(CFLAGS)

## Dépendances ................................................................:

-include $(OBJ:%.o=%.d)

## Nettoyage ..................................................................:

clean :
	@echo "--> Suppression des fichier temporaires de $(PROJECT) :"
	rm -f $(OBJ_PATH)*.o $(OBJ_PATH)*.d $(SRC_PATH)*~ $(INC_PATH)*~ gmon.out
	find . -name .fuse_hidden* -exec rm -f '{}' \;

mrproper : clean
	@echo "--> Suppression de l'exécutable et des fichiers produits" \
	    "de $(PROJECT) :"
	rm -f $(EXEC) $(OUT_PATH)*
	make clean --directory="$(BENCH_PATH)" --no-print-directory
	make clean --directory="$(DOC_PATH)" --no-print-directory
	@echo "--> Nettoyage complet du dossier de travail de $(PROJECT)" \
	    "effectué !"

## Debugger & Profiler ........................................................:

gdb : compil
	@echo "--> Debbugage avec $@ :"
	gdb --args $(EXEC) $(ARGS)

valgrind-p1 : compil
	@echo "--> Debbugage avec $@ (profile 1) :"
	valgrind --tool=memcheck --leak-resolution=high \
	    --show-possibly-lost=yes --show-reachable=yes $(EXEC) $(ARGS)

valgrind-p2 : compil
	@echo "--> Debbugage avec $@ (profile 2) :"
	valgrind --tool=memcheck --leak-check=full --leak-resolution=high \
	    --show-possibly-lost=yes --show-reachable=yes --track-origins=yes \
	    $(EXEC) $(ARGS)

gprof :
	make clean --no-print-directory
	@echo "--> Compilation avec les flags nécéssaires à $@ :"
	make run --no-print-directory CFLAGS="$(CFLAGS) $(PROF_FLAGS)" \
	    LDFLAGS="$(LDFLAGS) $(PROF_FLAGS)"
	@echo "--> Visionnage des résultats du profilage :"
	gprof $(EXEC)

## Présentation ...............................................................:

indent :
	@echo "--> Reformatage de la présentation du code (paramètres dans" \
	    ".indent.pro) :"
	indent $(SRC_PATH)* $(INC_PATH)*

## Documentation ..............................................................:

doc :
	make --directory="$(DOC_PATH)" --no-print-directory MASTER=1

man :
	@echo "--> Affichage du manuel de $(PROJECT) :"
	@# man $(MAN_PATH)$(EXE_NAME).1
	groff -k -Tutf8 -man $(MAN_PATH)$(EXE_NAME).1 | less

## Aide .......................................................................:

help :
	@echo "Affichage de l'aide de $(PROJECT) :\n"
	@echo "Il est possible de switcher la variable DEBUG du Makefile, de 1,"
	@echo "pour activer les flags de débuggage, à 0, pour les désactiver et"
	@echo "compiler en mode release.\n"
	@echo "Cibles :"
	@echo "\tmake [run] [ARGS=ARGUMENTS]"
	@echo "\t\tLance le programme avec les arguments de la variable ARGS."
	@echo "\n\tmake test [FILE_NAME=NAME] [FILE_DIR=PATH]"
	@echo "\t\tLance une série compression/décompression sur un fichier"
	@echo "\t\tspécifié par les variables FILE_NAME et FILE_DIR."
	@echo "\n\tmake benchmark"
	@echo "\t\tLance les benchmarks sur les algorithmes spécifiés dans le"
	@echo "\t\tMakefile du dossier "bench/" et les fichiers présents dans"
	@echo "\t\tle répertoire "./env/". Affiche le résultat sous forme de"
	@echo "\t\ttexte sur la sortie standard et les histogrammes sur une"
	@echo "\t\timage vectorielle svg."
	@echo "\n\tmake compil"
	@echo "\t\tCompile le programme."
	@echo "\n\tmake clean"
	@echo "\t\tNettoie les fichiers temporaires et de sauvegarde du"
	@echo "\t\tdossier de travail."
	@echo "\n\tmake mrproper"
	@echo "\t\tEffectue le nettoyage d'un "clean" ainsi que l'exécutable,"
	@echo "\t\tles fichiers produits par lui-même, les benchmarks et la"
	@echo "\t\tdocumentation."
	@echo "\n\tmake gdb [ARGS=ARGUMENTS]"
	@echo "\t\tLance gdb pour débugguer avec les arguments de la variable" 
	@echo "\t\tARGS."
	@echo "\n\tmake valgrind-p[1|2] [ARGS=ARGUMENTS]"
	@echo "\t\tLance valgrind avec les arguments de la variables ARGS" 
	@echo "\t\tavec le profil p1 (plus rapide mais moins précis) ou le"
	@echo "\t\tprofil p2 (plus lent mais plus précis)."
	@echo "\n\tmake gprof [ARGS=ARGUMENTS]"
	@echo "\t\tNettoie les fichiers temporaires et relance la compilation"
	@echo "\t\tavec les flags necéssaires à gprof, puis affiche le résultat"
	@echo "\t\tdu profilage sur la sortie standard."
	@echo "\n\tmake indent"
	@echo "\t\tLance le progamme indent sur les fichiers sources et" 
	@echo "\t\theaders avec les paramètres du fichier .indent.pro."
	@echo "\n\tmake doc"
	@echo "\t\tAffiche la documentation du projet (lance la génération si" 
	@echo "\t\tnon générée précédemment)."
	@echo "\n\tmake man"
	@echo "\t\tAffiche le manuel du programme."
	@echo "\n\tmake help"
	@echo "\t\tAffiche l'aide du Makefile."
