CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = toy-sqlite
SRC = src/main.c src/shell.c
OBJ = $(SRC:.c=.o)
LIBS = -lreadline

all: $(TARGET)

$(TARGET): $(OBJ)
		$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

clean:
		rm -f $(TARGET) $(OBJ)

run: $(TARGET)
		./$(TARGET)

debug: $(TARGET)
		gdb ./$(TARGET)

.PHONY: all clean run debug