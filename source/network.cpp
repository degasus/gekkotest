#include <network.h>

#include "gekkotest.h"


static int client_socket;
static int server_socket;


void print(const char* str, u32 len) {
	net_send(client_socket, str, len, 0);
}

void network_init() {
	struct sockaddr_in my_name;
	
	my_name.sin_family = AF_INET;
	my_name.sin_port = htons(SERVER_PORT);
	my_name.sin_addr.s_addr = htonl(INADDR_ANY);
	
	net_init();
	
	server_socket = net_socket(AF_INET, SOCK_STREAM, 0);
	net_setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, NULL, sizeof(int));
	while(net_bind(server_socket, (struct sockaddr*)&my_name, sizeof(my_name)) < 0);
	net_listen(server_socket, 0);
	
	struct sockaddr_in client_info;
	socklen_t ssize = sizeof(client_info);
	client_socket = net_accept(server_socket, (struct sockaddr*)&client_info, &ssize);
}

void network_shutdown() {
	net_close(client_socket);
	net_close(server_socket);
}
