INCLUDES = -I../../inc/scanner
SOURCES = $(wildcard *.cpp)
OBJS = $(SOURCES:%.cpp=../../build/scanner/%.o)
OBJ_NAME = ../../scanner.exe

../../build/scanner/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(OBJ_NAME)

clean:
	rm -f ../../build/scanner/*
	rm -f $(OBJ_NAME)

