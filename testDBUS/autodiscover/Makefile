.PHONY: all
.PHONY:gen 
FLAGS=$(shell pkg-config --libs --cflags gio-2.0 gio-unix-2.0 glib-2.0)

all: server_cups server_dummy front_end

gen: genback genfront 

genback:
	gdbus-codegen --generate-c-code backend_interface --c-namespace Print --interface-prefix com.Print. com.Backend.xml

genfront:
	gdbus-codegen --generate-c-code frontend_interface --c-namespace Print --interface-prefix com.Print. com.Frontend.xml

%.o: %.c
	gcc -o $@ $^ -c $(FLAGS)

server_cups: server_cups.o backend_interface.o 
	gcc -o $@ $^ $(FLAGS)

server_dummy: server_dummy.o backend_interface.o 
	gcc -o $@ $^ $(FLAGS)

front_end: front_end.o frontend_interface.o backend_interface.o
	gcc -o $@ $^ $(FLAGS)


clean:
	rm -f *.o server_cups server_dummy front_end