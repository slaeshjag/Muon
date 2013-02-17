# Project: muon
include $(TOPDIR)/config.mk
LDFLAGS		+=	-ldarnit

BIN		=	../muon-maped

DEPENDS		=

ifeq ($(strip $(OS)), Windows_NT)
	LDFLAGS	+=	-mwindows
	DEPENDS +=	muon-maped.res
endif

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES) $(DEPENDS)
	@echo ""
	@echo " [ LD ]	maped/$(BIN)"
	@$(CC) $(OBJFILES) $(DEPENDS) ../client/platform.o ../client/ui/*.o -o $(BIN) $(LDFLAGS)
	@echo " Done."
	@echo

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(OBJFILES)
	@$(RM) $(BIN)
	@$(RM) $(DEPENDS)

%.o: %.c %.h
	@echo " [ CC ]	maped/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 

muon-maped.res:
	@windres ../res/windows/muon-maped.rc -O coff -o muon-maped.res
