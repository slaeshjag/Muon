MAKEFLAGS += --no-print-directory
PREFIX = /usr/local
DATAPATH=/usr/share/games/muon
APPLICATIONSPATH=/usr/share/applications

all:
	@echo " [ CD ] server/"
	+@make -C server/
	@echo " [ CD ] client/"
	+@make -C client/
	@echo " [ CD ] res/"
	+@make -C res/
	
install:
	@echo " [INST] muon"
	@install -m 0755 -D muon $(PREFIX)/bin/muon
	@echo " [INST] muon-server"
	@install -m 0755 -D muon-server $(PREFIX)/bin/muon-server
	@echo " [INST] muon resources"
	@install -m 0744 -D res/FreeMonoBold.ttf $(DATAPATH)/res/FreeMonoBold.ttf
	@install -m 0744 -D res/FreeMono.ttf $(DATAPATH)/res/FreeMono.ttf
	@install -m 0744 -D res/chat.png $(DATAPATH)/res/chat.png
	@install -m 0744 -D res/mouse.png $(DATAPATH)/res/mouse.png
	@install -m 0744 -D res/icon.png $(DATAPATH)/res/icon.png
	@install -m 0755 -d $(DATAPATH)/maps
	@install -m 0744 -D -t $(DATAPATH)/maps/ maps/*
	@install -m 0755 -D res/muon.desktop $(APPLICATIONSPATH)/muon.desktop

pandora: all
	@echo " [PND ] muon.pnd"
	@rm -Rf muon.pnd
	@mkdir -p muon-pnd
	@mkdir -p muon-pnd/res
	@mkdir -p muon-pnd/maps
	@mkdir -p muon-pnd/lib
	@cp muon muon-pnd
	@cp res/FreeMonoBold.ttf muon-pnd/res
	@cp res/FreeMono.ttf muon-pnd/res
	@cp res/chat.png muon-pnd/res
	@cp res/mouse.png muon-pnd/res
	@cp res/icon.png muon-pnd/res
	@cp res/pandora/PXML.xml muon-pnd
	@cp maps/*.ldi muon-pnd/maps
	@cp /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr/lib/libdarnit.so muon-pnd/lib
	@mksquashfs muon-pnd/* muon.pnd > /dev/null
	@cat muon-pnd/PXML.xml >> muon.pnd
	@cat muon-pnd/res/icon.png >> muon.pnd
	@rm -Rf muon-pnd


clean:
	@echo " [ CD ] server/"
	+@make -C server/ clean
	@echo " [ CD ] client/"
	+@make -C client/ clean
	@echo " [ CD ] res/"
	+@make -C res/ clean
	@echo " [ RM ] maps/"
	@rm -Rf maps/
	
	@rm -Rf muon.pnd
