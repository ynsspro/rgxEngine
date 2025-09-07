CC = gcc
CFLAGS = -Wall -Wextra  -std=c2x -pedantic # -Werror  you can add this flag to turn warning to errors
TARGET = bin/rgxEngine
SRC = rgxEngine.c rgx.c

all: $(TARGET)
	./$(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
clean:
	rm -f $(TARGET)
run:all
	./$(TARGET)
