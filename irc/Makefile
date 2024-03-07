CC = c++
CFLAGS = -g -Wall -Wextra -Werror -MP -MD -std=c++98 -I$(INC)
FSAN = -fsanitize=address 
SRCDIR = src
OBJDIR = obj
INC = ./inc

SRC = main.cpp Socket.cpp Server.cpp Client.cpp command.cpp parser.cpp channels.cpp Channel.cpp \
		communication.cpp
OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))
DEP = $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRC))

NAME = ircserv

all: $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(FSAN) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(FSAN) $(CFLAGS) -c $< -o $@

$(OBJDIR) :
	mkdir $(OBJDIR)

clean :
	@rm -rf $(OBJDIR)

fclean : clean
	@rm -f $(NAME)
	@rm -rf ../.vscode

re : fclean all

-include $(DEP)

.PHONY : clean
