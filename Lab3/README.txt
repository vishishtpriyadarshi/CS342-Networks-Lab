Name1 - VISHISHT PRIYADARSHI
Roll1 - 180123053

Name2 - ASHISH KUMAR BARNAWAL
Roll2 - 180123006


NOTE: If anytime some error in socket binding occurs, kindly change the port no.

 ===================  HOW TO EXECUTE  ===================

Kindly run these commands in the following order:


----------------------------------------------------------------------
	<Open new tab/terminal>
1.	gcc server.c -o server
2.	./server 12005			

	<Open new tab/terminal>
3. 	gcc multithreaded_proxy.c -o proxy -pthread
4.  ./proxy 127.0.0.1 12005 12006			[1st Port No should match with that of server]

	<Open new tab/terminal>
5. 	gcc client.c -o client
6.	./client 127.0.0.1 12006				[This port no should match with the port no (3rd argument) given in line 3]
------------------------------------------------------------------------



=> step 6 can be repeated multiple times for connecting multiple clients
=> feel free to change the port number, but they should match with the following format:

-------------------------------------------------------------------------------------
SERVER:  <executable_code>  <server_port_no>
PROXY:	 <executable_code>  <server_ip_address>  <server_port_no>  <proxy_port_no>
CLIENT:	 <executable_code>  <proxy_ip_address>   <proxy_port_no>
-------------------------------------------------------------------------------------


=> Proxy acts as both client and server, hence, its execution is like this





 ===================  PASSING ARGUMENTS IN CLIENT  ===================
=> client supports following arguments:

1. 1 <domain_name>  	=> retrieves ip address corresponding to the specified domain name
2. 2 <ip_address>		=> retrieves domain name corresponding to the specified ip address
3. 0 exit				=> gracefully closes the connection of the client with the proxy

=> any other type of message will lead to closing of connection with the client. So, one need to connect to the proxy again.
=> one can connect multiple clients
 
 
 
 
 
 ===================  OUTPUTS IN VARIOUS CASES  ===================
=> I have printed readable output which will make one understand how process is executing in each of the client, proxy and server. Feel free to read those, they are interesting.



