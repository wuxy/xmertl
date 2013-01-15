obj-m += serial/serial.o
obj-m += serial/linux/luart.o
obj-m += rtfifo/rtfifo.o
obj-m += rtfifo/linux/lfifo.o
obj-m += shm/shm.o
obj-m += shm/linux/lshm.o

include $(XMDEV_PATH)/rules.mk

clean:
	@$(RM) -rf *~ .tmp_versions Modules.symvers
	@$(MAKE) -C rtfifo clean	
	@$(MAKE) -C serial clean	
	@$(MAKE) -C shm clean	
