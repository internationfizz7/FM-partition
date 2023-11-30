TARGET = FM_N26124963
SRC_DIR = ./src
INC_DIR = ./include
# Please place your .cpp and .h file under "src" and "include" directory, respectively.
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))
CC = g++
CFLAG = -Wall -g -O3
${TARGET} : ${OBJ}
	${CC} ${CFLAG} $^ -o $@

# $^ all dependencies
# $< first dependency
# $@ target

$(SRC_DIR)/%.o : $(SRC_DIR)/%.cpp
	${CC} ${CFLAG} -c -I $(INC_DIR) $< -o $@
.PHONY : clean
clean:
	-rm ${OBJ} $(TARGET) 
