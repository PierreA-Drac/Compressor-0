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
	$(INTERNET_BROWSER) $(DOC_PATH)html/index.html &
	$(PDF_VIEWER) $(DOC_PATH)latex/refman.pdf &

## Compilation ................................................................:

compil : $(DOC_PATH)html/index.html $(DOC_PATH)latex/refman.pdf

$(DOC_PATH)html/index.html : $(DEPS)
	@echo "--> Création de la documentation de $(PROJECT) :"
	doxygen Doxyfile
	
$(DOC_PATH)latex/refman.pdf : $(DEPS)
	@echo "--> Compilation de la documentation en PDF de $(PROJECT)" \
	    "dans './latex/' :"
	make --directory="$(DOC_PATH)latex/" --no-print-directory

## Nettoyage ..................................................................:

clean:
	@echo "--> Suppression de la documentation de $(PROJECT) :"
	rm -f -r $(DOC_PATH)html/ $(DOC_PATH)latex/
