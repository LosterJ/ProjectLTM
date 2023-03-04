all:server client

server: server.c login_regist.h load_quest.h
	gcc -o server server.c
client: client.c
	gcc -o client client.c
clean: 
	rm -rf server client