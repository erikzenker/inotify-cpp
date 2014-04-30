all:
	clang++ main.cc FileSystemEvent.cc -I . -std=c++11 -lboost_system -lboost_filesystem
