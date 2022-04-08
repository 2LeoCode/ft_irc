SHELL		=	zsh


NAME		=	ft_irc

CXX			=	clang++
CXXFLAGS	=	-Wall -Werror -Wextra -MMD -std=c++98 #-g3 -fsanitize=address
RM			=	rm -f

SRC			=	src/main.cpp\
				test/trie_test_1.cpp\
				test/trie_test_2.cpp
OBJ			=	$(SRC:.cpp=.o)
DEP			=	$(OBJ:.o=.d)

all: $(NAME)

-include $(DEP)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) $(OBJ) $(DEP)

fclean: clean
	$(RM) $(NAME)

re: fclean all
