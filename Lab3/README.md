# Lab 3: Multi-stage DNS Resolving System using Client-Server socket programming

![dns](dns.PNG)

## Usage
Run following commands on a new terminal:
```
gcc server.c -o server
./server <server_port_no>
```

```
gcc multithreaded_proxy.c -o proxy -pthread
./proxy <server_ip_address>  <server_port_no>  <proxy_port_no>
```

```
gcc client.c -o client
./client <proxy_ip_address>   <proxy_port_no>
```

## Example

```
gcc server.c -o server
./server 12005

gcc multithreaded_proxy.c -o proxy -pthread
./proxy 127.0.0.1 12005 12006

gcc client.c -o client
./client 127.0.0.1 12006
```

## Available commands
The client supports following arguments:

1. ```1 <domain_name>``` will retrieve ip address corresponding to the specified domain name.
2. ```2 <ip_address>``` will retrieve domain name corresponding to the specified ip address.
3. ``` 0 exit``` will gracefully close the connection of the client with the proxy.  


* The available ```domain_name``` and ```ip_address``` can be found in the ```database.txt``` file.

* Any other type of message will lead to closing of the connection with the client. So, one needs to connect to the proxy again.