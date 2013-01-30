# Project: muon
include $(TOPDIR)/config.mk
LDFLAGS		+=	-ldarnit

SERVER		=	../server/server.a
BIN		=	../muon

DEPENDS		=

ifeq ($(strip $(OS)), Windows_NT)
	LDFLAGS	+=	-mwindows
	DEPENDS +=	muon.res
endif

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES) $(DEPENDS)
	@echo ""
	@echo " [ CD ] client/ui/"
	+@make -C ui/
	@echo " [ CD ] client/menu/"
	+@make -C menu/
	@echo " [ LD ]	client/$(BIN)"
	@$(CC) $(OBJFILES) $(DEPENDS) ui/*.o menu/*.o $(SERVER) -o $(BIN) $(LIBS) $(LDFLAGS)
	@echo " Done."
	@echo

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(OBJFILES)
	@$(RM) $(BIN)
	@$(RM) $(DEPENDS)
	@echo ""
	@echo " [ CD ] client/ui/"
	+@make -C ui/ clean
	@echo " [ CD ] client/menu/"
	+@make -C menu/ clean

%.o: %.c %.h
	@echo " [ CC ]	client/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 

muon.res:
	@windres ../res/muon.rc -O coff -o muon.res
