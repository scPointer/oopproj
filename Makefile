main: main.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp
	g++ -O2 -o main -std=c++14 main.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp

win_main: main.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp
	g++ -O2 -o main.exe -std=c++14 main.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp

.PHONY: clean
clean:
	rm main

.PHONY: win_clean
win_clean:
	del main.exe