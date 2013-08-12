main: main.o parser.o builtin.o model.o eval.o exc.o consts.o types.o
	g++ -o main $^ -pg -lgmp

.cpp.o:
	g++ $< -c -g -pg -DGMP_SUPPORT -Wall

clean:
	rm -f *.o
	rm -f main

db:
	gdb main

cdb:
	cgdb main
