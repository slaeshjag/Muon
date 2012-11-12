RM	= rm

all:
	@./makemaps.sh
	@mkdir -p map_build
	@cp -R *.ldi map_build
	@rm -Rf ../maps
	@mv -f map_build ../maps

clean:
	@$(RM) -f *.ldi
	@$(RM) -f map_build
