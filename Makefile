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
CALLGRIND_OUT = callgrind.out

## Compilation ................................................................:

# MODE : RELEASE, PROFILER, DEBUG
CC = gcc
CC_MODE = RELEASE
TAG_MODE = .$(CC_MODE)

INC_FLAGS = -I$(INC_PATH)
DEP_FLAGS = -MMD -MP

RELEASE_CFLAGS 	 = -O2 -DNDEBUG -flto
RELEASE_LDFLAGS  = -s -flto

PROFILER_CLFAGS  = -O2 -g3 -DNDEBUG -fno-inline
PROFILER_LDFLAGS =

DEBUG_CFLAGS     = -Og -g3 -Wall
DEBUG_LDFLAGS 	 =

GPROF 		 =
GPROF_CFLAGS 	 = -pg
GPROF_LDFLAGS 	 = -pg

CFLAGS  = $(INC_FLAGS) $(DEP_FLAGS)
LDFLAGS =

ifeq '$(CC_MODE)' "RELEASE"
    CFLAGS  += $(RELEASE_CFLAGS)
    LDFLAGS += $(RELEASE_LDFLAGS)
else ifeq '$(CC_MODE)' "PROFILER"
    CFLAGS  += $(PROFILER_CLFAGS)
    LDFLAGS += $(PROFILER_LDFLAGS)
    ifdef GPROF
	TAG_MODE = .GPROF
	CFLAGS  += $(GPROF_CFLAGS)
	LDFLAGS += $(GPROF_LDFLAGS)
    endif
else ifeq '$(CC_MODE)' "DEBUG"
    CFLAGS  += $(DEBUG_CFLAGS)
    LDFLAGS += $(DEBUG_LDFLAGS)
else
    $(error Le mode spécifié est invalide. Modes disponibles : RELEASE, \
	PROFILER, DEBUG)
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
	@sha1sum $(FILE_ORIG) $(FILE_DCMP)
	@wc $(FILE_ORIG)
	@wc $(FILE_CMPR)
	@wc $(FILE_DCMP)

## Benchmark ..................................................................:

benchmark : MODE_RELEASE
	@make --directory="$(BENCH_PATH)" --no-print-directory

## Compilation ................................................................:

compil : pre-compil $(EXEC)

$(EXEC) : $(OBJ) 
	@echo "--> Édition des liens dans '$(EXEC)' :"
	$(CC) $^ -o $(EXEC) $(LDFLAGS)
	@echo "--> Compilation en mode '$(CC_MODE)' effectuée."

$(OBJ_PATH)%.o : $(SRC_PATH)%.c
	@echo "--> Compilation de '$<' :"
	$(CC) -c $< -o $@ $(CFLAGS)

## Compilation modale .........................................................:

pre-compil :
	@if [ ! -e $(TAG_MODE) ]; then \
	    make clean --no-print-directory; \
	    touch $(TAG_MODE); \
	fi

MODE_% :
	@make compil 'CC_MODE=$(@:MODE_%=%)' --no-print-directory

## Dépendances ................................................................:

-include $(OBJ:%.o=%.d)

## Nettoyage ..................................................................:

clean :
	@echo "--> Suppression des fichier temporaires de $(PROJECT) :"
	rm -f $(OBJ_PATH)*.o $(OBJ_PATH)*.d $(SRC_PATH)*~ $(INC_PATH)*~ \
	    gmon.out $(CALLGRIND_OUT) .RELEASE .PROFILER .DEBUG .GPROF
	find . -name .fuse_hidden* -exec rm -f '{}' \;

mrproper : clean
	@echo "--> Suppression de l'exécutable et des fichiers produits" \
	    "de $(PROJECT) :"
	rm -f $(EXEC) $(OUT_PATH)*
	@make clean --directory="$(BENCH_PATH)" --no-print-directory
	@make clean --directory="$(DOC_PATH)" --no-print-directory
	@echo "--> Nettoyage complet du dossier de travail de $(PROJECT)" \
	    "effectué !"

## Debugger & Profiler ........................................................:

gdb : MODE_DEBUG
	@echo "--> Debbugage avec $@ :"
	$@ --args $(EXEC) $(ARGS)

valgrind-p1 : MODE_DEBUG
	@echo "--> Debbugage avec $@ (profile 1) :"
	valgrind --tool=memcheck --leak-resolution=high \
	    --show-possibly-lost=yes --show-reachable=yes $(EXEC) $(ARGS)

valgrind-p2 : MODE_DEBUG
	@echo "--> Debbugage avec $@ (profile 2) :"
	valgrind --tool=memcheck --leak-resolution=high --leak-check=full \
	    --show-possibly-lost=yes --show-reachable=yes --track-origins=yes \
	    $(EXEC) $(ARGS)

kcachegrind : MODE_PROFILER
	@echo "--> Profilage avec callgrind :"
	valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes \
	    --callgrind-out-file="$(CALLGRIND_OUT)" $(EXEC) $(ARGS)
	@echo "--> Visionnage du profilage avec callgrind :"
	callgrind_annotate --auto=yes $(CALLGRIND_OUT)
	@echo "--> Visionnage du profilage avec $@ :"
	$@ $(CALLGRIND_OUT) &

gprof :
	@make run --no-print-directory 'CC_MODE=PROFILER' 'GPROF=1'
	@echo "--> Visionnage des résultats du profilage :"
	$@ $(EXEC)

## Présentation ...............................................................:

indent :
	@echo "--> Reformatage de la présentation du code (paramètres dans" \
	    ".indent.pro) :"
	$@ $(SRC_PATH)* $(INC_PATH)*

## Documentation ..............................................................:

doc :
	@make --directory="$(DOC_PATH)" --no-print-directory MASTER=1

man :
	@echo "--> Affichage du manuel de $(PROJECT) :"
	@# $@ $(MAN_PATH)$(EXE_NAME).1
	groff -k -Tutf8 -man $(MAN_PATH)$(EXE_NAME).1 | less

## Aide .......................................................................:

help :
	@echo "Affichage de l'aide de $(PROJECT) :\n"
	@echo "La variable \"CC_MODE\" peut être positionné à \"RELEASE\","
	@echo "\"PROFILER\" ou encore à \"DEBUG\" en fonction du mode voulu"
	@echo "pour la compilation.\n"
	@echo "Cibles :"
	@echo "\tmake [run] [ARGS=ARGUMENTS]"
	@echo "\t\tLance le programme avec les arguments de la variable ARGS."
	@echo "\n\tmake test [FILE_DIR=PATH] [FILE_NAME=NAME]"
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
	@echo "\n\tmake kcachegrind [ARGS=ARGUMENTS]"
	@echo "\t\tLance le profilage avec callgrind, affiche les résultats"
	@echo "\t\tbruts sur la sortie standard, et lance la visualisation avec"
	@echo "\t\tkcachegrind."
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
