CXX = g++
CXXFLAGS = -Wall -pthread -std=c++11 -Isrc -Isrc/ui
SRC = $(wildcard src/*.cpp src/game/*.cpp src/threads/*.cpp src/ui/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = breakout

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lncurses

# Reglas generales para compilar .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
