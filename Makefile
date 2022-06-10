SHELL		=	zsh

NAME		=	ft_irc
DEBUG		=	false

PORT		=	6667
PASSWORD	=	123

CXX			=	clang++
CXXFLAGS	=	-Wall -Werror -Wextra -MMD -std=c++98 -g#-g3 -fsanitize=address
LDFLAGS		=	
RM			=	rm -f

SRC			=	src/irc/Channel/Channel.cpp\
				src/irc/Client/Client.cpp\
				src/irc/Server/Server.cpp\
				src/main.cpp\
				src/utils.cpp

OBJ			=	$(SRC:.cpp=.o)
DEP			=	$(OBJ:.o=.d)

ifeq ($(DEBUG),true)
	CXXFLAGS += -DDEBUG
	LDFLAGS = -lncurses
endif

all: $(NAME)

run: all
	./ft_irc $(PORT) $(PASSWORD)

-include $(DEP)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) $(OBJ) $(DEP)

fclean: clean
	$(RM) $(NAME)

re: fclean all
