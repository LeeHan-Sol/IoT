CC = gcc
CFLAGS = -W -Wall -g -D_REENTRANT
TARGET = IoT
OBJECTS = DHT11.o TextLCD.o LED.o Button.o IoT.o
LDFLAGS = -lwiringPi -lwiringPiDev -lpthread 

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	rm DHT11.o TextLCD.o LED.o Button.o IoT.o IoT
