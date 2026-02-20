NAME = ircserv
CXX = c++
INCLUDES = \
	-I./inc \
	-I./inc/server \
	-I./inc/commands \
	-I./inc/client \
	-I./inc/channel \
	-I./inc/protocol

VPATH = ./src ./src/server ./src/commands ./src/client ./src/channel ./src/protocol

SRCS = main.cpp Server.cpp ServerIO.cpp ServerLoop.cpp ServerState.cpp  Client.cpp Channel.cpp IrcMsg.cpp  \
	CommandDispatcher.cpp \
	handleCap.cpp \
	handlePass.cpp \
	handleUser.cpp \
	handleJoin.cpp \
	handleNick.cpp \
	handlePrivMsg.cpp \
	handleKick.cpp \
	handleInvite.cpp \
	handleMode.cpp \
	handleNames.cpp \
	handleWho.cpp \
	handlePing.cpp \
	handleQuit.cpp \
	handleTopic.cpp \
	handlePart.cpp \
	handleNotice.cpp

OBJS_DIR = objects
OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))

CXXFLAGS = -std=c++17 -Wall -Wextra -Werror $(INCLUDES)

all: $(NAME)


debug: CXXFLAGS += -g3 -O0 -fno-omit-frame-pointer -fno-inline -fsanitize=address
debug: re

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: %.cpp | $(OBJS_DIR)
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJS_DIR):
	@echo "Creating $(OBJS_DIR) directory"
	@mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all