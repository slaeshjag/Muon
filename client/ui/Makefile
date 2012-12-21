# Project: muon

MAKEFLAGS += --no-print-directory
DBGFLAGS= -g -D__DEBUG__
CFLAGS 	+= -Wall $(INCS) $(DBGFLAGS)
LDFLAGS += -ldarnit
RM	= rm -f

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@echo ""

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo ""

%.o: %.c %.h
	@echo " [ CC ]	client/ui/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
