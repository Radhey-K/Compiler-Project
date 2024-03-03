CC = gcc

# Define the source files
SRCS = driver.c lexer.c symbol_table.c

# Define the target executable
TARGET = lexer

# Rule to build the target executable
$(TARGET): $(SRCS)
	@$(CC) -o $(TARGET) $(SRCS)

# Default rule to run the target executable
run: $(TARGET)
	@./$(TARGET)
	@make clean

# Clean rule
clean:
	@rm -f $(TARGET)
