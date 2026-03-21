preset:
	@cmake --preset dev

dev:
	@cmake --build build/dev --target run 

dev-build:
	@cmake --build build/dev

clean:
	@cmake --build build/dev --target clean

clear:
	@rm -rf build