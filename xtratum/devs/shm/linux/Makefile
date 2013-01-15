obj-m += lshm.o

current_dir = $(shell pwd)

all: 
	@$(MAKE) -C $(XM_LINUX_DIR) SUBDIRS=$(current_dir) modules
	@$(MAKE) -C examples all
	@$(RM) *.o *.mod.* *.symvers
install:
	@cp *.ko $(RULES_DIR)/modules
clean:
	@$(MAKE) -C examples clean 
	@$(RM) *.o *.ko *.mod.* *.symvers
	
distclean:
	@$(MAKE) -C examples distclean
	@$(RM) *.o *.ko *.mod.* *.symvers *.cmd

include $(RULES_DIR)/rules.mk	
