CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -MP -MD -std=c++98 -I$(INCLUDES)
FSAN = -fsanitize=address 
SRCDIR = src
OBJDIR = obj
INCLUDES = ./inc

SRC = main.cpp Socket.cpp Server.cpp Client.cpp command.cpp parser.cpp channels.cpp Channel.cpp \
		communication.cpp
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

fclean : clean
	@rm -f $(NAME)
	@rm -rf ../.vscode

re : fclean all

-include $(DEP)

.PHONY : clean
