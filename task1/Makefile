MAIN=op
MAIN2=op2

FUN=sqr sqrt
OBJ=$(FUN:=.o)
CMP=$(FUN:=.c)

S_LIB=lib$(MAIN).a
D_LIB=lib$(MAIN)dyn.so

FLAGS=-lm -ldl



op2: op2.o dlib
	gcc -o op2 op2.o $(FLAGS)

opd: $(MAIN).o dlib
	gcc -o $(MAIN)d $(MAIN).o -L. -l$(MAIN)dyn $(FLAGS)

ops: $(MAIN).o slib
	gcc -o $(MAIN)s $(MAIN).o -L. -l$(MAIN) $(FLAGS)

dlib: $(OBJ)
	gcc -shared -o $(D_LIB) $(OBJ) $(FLAGS)

slib: $(OBJ)
	ar rc $(S_LIB) $^
	ranlib $(S_LIB)

obj: $(MAIN).c $(MAIN2).c $(CMP)
	gcc -c $^

clean:
	rm -rf $(MAIN) $(MAIN)d $(MAIN)s $(MAIN).o $(MAIN2) $(MAIN2).o $(OBJ) lib$(MAIN).a lib$(MAIN)dyn.so
