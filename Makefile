CFLAGS = -Wall -Wextra
LIBS = -lfl
BUILD = build
SRC = src
OBJECTS = $(BUILD)/main.o \
	$(BUILD)/lexer.o \
	$(BUILD)/parser.o \
	$(BUILD)/ast.o

.PHONY : clean
.DEFAULT : all

all : $(BUILD) $(BUILD)/mysh

$(BUILD) :
	mkdir $@

$(BUILD)/mysh : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

$(BUILD)/lexer.c : $(SRC)/lexer.l
	flex -o $(BUILD)/lexer.c $<

$(BUILD)/parser.c : $(SRC)/parser.y
	bison --defines=$(BUILD)/parser.h -o $(BUILD)/parser.c $<

$(BUILD)/parser.h : $(SRC)/parser.y
	bison --defines=$(BUILD)/parser.h -o $(BUILD)/parser.c $<

$(BUILD)/lexer.o : $(BUILD)/lexer.c $(BUILD)/parser.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/parser.o : $(BUILD)/parser.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o : $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/main.o : $(BUILD)/parser.h

clean :
	rm -r $(BUILD)
