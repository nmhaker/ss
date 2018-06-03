IDIR=include
OBJDIR=obj
SRCDIR=src
CC=g++
CFLAGS=-Wall -I$(IDIR)
PROGRAM=prog

SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJ=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))

$(PROGRAM) : $(OBJ)
	$(CC) -g -o $@ $^ -std=c++11
	chmod +x prog
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -g -o $@ -c $< $(CFLAGS) -std=c++11
run:
	./prog
clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(PROGRAM)

.PHONY: clean
