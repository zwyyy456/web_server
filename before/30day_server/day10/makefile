server:
	g++ src/util.cpp src/buffer.cpp src/socket.cpp src/inet_address.cpp client.cpp -o client && \
	g++ src/util.cpp src/server.cpp src/epoll.cpp src/inet_address.cpp \
	src/socket.cpp src/channel.cpp src/event_loop.cpp src/acceptor.cpp \
	src/connection.cpp src/buffer.cpp src/thread_pool.cpp mserv.cpp -pthread -o server
clean:
	rm server && rm client