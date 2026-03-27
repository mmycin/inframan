dev-preset:
	@cmake --preset dev

dev:
	@cmake --build build/dev --target run 

dev-build:
	@cmake --build build/dev

release-preset:
	@cmake --preset release

release:
	@cmake --build build/release --target run

release-build:
	@cmake --build build/release

clean:
	@cmake --build build/dev --target clean
	@cmake --build build/release --target clean

clear:
	@rm -rf build