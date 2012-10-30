# Project: muon

MAKEFLAGS += --no-print-directory

BIN  = server
LIBOUT = server.a
STANDALONE = standalone/*.o
LIBOBJ = lib/*.o
DEBUGFLAGS = -O0 -g -D__DEBUG__
CFLAGS += $(INCS) $(DEBUGFLAGS) -Wall
LDFLAGS +=  -lm
RM = rm -f

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@echo ""
	@echo " [ CD ] server/standalone/"
	+@make -C standalone/
	@echo ""
	@echo " [ CD ] server/lib/"
	+@make -C lib/
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(CFLAGS) $(OBJFILES) $(STANDALONE) -o $(BIN) $(LIBS) $(LDFLAGS)
	@echo " [ AR ] $(LIBOUT)"
	@$(AR) -cq $(LIBOUT) $(OBJFILES) $(LIBOBJ)
	@echo " Done."
	@echo

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(BIN)
	@echo " [ RM ] $(LIBOUT)"
	@$(RM) $(LIBOUT)
	@echo ""
	@echo " [ CD ] server/standalone/"
	@make -C standalone/ clean
	@echo " [ CD ] server/lib/"
	@make -C lib/ clean

%.o: %.c %.h
	@echo " [ CC ]	server/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
