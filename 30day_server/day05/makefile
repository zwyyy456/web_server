server:
	g++ util.cpp client.cpp -o client && \
	g++ util.cpp server.cpp epoll.cpp inet_address.cpp socket.cpp channel.cpp -o server
clean:
	rm server && rm client