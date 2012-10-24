# Project: muon

LIBS	= -L"/usr/lib"
INCS	= -I"/usr/include"
BIN	= muon
DBGFLAGS= -g -D__DEBUG__
CFLAGS 	+= $(INCS)
LDFLAGS += -ldarnit
RM	= rm -f

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(OBJFILES) -o $(BIN) $(LIBS) $(LDFLAGS)
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
