all:
	$(MAKE) -C ./codecs/pure_wave
	$(MAKE) -C ./kernel


run_app:
	./bin/kernel
