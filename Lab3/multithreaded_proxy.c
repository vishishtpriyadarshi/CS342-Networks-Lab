#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <stdbool.h>
#include <arpa/inet.h> 
#include <ctype.h>
#include <pthread.h>

#define MAX_CONCURRENT_CLIENTS 5


const char *server_addr;
int PROXY_PORT_NO;
int SERVER_PORT_NO;

char host_cache[3][1024];
char ip_cache[3][1024];
int idx = 0;


bool existsInCache(int status, char *request_msg){

	for(int i = 0; i < 3; i++) {
		if(strcmp(request_msg, host_cache[i]) == 0)
			return 1;
		if(strcmp(request_msg, ip_cache[i]) == 0)
			return 1;
	}
	
	return 0;
}


void updateCache(char *request_msg, char *message, int status){

	if(status == 1) {
		strcpy(host_cache[idx], request_msg);
		strcpy(ip_cache[idx], message + 2);
	}
	else if(status == 2) {
		strcpy(host_cache[idx], message + 2);
		strcpy(ip_cache[idx], request_msg);
	}
	else {
		printf("\n\n**************************\n\n");
	}
	
	idx = (idx + 1) % 3;
}


bool retrieveQuery(char *request_msg, char *reply, int status) {

	if(status == 1) {
		for(int i = 0; i < 3; i++) {
			if(strcmp(request_msg, host_cache[i]) == 0){
				strcpy(reply, ip_cache[i]);
				return 1;
			}
		}
	}
	else if(status == 2) {
		for(int i = 0; i < 3; i++) {
			if(strcmp(request_msg, ip_cache[i]) == 0){
				strcpy(reply, host_cache[i]);
				return 1;
			}
		}
	}
	
	strcpy(reply, "Message Type incorrect");
	return 0;
}


void printCache() {
	
	printf("**************** CACHE *******************\n");
	printf("SI No\t\tHost\t\tIP\n");
	for(int i = 0; i < 3; i++) {
		printf("%d\t%s\t\t%s\n", i, host_cache[i], ip_cache[i]);
	}
	printf("\n");
}


int queryServer(char *request_msg, char *reply){
	
	printf("[PROGRESS]: Contacting the server\n");
	printf("[REQUESTED FOR]: %s\n", request_msg + 2);
	struct sockaddr_in serverAddress;
	int socket_fd, connection_fd;
	
	
	// Creating the socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0) {
		printf("[ERROR]: Unable to create socket\n");
		return -1;
	}
	else {
		printf("[SUCCESS]: Socket created\n");
	}
	
	
	// Configuring socket parameters for the Proxy-Server Connection
	memset(&serverAddress, '\0', sizeof serverAddress);
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(server_addr);
	serverAddress.sin_port = htons( SERVER_PORT_NO );
	
	
	// Setting up the connection with DNS Server
	connection_fd = connect(socket_fd, (struct sockaddr *)&serverAddress, sizeof serverAddress);
	
	if(connection_fd < 0) {
		printf("[ERROR]: Failed to connect to the server\n");
		return -1;
	}
	else {
		printf("[SUCCESS]: Connected to the server\n");
	}
	
	// Sending query to the DNS Server
	send(socket_fd, request_msg, strlen(request_msg), 0);
	
	
	memset(reply, 0, strlen(reply));
	int recv_status = recv(socket_fd, reply, 1024, 0); 
	close(socket_fd);
	
	if(reply[0] == '-') {
		return -1;
	}
	
	return (reply[0] - '0');
}


void *child_thread(void *args) {

	int connection_fd = *(int*)args;
	pthread_detach(pthread_self());
		
	while(1) {
		char *reply = (char *) malloc(1024);
		char buffer[1024]; 
	
		memset(buffer, 0, strlen(buffer));
				
		// Receiving the requested message from the client
		int recv_status = recv(connection_fd, buffer, 1024, 0); 
		int type_of_message = buffer[0] - '0';
		printf("[PROGRESS]: Type of message received from the Client = %d\n", type_of_message);
				
		// Client closed the connection
		if(type_of_message == 0) {
			printf("[PROGRESS]: Client Disconnected\n");
			break;
		}
		else if(type_of_message != 1 && type_of_message != 2) {
			printf("[UNKNOWN]: Unidentified Request Received\n");
			break;
		}
		
		
		int server_status = 0;
		char request_msg[strlen(buffer) - 2];
		memcpy(request_msg, &buffer[2], strlen(buffer) - 2);
		request_msg[strlen(buffer) - 2] = '\0';
						
		if(type_of_message == 1) {
			printf("Domain Name = %s.\n", request_msg);
			printf("[SEARCHING]...\n\n");
		}
		else {
			printf("I/P Address = %s.\n", request_msg);
			printf("[SEARCHING]...\n\n");
		}
		

		printf("[STATUS]: Cache status before DNS lookup\n");		
		printCache();
		bool flg = existsInCache(type_of_message, request_msg);
		
		if(recv_status != 0) {
			if(flg) {
				
				bool valid = retrieveQuery(request_msg, reply, type_of_message);
				char temp[1024];
				
				if(valid) {
					printf("[PROGRESS]: Found in the Cache!! Retrieving from the Cache\n");
					temp[0] = '3';
				}
				else {
					printf("[PROGRESS]: Message Type Mismatch\n");
					temp[0] = '4';
				}
				
				temp[1] = '#';
				strcpy(temp + 2, reply);
				strcpy(reply, temp);
			}
			else {
				
				printf("[PROGRESS]: Record not found in the cache\n");
				
				// Querying the DNS Server
				server_status = queryServer(buffer, reply);
						
				if(server_status == 3) {
					updateCache(request_msg, reply, type_of_message);
					printf("[PROGRESS]: Cache Updated\n");
				}
				else if(server_status == -1) {
					strcpy(reply, "-1#Server is Down");
					printf("[ERROR]: Server is down\n");
				}
			}
		}
		printf("[MESSAGE SENT]: %s\n\n", reply);
				
		// Replying to the Client
		send(connection_fd, reply, strlen(reply), 0);
		
		if(server_status == -1)
			break;
	}
			
	// Client closed
	printf("[PROGRESS]: Closing the connection with the client\n\n");
	close(connection_fd);	
	
}

int main(int argc, char const *argv[]) 
{ 
	pthread_t thread_id;
	
	// Validating User Parameters
	if(argc != 4) {
		printf("[USAGE]: <executable code> <Server IP Address> <Server Port number> <Proxy Port Number>\n");
		return 0;
	}
	
	server_addr = argv[1];
	SERVER_PORT_NO = atoi(argv[2]);
	PROXY_PORT_NO = atoi(argv[3]);
	
	int socket_fd, connection_fd; 
	struct sockaddr_in serverAddress, clientAddress; 
	
	// Creating the socket  
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(socket_fd < 0) { 
		printf("[ERROR]: Unable to create socket\n");
		exit(EXIT_FAILURE); 
	}
	else {
		printf("[SUCCESS]: Socket created\n");
	}
	
	
	// Configuring socket parameters for the Proxy-Client Connection
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_addr.s_addr = INADDR_ANY; 
	serverAddress.sin_port = htons( PROXY_PORT_NO ); 
	
	
	// Binding the socket to the specified port
	int bind_res = bind(socket_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if(bind_res < 0) { 
		printf("[ERROR]: Failed to bind to the socket\n"); 
		exit(EXIT_FAILURE); 
	} 
	else {
		printf("[SUCCESS]: Successfully binded\n");
	}
	
	
	// Listening for the requests from the Clients
	if(listen(socket_fd, MAX_CONCURRENT_CLIENTS) < 0) { 
		printf("[ERROR]: Unable to Listen\n");
		exit(EXIT_FAILURE); 
	} 
	else {
		printf("[SUCCESS]: Listening\n");
	}
	
	int clientAddress_len = sizeof clientAddress ;
	
	while(1)
    {
    	connection_fd =  accept(socket_fd, (struct sockaddr *)&clientAddress, &clientAddress_len);
		if(connection_fd < 0) { 
			printf("[ERROR]: Refused to connect\n"); 
			exit(EXIT_FAILURE); 
		} 
		else {
			printf("[SUCCESS]: Connection Established\n");
		}
    	
		if(pthread_create(&thread_id, NULL, child_thread, (void *) &connection_fd) < 0) {
			printf("[ERROR]: Could not create thread\n");
			return 1;
		}
		printf("\n\n[WELCOME]: New client connected\n\n");
		
	}
	
	printf("[COMPLETED]: Proxy Server Closed\n"); 
	close(socket_fd);
	return 0; 
} 
