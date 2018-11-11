CXX = g++
CXXFLAGS = -Wall -MMD -O2 -std=c++17
OBJECTS = $(patsubst %.cc,%.o,$(filter-out main.cc, $(wildcard *.cc)))

MAIN_EXEC = main
MAIN_OBJS = ${OBJECTS} main.o
MAIN_DEPS = ${MAIN_OBJS:.o=.d}

TEST_EXEC = test
TEST_OBJS = ${OBJECTS} $(patsubst %.cc, %.o, $(wildcard tests/*.cc))
TEST_DEPS = ${TEST_OBJS:.o=.d}

CLI_EXEC = spells_cli
CLI_OBJS = ${OBJECTS} $(patsubst %.cc, %.o, $(wildcard cli/*.cc))
CLI_DEPS = ${CLI_OBJS:.o=.d}

AI_EXEC = spells_ai
AI_OBJS = ${OBJECTS} $(patsubst %.cc, %.o, $(wildcard ai/*.cc))
AI_DEPS = ${AI_OBJS:.o=.d}

EXECS = ${MAIN_EXEC} ${TEST_EXEC} ${CLI_EXEC} ${AI_EXEC}

all: ${EXECS}

${MAIN_EXEC}: ${MAIN_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

${TEST_EXEC}: ${TEST_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

${CLI_EXEC}: ${CLI_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

${AI_EXEC}: ${AI_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

-include ${MAIN_DEPS} ${TEST_DEPS} ${CLI_DEPS} ${AI_DEPS}

clean:
	rm -f {.,tests,cli,ai}/*.{o,d} ${EXECS}
