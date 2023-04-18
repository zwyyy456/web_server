server:
	g++ src/util.cpp client.cpp -o client && \
	g++ src/util.cpp src/server.cpp src/epoll.cpp src/inet_address.cpp src/socket.cpp src/channel.cpp src/event_loop.cpp src/acceptor.cpp mserv.cpp -o server
clean:
	rm server && rm client