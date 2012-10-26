# Project: muon

MAKEFLAGS += --no-print-directory

SERVER	= ../server/server.a
LIBS	= -L"/usr/lib"
INCS	= -I"/usr/include"
BIN	= muon
DBGFLAGS= -g -D__DEBUG__
CFLAGS 	+= -Wall $(INCS) $(DBGFLAGS)
LDFLAGS += -ldarnit
RM	= rm -f

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@darnit-fsimage maps map.ldi
	@echo " [ CD ] ui/"
	@make -C ui/
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(OBJFILES) ui/*.o $(SERVER) -o $(BIN) $(LIBS) $(LDFLAGS)
	@echo " Done."
	@echo

clean:
	@echo " [ CD ] ui/"
	@make -C ui/ clean
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
