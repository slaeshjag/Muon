MAKEFLAGS += --no-print-directory

all:
	@echo " [ CD ] server/"
	@make -C server/
	@echo " [ CD ] client/"
	@make -C client/
	@echo " [ CD ] res/"
	@make -C res/
	
clean:
	@echo " [ CD ] server/"
	@make -C server/ clean
	@echo " [ CD ] client/"
	@make -C client/ clean
