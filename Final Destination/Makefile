#---------Group Information----------
# Group Number: 15

# Yash Mundada: 2021A7PS0001P
# Aaryan Garg: 2021A7PS2222P
# Dev Kulkarni: 2021A7PS2430P
# Radhey Kanade: 2021A7PS2534P
# Shardul Shingare: 2021A7PS2539P
# Shantanu Ambekar: 2021A7PS2540P
#------------------------------------
CC = gcc

# Define the source files
SRCS = driver.c lexer.c symbol_table.c parser.c parsetree.c stack.c

# Define the target executable
TARGET = stage1exe

# Rule to build the target executable
$(TARGET): $(SRCS)
	@$(CC) -o $(TARGET) $(SRCS)

# Default rule to run the target executable
build: $(TARGET)
