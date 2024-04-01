CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -MP -MD -std=c++98 -I$(INCLUDES)
FSAN = -fsanitize=address 
SRCDIR = src
OBJDIR = obj
INCLUDES = ./inc
BOT_PATH = ./bot
BOT_NAME = ircbot

SRC = main.cpp Socket.cpp Server.cpp Server_utils.cpp Client.cpp parsing.cpp channels.cpp Channel.cpp Channel_utils.cpp \
		communication.cpp tools.cpp irc_commands.cpp op_commands.cpp auth_commands.cpp other_commands.cpp
OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))
DEP = $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRC))

NAME = ircserv

all: $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(FSAN) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(FSAN) $(CXXFLAGS) -c $< -o $@



$(OBJDIR) :
	mkdir $(OBJDIR)

clean :
	@rm -rf $(OBJDIR)
	@make clean -C $(BOT_PATH)

fclean : clean
	@rm -f $(NAME)
	@rm -rf ../.vscode
	@make fclean -C $(BOT_PATH)

re : fclean all

bonus:
	make -C $(BOT_PATH)

-include $(DEP)

.PHONY : clean
