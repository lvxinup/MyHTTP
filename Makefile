bin=HttpdServer
cc=g++

.PHONY:all
all:HttpdServer cgi

HttpdServer:
	g++ -o HttpdServer HttpdServer.cc -std=c++11 -lpthread

cgi:
	g++ -o Cal Cal.cc

.PHONY:clean
clean:
	rm -f HttpdServer Cal output

.PHONY:output
output:
	mkdir output
	cp HttpdServer output
	cp -rf wwwroot output
	cp Cal output/wwwroot
	cp start.sh output
