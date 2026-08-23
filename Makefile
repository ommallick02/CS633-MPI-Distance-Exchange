# ============================================================================
# Makefile for CS633 MPI Distance Exchange Project
# ============================================================================

# Compiler and flags
CC = mpicc
CFLAGS = -O2 -Wall -Wextra -lm
TARGET = execute
SRCS = src/src.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)
	@echo "Build successful: $(TARGET)"

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJS) *.o
	@echo "Clean complete"

# Rebuild
rebuild: clean all

# Phony targets
.PHONY: all clean rebuild

# Help
help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Compile the MPI program (default)"
	@echo "  clean     - Remove compiled files"
	@echo "  rebuild   - Clean and build"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Example:"
	@echo "  make                           # Build the project"
	@echo "  mpirun -np 8 ./execute ..."    # Run with 8 processes"

.PHONY: help