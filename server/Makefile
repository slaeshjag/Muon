# Project: muon

MAKEFLAGS += --no-print-directory

CC   = gcc
LIBS =  -L"/usr/lib"
INCS =  -I"/usr/include"
BIN  = server
LIBOUT = server.a
STANDALONE = standalone/*.o
LIBOBJ = lib/*.o
DEBUGFLAGS = -g -D__DEBUG__
CFLAGS = $(INCS)
LDFLAGS =  -lm
RM = rm -f

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@echo
	@echo " [ CD ] standalone/"
	@make -C standalone/
	@echo " [ CD ] lib/"
	@make -C lib/
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(OBJFILES) $(STANDALONE) -o $(BIN) $(LIBS) $(LDFLAGS)
	@echo " [ AR ] $(LIBOUT)"
	@$(AR) -cq $(LIBOUT) $(OBJFILES) $(LIBOBJ)
	@echo " Done."
	@echo

clean:
	@echo ""
	@echo " [ CD ] standalone/"
	@make -C standalone/ clean
	@echo " [ CD ] lib/"
	@make -C lib/ clean
	@echo " [ RM ]	$(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(BIN)
	@echo " [ RM ] $(LIBOUT)"
	@$(RM) $(LIBOUT)
	@echo " Done."
	@echo

%.o: %.c %.h
	@echo " [ CC ]	$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
