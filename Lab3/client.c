#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char const *argv[]) 
{ 
	struct sockaddr_in serverAddress;
	int socket_fd, connection_fd;
	
	// Validating User Parameters
	if(argc != 3) {
		printf("[USAGE]: <executable code> <Proxy_Server IP Address> <Proxy_Server Port number>\n");
		return 0;
	}
	
	const char *proxy_addr = argv[1];
	int PROXY_PORT_NO = atoi(argv[2]);
	
	// Creating the socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0) {
		printf("[ERROR]: Unable to create socket\n");
		exit(EXIT_FAILURE); 
	}
	else {
		printf("[SUCCESS]: Socket created\n");
	}
	
	
	// Configuring socket parameters for the Client-Proxy Connection
	memset(&serverAddress, '\0', sizeof serverAddress);
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(proxy_addr);
	serverAddress.sin_port = htons(PROXY_PORT_NO);
	
	
	// Setting up the connection with DNS Proxy
	connection_fd = connect(socket_fd, (struct sockaddr *)&serverAddress, sizeof serverAddress);
	if(connection_fd < 0) {
		printf("[ERROR]: Failed to connect to the proxy server\n");
		exit(EXIT_FAILURE); 
	}
	else {
		printf("[SUCCESS]: Connected to the proxy server\n");
	}
	
	
	printf("[USAGE]:\nMessage Type - 1: Request for IP address corresponding to the Domain Name\nMessage Type - 2: Request for Domain Name corresponding to the IP address\n");
		printf("[Command]:\n1\t<Domain Name>\n2\t<IP Address>\n0\texit\n\n");
		
	// Infinite loop to query for multiple requests from the clients
	while(1) {
			
		int status;
		scanf("%d", &status);
		
		char dns_request1[1024];
		scanf("%s", dns_request1);
		
		// Combining User parameters in a single message
		char dns_request2[1024];
		dns_request2[0] = ('0' + status);
		dns_request2[1] = '#';
		
		strcpy(dns_request2 + 2, dns_request1);
		
		// Sending query to the DNS Proxy
		send(socket_fd, dns_request2, strlen(dns_request2), 0); 
		if(status != 0)
			printf("[PROGRESS]: Requested\n"); 
		
		// Receiving reply from the DNS Proxy
		char dns_reply[1024] = {0}; 
		int valread = recv(socket_fd, dns_reply, 1024, 0);
		
		if(dns_reply[0] == '-') {
			printf("server is down\n");
			break;
		}
		if(status == 0) {
			break;
		}
		
		int res_status = dns_reply[0] - '0';
		printf("[MESSAGE RECEIVED]: %s\n", dns_reply);
		if(res_status == 0)
			printf("[RESULT]: %s\n\n", dns_reply + 2); 
		else
			printf("[RESULT]: %s\n\n", dns_reply + 2);
		
		
		memset(dns_request1, 0, strlen(dns_request1));
		memset(dns_request2, 0, strlen(dns_request2));
	}
	
	
	close(connection_fd);
	close(socket_fd);
	return 0; 
} 
