server: server.o
	echo "Making server"
	gcc -o server server.o

server.o: server.c
	echo "Making server.o"
	gcc -c server.c

clean:
	echo "Cleaning up"
	rm -f *.o server