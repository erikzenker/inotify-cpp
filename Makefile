all:
	clang++ main.cc -I . -std=c++11 -lboost_system -lboost_filesystem
