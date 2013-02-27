# Project: muon
include $(TOPDIR)/config.mk

LIBOUT		=	ai.a

SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES) ai_lib
	@echo " Done."
	@echo

ai_lib:
	@echo " [ AR ] $(LIBOUT)"
	@$(AR) -cr $(LIBOUT) $(OBJFILES)

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ] $(LIBOUT)"
	@$(RM) $(LIBOUT)

%.o: %.c %.h
	@echo " [ CC ]	server/ai/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
