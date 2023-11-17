# Pulse
Efficient Client-Server Monitoring for External Devices

## Synopsis
Our project revolves around a sophisticated client-server model designed to monitor external device connections. The server, hosted on a central system, leverages the power of Queues for efficient timestamp and IP storage whenever a USB or external device connects. Clients, situated on individual systems, seamlessly communicate with the server, enabling seamless and organized data logging. Our innovative approach optimizes space utilization by employing enqueue operations for data entry and periodic dequeue operations to remove outdated records. To facilitate rapid log retrieval based on usernames, we've implemented a Hash Table, adding a layer of efficiency to our system.
This implementation serves as a testament to the practical application of data structures, providing an organized and space-efficient solution for monitoring external hardware devices. The client-server architecture ensures scalability and streamlined communication, making our solution versatile and well-suited for deployment in diverse computing environments.

## To run the program - 

To run the server
### `gcc -o server server.c -lws2_32`
### `./server`

To run the client
### `gcc -o client client.c -lws2_32`
### `./client`

**Note: Remember to change the `IP Address` in the client file to the IP address of the Server Computer**

To find the IP Address, open the cmd and run
### `ipconfig`