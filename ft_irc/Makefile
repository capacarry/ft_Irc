NAME = ircserv

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm -rf

SRC = main.cpp srcs/Server.cpp srcs/Client.cpp srcs/Utils.cpp srcs/CommandHandler.cpp srcs/Channel.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) -o $(NAME) $(OBJ)
	@echo "\033[32m[success]\033[0m $(NAME) compiled!"

%.o: %.cpp
	@$(CC) $(FLAGS) -c $< -o $@
	@echo "\033[36m[compiled]\033[0m $<"

clean:
	@$(RM) $(OBJ)
	@echo "\033[33m[clean]\033[0m object files removed!"

fclean: clean
	@$(RM) $(NAME)
	@echo "\033[31m[fclean]\033[0m $(NAME) removed!"

re: fclean all