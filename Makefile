SOURCES = shell.cpp 
FLAGS = -c -O2 -I. 
TARGET = MyShell
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

main.o : shell.h
%.o : %.c
$(OBJECTS): Makefile 

.cpp.o:
	$(CXX) $(FLAGS) $< -o $@
 
$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS)


.PHONY: clean
clean:
	rm -f *.o ${TARGET}
