src_files:=$(wildcard *.c)
out_files:=$(patsubst %.c,%.o,$(src_files))
CXX:=g++
final_file:= Client
$(final_file): $(out_files)
	$(CXX) $^ -o $@ -lcrypt
%.o:%.c
	$(CXX) -c $^ -o $@ -Wall -D DEBUG
.PHONY: rebuild clean
rebuild:clean all
clean: 
	rm -rf $(out_files)
