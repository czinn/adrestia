all: server_abomination client_abomination

server_abomination: server_abomination.cc
	g++ -std=c++17 -o server_abomination server_abomination.cc -lpqxx -lssl -lcrypto

client_abomination: client_abomination.cc
	g++ -std=c++17 -o client_abomination client_abomination.cc -lpqxx -lssl -lcrypto

clean:
	rm -f server_abomination client_abomination
