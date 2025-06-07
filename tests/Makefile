CXX = g++
CXXFLAGS = -std=c++17 -I../BZ_CARD -I./mocks -pthread -include mocks/ArduinoCompat.h
LDFLAGS = -lgtest -lgtest_main -pthread

SRC = ../BZ_CARD/StorageManager.cpp test_storagemanager.cpp


test_storage: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -L/usr/lib/x86_64-linux-gnu $(LDFLAGS) -o $@

clean:
	rm -f test_storage *.o
