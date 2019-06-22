main: main1 main2 main3

win_main: win_main1 win_main2 win_main3

main1: main1.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp
	g++ -O2 -o main1 -std=c++14 main1.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp

win_main1: main1.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp
	g++ -O2 -o main1.exe -std=c++14 main1.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp

main2: main1.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp
	g++ -O2 -o main2 -std=c++14 main1.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp

win_main2: main1.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp
	g++ -O2 -o main2.exe -std=c++14 main1.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp

main3: main3.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp
	g++ -O2 -o main3 -std=c++14 main3.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp

win_main3: main3.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp
	g++ -O2 -o main3.exe -std=c++14 main3.cpp Node.cpp build_tree.cpp evaluation.cpp CG_debug.cpp

.PHONY: clean
clean:
	rm main1 main2 main3

.PHONY: win_clean
win_clean:
	del main1.exe main2.exe main3.exe