CXX = g++
CXXFLAGS = -Wall -MMD -std=c++17 -O2
CXXFLAGS_POST = -L/usr/local/lib64 -lpqxx -lssl -lcrypto
MAINS = server.cc test_client.cc database_access_test.cc indigestion.cc
OBJECTS = $(patsubst %.cc,%.o,$(filter-out $(MAINS), $(wildcard *.cc)))

SERVER_EXEC = server
SERVER_OBJS = ${OBJECTS} server.o
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
