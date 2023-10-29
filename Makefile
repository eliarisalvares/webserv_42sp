# Variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++98
TARGET = server
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Linking the final program
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Compiling the source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Removing all object files
clean:
	rm -f $(OBJ)

# Removing all object files and the executable
fclean: clean
	rm -f $(TARGET)

# Rebuilding the entire program
re: fclean all

.PHONY: all clean fclean re
