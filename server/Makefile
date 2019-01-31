CXX = g++
CXXFLAGS = -Wall -MMD -std=c++17 -O2
CXXFLAGS_POST = -L/usr/local/lib -lpqxx -lssl -lcrypto
MAINS = ./server_main.cc ./test_client.cc
OBJECTS = $(patsubst %.cc,%.o,$(filter-out $(MAINS), $(shell find . ../cpp/game_rules.cc ../cpp/book.cc ../cpp/duration.cc ../cpp/effect.cc ../cpp/effect_instance.cc ../cpp/game_state.cc ../cpp/game_view.cc ../cpp/player.cc ../cpp/random_strategy.cc ../cpp/selector.cc ../cpp/spell.cc ../cpp/sticky.cc ../cpp/sticky_instance.cc ../cpp/sticky_invoker.cc -name "*.cc")))

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
	rm -f handlers/*.o handlers/*.d pushers/*.o pushers/*.d *.o *.d ${EXECS}
