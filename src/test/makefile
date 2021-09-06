INCLUDES = -I../../inc/scanner -I../../inc/test
SOURCES = $(wildcard *.cpp)
OBJS = $(SOURCES:%.cpp=../../build/test/%.o)
OBJ_NAME = ../../test_scanner.exe

../../build/test/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(OBJ_NAME)

clean:
	rm -f ../../build/test/*
	rm -f $(OBJ_NAME)


