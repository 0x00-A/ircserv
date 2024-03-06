CC = c++
CFLAGS = -g -Wall -Wextra -Werror -MP -MD -std=c++98
FSAN = -fsanitize=address
SRCDIR = .
OBJDIR = obj

SRC = main.cpp TCPSocket.cpp error.cpp
OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))
DEP = $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRC))

NAME = bin

all: $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(FSAN) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(FSAN) $(CFLAGS) -c $< -o $@

$(OBJDIR) :
	mkdir $(OBJDIR)

clean :
	@rm -rf $(OBJ) $(DEP)

fclean : clean
	@rm -f $(NAME)

re : fclean all

-include $(DEP)

.PHONY : clean
