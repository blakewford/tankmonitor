CMD=g++ -std=c++20 $<

tankmonitor: main.cpp
	$(CMD) -o $@ -llgpio

tankmonitor_x86: main.cpp
	$(CMD) -o tankmonitor

clean:
	rm tankmonitor
