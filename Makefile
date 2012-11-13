MAKEFLAGS += --no-print-directory
PREFIX = /usr/local
DATAPATH=/usr/share/games/muon

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
	@install -m 0755 -D res/FreeMonoBold.ttf $(DATAPATH)/res/FreeMonoBold.ttf
	@install -m 0755 -D res/FreeMono.ttf $(DATAPATH)/res/FreeMono.ttf
	@install -m 0755 -D res/chat.png $(DATAPATH)/res/chat.png
	@install -m 0755 -D res/mouse.png $(DATAPATH)/res/mouse.png
	@install -m 0755 -D res/icon.png $(DATAPATH)/res/icon.png
	@install -m 0755 -d $(DATAPATH)/maps
	@install -m 0755 -D -t $(DATAPATH)/maps/ maps/* 
clean:
	@echo " [ CD ] server/"
	+@make -C server/ clean
	@echo " [ CD ] client/"
	+@make -C client/ clean
	@echo " [ CD ] res/"
	+@make -C res/ clean
	@echo " [ RM ] maps/"
	@rm -Rf maps/
