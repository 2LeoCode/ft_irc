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
				src/utils.cpp\
				src/irc/Server/Execs/cap.cpp\
				src/irc/Server/Execs/die.cpp\
				src/irc/Server/Execs/join.cpp\
				src/irc/Server/Execs/kick.cpp\
				src/irc/Server/Execs/kill.cpp\
				src/irc/Server/Execs/mode.cpp\
				src/irc/Server/Execs/motd.cpp\
				src/irc/Server/Execs/names.cpp\
				src/irc/Server/Execs/nick.cpp\
				src/irc/Server/Execs/notice.cpp\
				src/irc/Server/Execs/oper.cpp\
				src/irc/Server/Execs/part.cpp\
				src/irc/Server/Execs/pass.cpp\
				src/irc/Server/Execs/ping.cpp\
				src/irc/Server/Execs/pong.cpp\
				src/irc/Server/Execs/privmsg.cpp\
				src/irc/Server/Execs/quit.cpp\
				src/irc/Server/Execs/user.cpp

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
