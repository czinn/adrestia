CXX = g++
CXXFLAGS = -Wall -MMD -std=c++17 -I. -O2
OBJECTS = $(patsubst %.cc,%.o,$(filter-out main.cc, $(wildcard *.cc))) ../server/versioning.o

MAIN_EXEC = main
MAIN_OBJS = ${OBJECTS} main.o
MAIN_DEPS = ${MAIN_OBJS:.o=.d}

TEST_EXEC = test
TEST_OBJS = ${OBJECTS} $(patsubst %.cc, %.o, $(wildcard tests/*.cc))
TEST_DEPS = ${TEST_OBJS:.o=.d}

AI_EXEC = skynet
AI_OBJS = ${OBJECTS} $(patsubst %.cc, %.o, $(wildcard ai/*.cc))
AI_DEPS = ${TEST_OBJS:.o=.d}

CFR_EXEC = train
CFR_OBJS = ${OBJECTS} $(patsubst %.cc, %.o, $(wildcard cfr_train/*.cc))
CFR_DEPS = ${TEST_OBJS:.o=.d}

EXECS = ${MAIN_EXEC} ${TEST_EXEC} ${AI_EXEC} ${CFR_EXEC}

all: ${EXECS}

${MAIN_EXEC}: ${MAIN_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

${TEST_EXEC}: ${TEST_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

${AI_EXEC}: ${AI_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

${CFR_EXEC}: ${CFR_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@

-include ${MAIN_DEPS} ${TEST_DEPS} ${AI_DEPS} ${CFR_DEPS}

clean:
	rm -f {.,tests,ai,cfr_train}/*.{o,d} ${EXECS}
