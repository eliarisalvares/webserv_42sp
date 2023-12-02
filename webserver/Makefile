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
NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP -g3
CXXDEBUG = -D DEBUG=true

SRCS =			main.cpp $(WEBSERV) $(SERVER) $(REQUEST) $(LOGGER) $(RESPONSE) \
				$(UTILS)
WEBSERV =		WebServ.cpp
SERVER =		Server.cpp ServerParser.cpp ServerBuilder.cpp ServerGetters.cpp \
				ServerSetters.cpp
REQUEST =		Request.cpp RequestBuilder.cpp RequestParser.cpp
RESPONSE =		Response.cpp ResponseBuilder.cpp StatusMessage.cpp HandleCGI.cpp
LOGGER =		Logger.cpp
UTILS =			openFile.cpp ftstring.cpp http.cpp utils.cpp

INCLUDES =		-I ./ -I src/Logger -I src/Request -I src/Response \
				-I src/Server -I src/Parser -I src/WebServ -I src/Utils

OBJ	=			$(SRCS:%.cpp=$(OBJPATH)/%.o)
DEPS =			$(SRCS:%.cpp=$(OBJPATH)/%.d)

OBJPATH =		temp
VPATH =			src src/Logger src/Request src/Response src/Server src/Parser \
				src/WebServ src/Utils

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

#run the program with args
run:
	./$(NAME) config/testconfig.conf

.PHONY: all clean fclean re
