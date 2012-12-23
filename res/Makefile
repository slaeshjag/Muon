RM	= rm -f

all:
	@./makemaps.sh
	@echo " [DSTZ] res/lang"
	@darnit-stringtable lang lang.stz 2> /dev/null

clean:
	@$(RM) *.ldi
