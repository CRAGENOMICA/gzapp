# Command lines must start with a '\t' otherwise make shows: "make: *** falta un separador: Alto"
# If Eclipse debug shows: "No source available for main()", then set this to the gcc command: -g -O0
# If make shows "make: Error 1" then be sure that you set the gcc arguments correctly
# If Eclipse says: Errors (1 item)  make: *** [main.o] Error 1. Set Properties > C/C++ Build > [] Generate Makefiles automatically 
# Build directory: ${workspace_loc:/gzapp}/
# And Use Linux GCC Tool Chain!!

all: clean gzapp

gzapp:
	gcc *.c -lm -o ./bin/gzapp -Wall -g -O0 -lz -Wdiscarded-qualifiers

clean:
	@rm -f *.o gzapp
