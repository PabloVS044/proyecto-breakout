
CXX = g++
CXXFLAGS = -Wall -pthread -std=c++11

SRC = $(wildcard src/game/*.cpp src/threads/*.cpp src/ui/*.cpp)
OBJ = $(SRC:.cpp=.o)

TARGET = breakout

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)
