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
INC_FLAGS = -I$(INC_PATH)
DEP_FLAGS = -MMD -MP

CC = gcc
LDFLAGS =

CFLAGS = $(INC_FLAGS) $(DEP_FLAGS)
ifeq ($(DEBUG), 1)
    CFLAGS += -Og -g3 -Wall
else
    CFLAGS += -O2 -DNDEBUG
endif

## Lancement ..................................................................:

ARGS = -c -i env/text/Shakespeare.txt --LZMA -s

# Cibles =======================================================================

.PHONY : clean mrproper indent doc man

## Lancement ..................................................................:

run : compil 
	@echo "--> Lancement de '$(EXEC)' :"
	$(EXEC) $(ARGS)

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
	rm -f $(OBJ_PATH)*.o $(OBJ_PATH)*.d $(SRC_PATH)*~ $(INC_PATH)*~

mrproper : clean
	@echo "--> Suppression de l'exécutable et des fichiers produits" \
	    "de $(PROJECT) :"
	rm -f $(EXEC) $(OUT_PATH)*
	make clean --directory="$(BENCH_PATH)" --no-print-directory
	make clean --directory="$(DOC_PATH)" --no-print-directory
	@echo "--> Nettoyage complet du dossier de travail de $(PROJECT)" \
	    "effectué !"

## Debugger ...................................................................:

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
