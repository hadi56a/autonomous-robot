all: doc prg tests

prg:
	$(MAKE) -C algo
	$(MAKE) -C elec

tests:
	$(MAKE) -C algo tests
	$(MAKE) -C elec tests

doc:
	$(MAKE) -C algo doc

clean:
	$(MAKE) -C algo clean
	$(MAKE) -C elec clean
