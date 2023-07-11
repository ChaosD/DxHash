DIR=..

SRC_DIR=${DIR}/src
TEST_DIR=${DIR}/test
INCLUDE_DIR=${DIR}/src
OBJ_DIR=${DIR}/obj

INCLUDE = \
-I ${INCLUDE_DIR} 

EXTENSION=cpp
OBJS= $(patsubst $(SRC_DIR)/%.$(EXTENSION), $(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.$(EXTENSION))) \
	$(patsubst $(TEST_DIR)/%.$(EXTENSION), $(OBJ_DIR)/%.o,$(wildcard $(TEST_DIR)/*.$(EXTENSION)))


CXX = g++
CFLAGS = -c -Wall --std=c++11
LFLAGS = -o test -Wall --std=c++11

.PHONY: all clean rebuild

all: ${OBJS}
	${CXX} ${LFLAGS} ${OBJS}

${OBJ_DIR}/%.o:${SRC_DIR}/%.${EXTENSION}
	${CXX} $< -o $@ ${CFLAGS} ${INCLUDE}

${OBJ_DIR}/%.o:${TEST_DIR}/%.${EXTENSION}
	${CXX} $< -o $@ ${CFLAGS} ${INCLUDE}

rebuild: clean all

clean:
	rm -rf ${OBJS}