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
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all \
--trace-children=yes --track-origins=yes --track-fds=yes

SRCS =			main.cpp $(WEBSERV) $(SERVER) $(REQUEST) $(LOGGER) $(RESPONSE) \
				$(UTILS)
WEBSERV =		WebServ.cpp
SERVER =		Server.cpp ServerParser.cpp server_builder.cpp server_getters.cpp \
				server_setters.cpp location_builder.cpp server_exceptions.cpp
REQUEST =		Request.cpp RequestBuilder.cpp RequestParser.cpp \
				request_parser_getters.cpp parse_control_data.cpp \
				parse_headers.cpp parse_body.cpp parse_utils.cpp
RESPONSE =		Response.cpp ResponseBuilder.cpp StatusMessage.cpp HandleCGI.cpp \
				HandleGetRequest.cpp HandlePostRequest.cpp HandleDeleteRequest.cpp \
				HandleRedirect.cpp ResponseUtils.cpp GetUtils.cpp
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

#needed to run the project if python not installed
update-env:
	sudo apt-get update
	sudo apt-get python3

# Folder for temporary objects
$(OBJPATH)/%.o:	%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)
	@printf "$(GRAY)[ INFO ] Generating $(CYAN)$(NAME)$(GRAY) objects... $(YELLOW)%-33.33s\n$(RESET)" $@

# Folder for temporary objects
$(OBJPATH):
	@printf "$(GRAY)[ INFO ] Compiling objects with $(RESET)$(CXX) $(CXXFLAGS) \n" $@
	@mkdir -p $(OBJPATH)
	@mkdir -p content/upload

# Linking the final program
$(NAME): $(OBJPATH) $(OBJ)
	@$(CXX) $(OBJ) -o $(NAME)
	@echo "$(CYAN)$(NAME)$(GRAY): $(GREEN)Done$(GRAY)!$(RESET)"

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

val:
	$(VALGRIND) ./$(NAME) config/testconfig.conf

.PHONY: all clean fclean re
