CXX = clang++
FLAGS = -fsycl
SRC = main.cc
BIN_NAME = $(basename $(SRC))

$(BIN_NAME): $(SRC)
	$(CXX) $(FLAGS) -o ./bin/$@ $<