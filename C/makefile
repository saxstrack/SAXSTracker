CC  = g++
INC = -I/opt/local/include/ 
INC2 = -L/opt/local/lib -lopencv_core.2.4.0 -lopencv_highgui.2.4.0 -lopencv_video.2.4.0 -lopencv_imgproc -lopencv_ml

all: trackitc

trackit.o: trackit.cc
	g++ $(INC) -c -o $@ $<

trackit: trackit.o
	g++ $(INC2) -o $@ $^

trackitc.o: trackitc.c
	g++ $(INC) -c -o $@ $<

trackitc: trackitc.o
	g++ $(INC2) -o $@ $^

      
.PHONY: clean

clean:
	rm -f trackit trackit.o
