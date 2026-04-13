SRC=main.cpp report.cpp mqtt.cpp platform.cpp sensor.cpp pump.cpp
CMD=g++ -gdwarf-4 -std=c++20 $(SRC)

tankmonitor: $(SRC)
	$(CMD) -o $@ -llgpio

tankmonitor_x86: $(SRC)
	$(CMD) -o tankmonitor

clean:
	rm tankmonitor
