
all:mkstalib
	gcc main.c -o comporttool libcomport.a
	rm -r *.o
mkstalib:comport.o
	gcc comport.c -c -o comport.o
comport.o:
	ar rcs -o libcomport.a comport.o

mkdynlib:
	gcc -shared -fPIC -o libcomport.so comport.c
	export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
	gcc main.c -o comporttool -lcomport
cleanup:
	rm -r *.o *.a *.so
