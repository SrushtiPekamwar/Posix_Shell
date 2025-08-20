CXX = g++
CXXFLAGS = -g -Wall -std=c++11
TARGET = shell
SRCS = shell.cpp commands_folder/echo.cpp
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile a single .cpp file into a .o file
# $<: is a special variable that represents the first prerequisite (the .cpp file)
# $@: is a special variable that represents the target (the .o file)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)

