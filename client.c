#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

struct LogEntry {
    char username[20];
    char ipAddress[16];
    char timestamp[20];
};

// Grab the IP Address of the Device and the current timestamp
void get_data(struct LogEntry* newEntry)
{
    FILE *fp;
    char buffer[256];

    printf("Username - ");
    scanf("%s", buffer);
    strcpy(newEntry->username, buffer); // Get Username

    system("ipconfig | findstr /r /c:\"IPv4\" > ip_address.txt"); // Get IP Address

    fp = fopen("ip_address.txt", "r");
    if (fp != NULL) {
        fgets(newEntry->ipAddress, sizeof(buffer), fp);
        sprintf(newEntry->ipAddress, "%.*s", 15, newEntry->ipAddress +39);
        fclose(fp);
    } else {
        strcpy(newEntry->ipAddress, "Unknown");
    }

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(newEntry->timestamp, 20, "%Y-%m-%d %H:%M:%S", timeinfo); // Get Timestamp
}

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    struct LogEntry newEntry;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock");
        return 1;
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0); // Create the Client
    if (client_socket == INVALID_SOCKET) {
        printf("Failed to create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("IP Address"); 

    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) { // Start the Client and Connect to the server
        printf("Connection failed");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    get_data(&newEntry); // Grab the IP Address of the Device and the current timestamp
    send(client_socket, (char*)&newEntry, sizeof(struct LogEntry), 0); // Send the Data to the Server

    closesocket(client_socket); // Close the Client Connection
    WSACleanup();

    return 0;
}
