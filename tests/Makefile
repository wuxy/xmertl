all: 
	@make -C xtratum_tests
	@make -C partikle_tests
	@make -C fifo_tests
	@make -C shm_tests
	@make -C trace_tests
	@echo `who`  >> .mk.xmertl.log
	@echo ""
	@echo `date` >> .mk.xmertl.log
	export XMERTL_ROOT=${XMERTL_ROOT}

clean:
	@rm -rf .mk.xmertl.log
	@make -C xtratum_tests clean
	@make -C partikle_tests clean
	@make -C fifo_tests clean
	@make -C shm_tests clean
	@make -C trace_tests clean
	
include config.mk
