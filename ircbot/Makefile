CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -MP -MD -std=c++98 -I$(INCLUDES)
FSAN = -fsanitize=address 
SRCDIR = .
OBJDIR = obj
INCLUDES = ./inc

SRC = main.cpp

OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))
DEP = $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRC))

NAME = ircbot

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
