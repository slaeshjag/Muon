# Project: muon

CC   = gcc
LIBS =  -L"/usr/lib"
INCS =  -I"/usr/include" -I../
DEBUGFLAGS = -g -D__DEBUG__
CFLAGS = $(INCS)
LDFLAGS = 
RM = rm -f

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@echo ""

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo

%.o: %.c %.h
	@echo " [ CC ]	server/standalone/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
