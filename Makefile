tankmonitor: main.cpp
	g++ -std=c++20 main.cpp -o $@ -llgpio

clean:
	rm tankmonitor
