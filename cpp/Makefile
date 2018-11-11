CXX = g++
CXXFLAGS = -Wall -MMD -O2 -std=c++17
OBJECTS = $(patsubst %.cc,%.o,$(filter-out main.cc, $(wildcard *.cc)))

MAIN_EXEC = main
MAIN_OBJS = ${OBJECTS} main.o
MAIN_DEPS = ${MAIN_OBJS:.o=.d}

TEST_EXEC = test
TEST_OBJS = ${OBJECTS} $(patsubst %.cc, %.o, $(wildcard tests/*.cc))
TEST_DEPS = ${TEST_OBJS:.o=.d}

EXECS = ${MAIN_EXEC} ${TEST_EXEC}

all: ${EXECS}

${MAIN_EXEC}: ${MAIN_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

${TEST_EXEC}: ${TEST_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

-include ${MAIN_DEPS} ${TEST_DEPS} ${CLI_DEPS} ${AI_DEPS}

clean:
	rm -f {.,tests}/*.{o,d} ${EXECS}
