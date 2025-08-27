CXX        := g++
CXXFLAGS   := -g -Wall -std=gnu++11

USE_EDITLINE ?= 0

READLINE_PREFIX ?= $(shell brew --prefix readline 2>/dev/null)
ifeq ($(READLINE_PREFIX),)
  READLINE_PREFIX := /opt/homebrew/opt/readline
  ifeq ($(shell test -d /usr/local/opt/readline && echo yes),yes)
    READLINE_PREFIX := /usr/local/opt/readline
  endif
endif

LDFLAGS    :=
LDLIBS     :=

ifeq ($(USE_EDITLINE),1)
  LDLIBS += -ledit -lncurses
else
  CXXFLAGS += -I$(READLINE_PREFIX)/include
  LDFLAGS  += -L$(READLINE_PREFIX)/lib
  LDLIBS   += -lreadline -lncurses
endif

TARGET := shell
SRCS   := shell.cpp \
          commands_folder/echo.cpp \
          commands_folder/cd.cpp \
          commands_folder/ls.cpp \
          commands_folder/skipDelimiters.cpp \
          commands_folder/home.cpp \
          commands_folder/initialPrompt.cpp \
          commands_folder/externalCommand.cpp \
          commands_folder/userInput.cpp \
          commands_folder/history.cpp
OBJS   := $(patsubst %.cpp,%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)