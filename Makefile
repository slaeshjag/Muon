# Project: muon
include config.mk
MAKEFLAGS	+=	--no-print-directory

TOPDIR		=	$(shell pwd)
export TOPDIR

.PHONY: all server install pandora msi deb clean

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
	@cp res/pandora/muon-1.0.png muon-pnd/
	
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
	
muon-$(VERSION)maemo.deb:
	@#mkdir "muon-$(VERSION)maemo"
	@echo " [DPKG] $@"

$(DEB).deb:
	@mkdir -p $(DEB)/DEBIAN
	@mkdir -p $(DEB)/usr/bin
	@mkdir -p $(DEB)/usr/games
	@mkdir -p $(DEB)/usr/lib
	@mkdir -p $(DEB)/usr/share/doc/muon
	@mkdir -p $(DEB)/usr/share/menu
	@mkdir -p $(DEB)$(DATAPATH)/res
	@mkdir -p $(DEB)$(APPLICATIONSPATH)
	
	@strip muon -o $(DEB)/usr/games/muon
	@strip muon-server -o $(DEB)/usr/bin/muon-server
	@strip `whereis libdarnit.so|sed 's/^.*: //'` -o $(DEB)/usr/lib/libdarnit.so
	@chmod 644 $(DEB)/usr/lib/libdarnit.so
	@cp res/*.png $(DEB)$(DATAPATH)/res
	@#cp res/*.ogg $(DEB)$(DATAPATH)/res
	@cp res/lang.stz $(DEB)$(DATAPATH)/res
	@cp res/FreeMonoBold.ttf $(DEB)$(DATAPATH)/res
	@cp -R maps $(DEB)$(DATAPATH)/
	
	@cat "res/debian/menu" | sed -e 's/\$$BIN/$(subst /,\/,/usr/games/muon)/' > "$(DEB)/usr/share/menu/muon"
	@cp "res/debian/postinst" "$(DEB)/DEBIAN/postinst"
	@cp "res/debian/copyright" "$(DEB)/usr/share/doc/muon/copyright"
	@gzip -9 -c "res/debian/changelog" > "$(DEB)/usr/share/doc/muon/changelog.gz"
	@cat "res/debian/control" | \
		sed	-e 's/\$$VERSION/$(subst .,\.,$(VERSION))/' \
			-e 's/\$$DEPS/$(subst .,\.,$(DEPS))/' \
			-e 's/\$$SIZE/'"`du -k --exclude=DEBIAN $(DEB)/|tail -n 1|cut -f 1`/" \
			-e 's/\$$ARCH/$(ARCH)/' \
			-e 's/\$$SECTION/$(subst /,\/,$(SECTION))/' \
		> "$(DEB)/DEBIAN/control"
	
	@chmod -R g-w $(DEB)
	@echo " [DPKG] $@"
	@fakeroot dpkg -b $(DEB) > /dev/null
	@rm -Rf $(DEB)
	
deb: $(PACKAGE)

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
	@rm -Rf muon.msi
	@rm -Rf *.deb
