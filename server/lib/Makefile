# Project: muon
include $(TOPDIR)/config.mk

INCS		=	-I../

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@echo

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo ""

%.o: %.c %.h
	@echo " [ CC ]	server/lib/$<"
	@$(CC) $(CFLAGS) $(INCS) -c -o $@ $< 
