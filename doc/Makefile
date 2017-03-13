# Variables ====================================================================

## Dépendances ................................................................:

DEPS = Doxyfile
# Si on appelle ce Makefile depuis le Makefile maître :
ifdef MASTER
    DEPS += $(INC:%.h=$(WD_PATH)%.h) $(SRC:%.c=$(WD_PATH)%.c) $(WD_PATH)README.md Doxyfile
endif

## Logiciels ..................................................................:

PDF_VIEWER = evince
INTERNET_BROWSER = firefox

# Cibles =======================================================================

.PHONY : clean

## Visionnage .................................................................:

view : compil
	@echo "--> Visionnement de la documentation de $(PROJECT) :"
	$(INTERNET_BROWSER) html/index.html &
	$(PDF_VIEWER) latex/refman.pdf &

## Compilation ................................................................:

compil : html/index.html latex/refman.pdf

html/index.html : $(DEPS)
	@echo "--> Création de la documentation de $(PROJECT) :"
	doxygen Doxyfile
	
latex/refman.pdf : $(DEPS)
	@echo "--> Compilation de la documentation en PDF de $(PROJECT)" \
	    "dans 'latex/' :"
	make --directory="latex/" --no-print-directory

## Nettoyage ..................................................................:

clean:
	@echo "--> Suppression de la documentation de $(PROJECT) :"
	rm -f -r html/ latex/
