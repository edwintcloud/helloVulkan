default_target: all

all: generate build-rel build-debug

build-rel:
	cmake -DCMAKE_BUILD_TYPE=Release build
	cd build && make

build-debug:
	cmake -DCMAKE_BUILD_TYPE=Debug build
	cd build && make

generate:
	mkdir -p build
	cd build && cmake -G "Unix Makefiles" ../
