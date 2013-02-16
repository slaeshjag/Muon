# Project: muon
include config.mk
MAKEFLAGS	+=	--no-print-directory

TOPDIR		=	$(shell pwd)
export TOPDIR

.PHONY: all server install pandora msi clean

all:
	@echo " [ CD ] server/"
	+@make -C server/
	@echo " [ CD ] client/"
	+@make -C client/
	@echo " [ CD ] res/"
	+@make -C res/

server:
	@echo " [ CD ] server/"
	+@make -C server/ server_standalone
	
install:
	@echo " [INST] muon"
	@install -m 0755 -D muon $(PREFIX)/bin/muon
	@echo " [INST] muon-server"
	@install -m 0755 -D muon-server $(PREFIX)/bin/muon-server
	@echo " [INST] muon resources"
	@install -m 0744 -D res/FreeMonoBold.ttf $(DATAPATH)/res/FreeMonoBold.ttf
	
	@install -m 0744 -D res/mouse.png $(DATAPATH)/res/mouse.png
	@install -m 0744 -D res/target.png $(DATAPATH)/res/target.png
	@install -m 0744 -D res/icon.png $(DATAPATH)/res/icon.png
	
	@install -m 0744 -D res/chat.png $(DATAPATH)/res/chat.png
	@install -m 0744 -D res/flare.png $(DATAPATH)/res/flare.png
	@install -m 0744 -D res/nuke.png $(DATAPATH)/res/nuke.png
	@install -m 0744 -D res/radar.png $(DATAPATH)/res/radar.png
	
	@install -m 0744 -D res/lang.stz $(DATAPATH)/res/lang.stz
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
	@cp res/lang.stz muon-pnd/res
	
	@cp res/chat.png muon-pnd/res
	@cp res/flare.png muon-pnd/res
	@cp res/nuke.png muon-pnd/res
	@cp res/radar.png muon-pnd/res
	@cp res/mouse.png muon-pnd/res
	@cp res/target.png muon-pnd/res
	@cp res/icon.png muon-pnd/res
	@cp res/pandora/dragonbox_compo.png muon-pnd/res
	
	@cp res/pandora/PXML.xml muon-pnd
	@cp res/pandora/run muon-pnd
	@cp maps/*.ldi muon-pnd/maps
	@cp /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr/lib/libdarnit.so muon-pnd/lib
	@mksquashfs muon-pnd/* muon.pnd > /dev/null
	@cat muon-pnd/PXML.xml >> muon.pnd
	@cat muon-pnd/res/icon.png >> muon.pnd
	@rm -Rf muon-pnd
	
msi:
	@echo " [MSI ] muon.msi"
	@mkdir -p res/windows/installer/build
	@mkdir -p res/windows/installer/build/res
	@mkdir -p res/windows/installer/build/maps
	
	@unix2dos -n README.md res/windows/installer/build/README.txt
	@unix2dos -n COPYING res/windows/installer/build/COPYING.txt
	
	@strip -o res/windows/installer/build/muon.exe muon.exe
	@strip -o res/windows/installer/build/muon-server.exe muon-server.exe
	
	@cp res/*.png res/windows/installer/build/res/
	@cp res/FreeMonoBold.ttf res/windows/installer/build/res/
	@cp res/lang.stz res/windows/installer/build/res/
	@cp maps/*.ldi res/windows/installer/build/maps/
	
	@cd res/windows/installer/ && candle -nologo "muon.wxs" -out "muon.wixobj" -ext WixUIExtension
	@cd res/windows/installer/ && light -nologo "muon.wixobj" -out "muon.msi" -ext WixUIExtension
	@mv res/windows/installer/muon.msi .


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
