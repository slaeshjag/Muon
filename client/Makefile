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
	@echo ""
	@echo " [ CD ] client/ui/"
	@make -C ui/
	@echo " [ LD ]	client/$(BIN)"
	@$(CC) $(OBJFILES) ui/*.o $(SERVER) -o $(BIN) $(LIBS) $(LDFLAGS)
	@echo " Done."
	@echo

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(OBJFILES)
	@$(RM) $(BIN)
	@echo ""
	@echo " [ CD ] client/ui/"
	@make -C ui/ clean

%.o: %.c %.h
	@echo " [ CC ]	client/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
