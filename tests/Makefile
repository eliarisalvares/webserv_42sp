#colors
GREEN			= \033[0;32m
YELLOW			= \033[0;38;5;184m
CYAN			= \033[0;36m
GRAY			= \033[0;90m
RESET			= \033[0m

################################################################################
##                                 COMPILATION                                ##
################################################################################
# Variables
NAME = tests
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP -g3
CXXDEBUG = -D DEBUG=true
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all \
--trace-children=yes --track-origins=yes --track-fds=yes

SRCS =			xtod_test.cpp Logger.cpp utils.cpp

INCLUDES =		-I ./ -I ../webserver/src/Logger -I ../webserver/src/Utils

OBJ	=			$(SRCS:%.cpp=$(OBJPATH)/%.o)
DEPS =			$(SRCS:%.cpp=$(OBJPATH)/%.d)

OBJPATH =		temp
VPATH =			../webserver/src/Logger ../webserver/src/Utils

################################################################################
##                                    RULES                                   ##
################################################################################

# Default target
all: $(NAME)

# Folder for temporary objects
$(OBJPATH)/%.o:	%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)
	@printf "$(GRAY)[ INFO ] Generating $(CYAN)$(NAME)$(GRAY) objects... $(YELLOW)%-33.33s\n$(RESET)" $@

# Folder for temporary objects
$(OBJPATH):
	@printf "$(GRAY)[ INFO ] Compiling objects with $(RESET)$(CXX) $(CXXFLAGS) \n" $@
	@mkdir -p $(OBJPATH)

# Linking the final program
$(NAME): $(OBJPATH) $(OBJ)
	@$(CXX) $(OBJ) -o $(NAME)
	@echo "$(CYAN)$(NAME)$(GRAY): $(GREEN)Done$(GRAY)!$(RESET)"

# Compiling the source files
# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# Removing all object files
clean:
	@rm -f $(OBJ)
	@echo "$(GRAY)[ INFO ] Cleaning objects$(RESET)"

# Removing all object files and the executable
fclean: clean
	@rm -f $(NAME)
	@rm -rf $(OBJPATH)
	@echo "$(GRAY)[ INFO ] Cleaning executable$(RESET)"

# Rebuilding the entire program
re: fclean all

# Checking headers for each file
-include $(DEPS)

val:
	$(VALGRIND) ./$(NAME)

.PHONY: all clean fclean re
