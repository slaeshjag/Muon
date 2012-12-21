# Project: muon

INCS =  -I../
DEBUGFLAGS = -O0 -g -D__DEBUG__
CFLAGS += $(INCS) $(DEBUGFLAGS)
LDFLAGS += 
RM = rm -f

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
	@$(CC) $(CFLAGS) -c -o $@ $< 
