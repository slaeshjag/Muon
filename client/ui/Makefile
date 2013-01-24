# Project: muon
include $(TOPDIR)/config.mk

SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.o)
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
