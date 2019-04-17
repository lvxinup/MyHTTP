bin=HttpdServer
cc=g++

.PHONY:all
all:HttpdServer cgi 

HttpdServer:
	g++ -o HttpdServer HttpdServer.cc -std=c++11 -lpthread

.PHONY:cgi
cgi:
	g++ -o Cal Cal.cc

.PHONY:three_game
three_game:
	g++ -o three_game three_game.c

.PHONY:mine_clearance
mine_clearance:
	g++ -o mine_clearance mine_clearance.c

.PHONY:clean
clean:
	rm -rf HttpdServer Cal output

.PHONY:output
output:
	mkdir output
	cp HttpdServer output
	cp -rf wwwroot output
	cp Cal output/wwwroot
	cp start.sh output
