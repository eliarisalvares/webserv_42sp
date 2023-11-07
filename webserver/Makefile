################################################################################
##                                 COMPILATION                                ##
################################################################################
# Variables
NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++98 -MMD -MP
CXXDEBUG = -D DEBUG=true

SRCS =			main.cpp $(SERVER) $(REQUEST) $(LOGGER) $(RESPONSE) openFile.cpp
LOGGER =		Logger.cpp
REQUEST =		Request.cpp RequestBuilder.cpp
RESPONSE =		Response.cpp
SERVER =		Server.cpp ParserConfig.cpp

INCLUDES =		-I ./ -I src/Logger -I src/Request -I src/Response -I src/Server -I src/Parser

OBJ	=			$(SRCS:%.cpp=$(OBJPATH)/%.o)
DEPS =			$(SRCS:%.cpp=$(OBJPATH)/%.d)

OBJPATH =		temp
VPATH =			src src/Logger src/Request src/Response src/Server src/Parser

################################################################################
##                                    RULES                                   ##
################################################################################

# Default target
all: $(NAME)

# Folder for temporary objects
$(OBJPATH)/%.o:	%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

# Folder for temporary objects
$(OBJPATH):
		@mkdir -p $(OBJPATH)

# Linking the final program
$(NAME): $(OBJPATH) $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

# Compiling the source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Removing all object files
clean:
	rm -f $(OBJ)

# Removing all object files and the executable
fclean: clean
	rm -f $(NAME)
	rm -rf $(OBJPATH)

# Rebuilding the entire program
re: fclean all

# Checking headers for each file
-include $(DEPS)

.PHONY: all clean fclean re
