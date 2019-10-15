#  2018201005 Vatsal Soni
CC = g++ 
CFLAGS = -Wall
DEPS = trackerconfig.h
OBJ = tracker.o
%.o: %.cpp $(DEPS)
		$(CC) $(CFLAGS) -c -o $@ $< -lpthread

tracker: $(OBJ)
		$(CC) $(CFLAGS) -o $@ $^ -lpthread

clean:
	rm -rf *.o tracker
