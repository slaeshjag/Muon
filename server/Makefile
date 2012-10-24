# Project: muon

CC   = gcc
LIBS =  -L"/usr/lib"
INCS =  -I"/usr/include"
BIN  = muon
DEBUGFLAGS = -g -D__DEBUG__
CFLAGS = $(INCS)
LDFLAGS = 
RM = rm -f

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@echo " [ CC ]	$(OBJFILES)"
	@$(CC) $(OBJFILES) -o $(BIN) $(LIBS)
	@echo " Done."
	@echo

clean:
	@echo ""
	@echo " [ RM ]	$(OBJFILES)"
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(OBJFILES)
	@$(RM) $(BIN)
	@echo " Done."
	@echo

%.o: %.c %.h
	@echo " [ CC ]	$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
