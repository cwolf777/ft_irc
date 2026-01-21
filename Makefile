NAME = server
CXX = c++
INCLUDES = \
	-I./inc \
	-I./inc/server \
	-I./inc/commands \
	-I./inc/client \
	-I./inc/channel \
	-I./inc/protocol

VPATH = ./src ./src/server ./src/commands ./src/client ./src/channel ./src/protocol

SRCS = main.cpp Server.cpp ServerState.cpp ServerUtils.cpp  Client.cpp Channel.cpp IrcMsg.cpp  \
	CommandDispatcher.cpp \
	CapCommand.cpp \
	PassCommand.cpp \
	UserCommand.cpp \
	JoinCommand.cpp \
	NickCommand.cpp \
	PrivMsgCommand.cpp \
	KickCommand.cpp \
	InviteCommand.cpp \
	ModeCommand.cpp \
	NamesCommand.cpp \
	WhoCommand.cpp \
	PingCommand.cpp \
	QuitCommand.cpp \
	TopicCommand.cpp \
	PartCommand.cpp

OBJS_DIR = objects
OBJS = $(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))

CXXFLAGS = -std=c++17 -Wall -Wextra -Werror $(INCLUDES)

all: $(NAME)

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