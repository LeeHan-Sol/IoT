CC = gcc
CFLAGS = -W -Wall -g -D_REENTRANT
TARGET = IoT
#OBJECTS = Error.o DHT11.o TextLCD.o LED.o Button.o IoT.o Thread.o
OBJECTS = Error.o Node.o List.o Thread.o IoT.o 
#LDFLAGS = -lwiringPi -lwiringPiDev -lpthread 
LDFLAGS = -lpthread 

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	#rm Error.o DHT11.o TextLCD.o LED.o Button.o IoT.o Thread.o IoT
	rm Error.o IoT.o Thread.o Node.o List.o IoT
