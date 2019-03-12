CXX = g++
CXXFLAGS = -Wall -g -MMD -std=c++17 -O2 -I../cpp/fdeep-include -I../cpp/fplus-include -I../cpp 
CXXFLAGS_POST = -L/usr/local/lib -L/usr/local/lib64 -lpqxx -lssl -lcrypto -lpthread
MAINS = ./server_main.cc ./test_client.cc ./autofill.cc ../cpp/main.cc ../cpp/ai/main.cc ../cpp/cfr_train/main.cc ../cpp/tests/test.cc ../cpp/tests/game_rules_test.cc ../cpp/tests/game_state_test.cc
OBJECTS = $(patsubst %.cc,%.o,$(filter-out $(MAINS), $(shell find . ../cpp -name "*.cc")))

SERVER_EXEC = server_main
SERVER_OBJS = ${OBJECTS} server_main.o
SERVER_DEPS = ${SERVER_OBJS:.o=.d}

CLIENT_EXEC = test_client
CLIENT_OBJS = ${OBJECTS} test_client.o
CLIENT_DEPS = ${CLIENT_OBJS:.o=.d}

AUTOFILL_EXEC = autofill
AUTOFILL_OBJS = ${OBJECTS} autofill.o
AUTOFILL_DEPS = ${AUTOFILL_OBJS:.o=.d}

EXECS = ${SERVER_EXEC} ${CLIENT_EXEC} ${AUTOFILL_EXEC}

all: ${EXECS}

${SERVER_EXEC}: ${SERVER_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@ ${CXXFLAGS_POST}

${CLIENT_EXEC}: ${CLIENT_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@ ${CXXFLAGS_POST}

${AUTOFILL_EXEC}: ${AUTOFILL_OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@ ${CXXFLAGS_POST}

-include ${MAIN_DEPS} ${AUTOFILL_DEPS}

clean:
	rm -f ../cpp/*.o handlers/*.o handlers/*.d pushers/*.o pushers/*.d *.o *.d ${EXECS}
