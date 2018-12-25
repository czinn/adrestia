CXX = g++
CXXFLAGS = -Wall -MMD -std=c++17 -O2
CXXFLAGS_POST = -L/usr/local/lib64 -lpqxx -lssl -lcrypto
MAINS = ./server_main.cc ./test_client.cc
OBJECTS = $(patsubst %.cc,%.o,$(filter-out $(MAINS), $(shell find . -name "*.cc")))

SERVER_EXEC = server_main
SERVER_OBJS = ${OBJECTS} server_main.o
SERVER_DEPS = ${SERVER_OBJS:.o=.d}

CLIENT_EXEC = test_client
CLIENT_OBJS = ${OBJECTS} test_client.o
CLIENT_DEPS = ${CLIENT_OBJS:.o=.d}

EXECS = ${SERVER_EXEC} ${CLIENT_EXEC}

all: ${EXECS}

${SERVER_EXEC}: ${SERVER_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@ ${CXXFLAGS_POST}

${CLIENT_EXEC}: ${CLIENT_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@ ${CXXFLAGS_POST}

-include ${MAIN_DEPS} ${CLIENT_DEPS}

clean:
	rm -f *.o *.d ${EXECS}
	rm -f handlers/*.o handlers/*.d
