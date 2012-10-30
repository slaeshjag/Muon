# Project: muon

MAKEFLAGS += --no-print-directory

BIN  = server-tester
DEBUGFLAGS = -g -D__DEBUG__
CFLAGS += $(INCS) -O0 -g -Wall
LDFLAGS =  -lm
RM = rm -f

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@echo ""
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(OBJFILES) -o $(BIN) $(LIBS) $(LDFLAGS)
	@echo " Done."
	@echo

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(BIN)
	@echo

%.o: %.c %.h
	@echo " [ CC ]	server/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
