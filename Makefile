DLL_SUFFIX := $(if $(findstring Darwin,$(OS)),.dylib,.so)

all: test

build:
	gcc -c -fPIC src/abt.c -o abt.o
	gcc -shared -o libabt$(DLL_SUFFIX) abt.o

clean:
	rm -f abt.o libabt$(DLL_SUFFIX) test/malloc_mock.o test/libmalloc_mock$(DLL_SUFFIX) test/test

test-build: build
	gcc -c -fPIC test/malloc_mock.c -o test/malloc_mock.o
	gcc -shared -o test/libmalloc_mock$(DLL_SUFFIX) -L. -labt test/malloc_mock.o
	gcc test/test.c -L. -Ltest -labt -lmalloc_mock -o test/test

test: test-build
	./test/test 2>&1 | diff - test/test.txt
