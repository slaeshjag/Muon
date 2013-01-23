# Project: muon
include $(TOPDIR)/config.mk

BIN		=	../muon-server
LIBOUT		=	server.a
STANDALONE	=	standalone/*.o
LIBOBJ		=	lib/*.o

DEPENDS		=	server_tester

ifeq ($(strip $(OS)), Windows_NT)
	DEPENDS =
endif

SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES) $(DEPENDS)
	@echo ""
	@echo " [ CD ] server/standalone/"
	+@make -C standalone/
	@echo ""
	@echo " [ CD ] server/lib/"
	+@make -C lib/
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(CFLAGS) $(OBJFILES) $(STANDALONE) -o $(BIN) $(LIBS) $(LDFLAGS)
	@echo " [ AR ] $(LIBOUT)"
	@$(AR) -cr $(LIBOUT) $(OBJFILES) $(LIBOBJ)
	@echo " Done."
	@echo
	
server_tester:
	@echo " [ CD ] server/server_tester/"
	+@make -C server_tester/

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(BIN)
	@echo " [ RM ] $(LIBOUT)"
	@$(RM) $(LIBOUT)
	@echo ""
	@echo " [ CD ] server/standalone/"
	+@make -C standalone/ clean
	@echo " [ CD ] server/lib/"
	+@make -C lib/ clean
	@echo " [ CD ] server/server_tester/"
	+@make -C server_tester/ clean

%.o: %.c %.h
	@echo " [ CC ]	server/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
