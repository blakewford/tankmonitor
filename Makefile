SRC=main.cpp mqtt.cpp
CMD=g++ -std=c++20 $(SRC)

tankmonitor: $(SRC)
	$(CMD) -o $@ -llgpio

tankmonitor_x86: $(SRC)
	$(CMD) -o tankmonitor

clean:
	rm tankmonitor
