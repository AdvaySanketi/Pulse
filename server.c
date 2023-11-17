#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_SIZE 100
#define HASH_TABLE_SIZE 50

struct LogEntry {
    char username[20];
    char ipAddress[16];
    char timestamp[20];
};

struct HashTable {
    struct Node* entries[HASH_TABLE_SIZE];
};

struct Node {
    struct LogEntry data;
    struct Node* next;
    struct Node* prev;
};

struct Queue {
    struct LogEntry items[MAX_SIZE];
    int front;
    int rear;
    int size;
};

struct Credentials {
    char username[20];
    char password[20];
};

// Create the Queue
struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    return queue;
}

// Create the Hash Table
struct HashTable* createHashTable() {
    struct HashTable* table = (struct HashTable*)malloc(sizeof(struct HashTable));
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        table->entries[i] = NULL;
    }
    return table;
}

// Authenticate the User Credentials
int authenticate(struct Credentials* credentials) {
    if (strcmp(credentials->username, "Advay") == 0 && strcmp(credentials->password, "A28") == 0) {
        return 1;
    }
    return 0;
}

// Create Hash value for the logs based on the Username
int hashFunction(char* username) {
    int hash = 0;
    while (*username) {
        hash += *username++;
    }
    return hash % HASH_TABLE_SIZE;
}

// Add logs to the hash table
void insertEntry(struct HashTable* table, struct LogEntry* entry) {
    int hash = hashFunction(entry->username);
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = *entry;
    newNode->next = table->entries[hash];
    newNode->prev = NULL;
    if (table->entries[hash] != NULL) {
        table->entries[hash]->prev = newNode;
    }
    table->entries[hash] = newNode;
}

// Filter logs for a specific username
void getLogEntriesForUser(struct HashTable* table, char* username) {
    struct Credentials hostCredentials;
    printf("Username - ");
    scanf("%s", hostCredentials.username);
    printf("Password - ");
    scanf("%s", hostCredentials.password);

    if(authenticate(&hostCredentials))
    {
        int hash = hashFunction(username);
        struct Node* current = table->entries[hash];

        printf("Log Entries for User '%s':\n\n", username);

        while (current != NULL) {
            if (strcmp(current->data.username, username) == 0) {
                printf("IP Address: %s", current->data.ipAddress);
                printf("Timestamp: %s\n\n", current->data.timestamp);
            }
            current = current->next;
        }
    }
    else {
        printf("Invalid Username / Password\n\n");
    }
}

// Add logs to the queue
void enqueue(struct Queue* queue, struct LogEntry* Entry) 
{
    queue->rear = (queue->rear + 1) % MAX_SIZE;
    queue->items[queue->rear] = *Entry;
    queue->size++;
}

// Display all the logs in the Queue
void displayQueue(struct Queue* queue) {
    int i;
    struct Credentials hostCredentials;
    printf("Username - ");
    scanf("%s", hostCredentials.username);
    printf("Password - ");
    scanf("%s", hostCredentials.password);
    if(authenticate(&hostCredentials))
    {
        printf("\nLogs:\n");
        for (i = 0; i < queue->size; ++i) {
            int index = (queue->front + i) % MAX_SIZE;
            printf("Username: %s\n", queue->items[index].username);
            printf("IP Address: %s", queue->items[index].ipAddress);
            printf("Timestamp: %s\n\n", queue->items[index].timestamp);
        }
    }
    else {
        printf("Invalid Username / Password\n\n");
    }
}

// Check if the log is older than a day
int isOlderThanOneDay(char* timestamp) {
    struct tm logTime;
    time_t currentTime, entryTime;
    time(&currentTime);
    sscanf(timestamp, "%d-%d-%d %d:%d:%d", &logTime.tm_year, &logTime.tm_mon, &logTime.tm_mday,
       &logTime.tm_hour, &logTime.tm_min, &logTime.tm_sec);
    logTime.tm_year -= 1900;
    logTime.tm_mon -= 1;
    entryTime = mktime(&logTime);
    return difftime(currentTime, entryTime) > 30; // 86400 seconds in a day
}

// To remove logs older than a day
void dequeueOldEntries(struct Queue* queue) {
    while (queue->size > 0 && isOlderThanOneDay(queue->items[queue->front].timestamp)) {
        queue->front = (queue->front + 1) % MAX_SIZE;
        queue->size--;
    }
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    char ch;
    struct sockaddr_in server, client;
    struct LogEntry receivedEntry;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock");
        return 1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0); // Create the server
    if (server_socket == INVALID_SOCKET) {
        printf("Failed to create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(12345);

    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) { // Start the Server
        printf("Bind failed");
        closesocket(server_socket);
        return 1;
    }

    listen(server_socket, 5);
    printf("Server listening on port 12345...\n\n");

    struct HashTable* logTable = createHashTable(); // Create the Hash Table
    struct Queue* queue = createQueue(); // Create the Queue

    char command[256];
    do {
        // Commands
        scanf(" %c",&ch);
        while (getchar() != '\n');
        if (ch == 'show') {
            displayQueue(queue); // Display all the logs
        } else if (ch == 'search') {
            char targetUsername[20];
            printf("Enter the username to retrieve log entries: ");
            scanf("%s", targetUsername);
            getLogEntriesForUser(logTable, targetUsername); // Filter Logs for a specific username
        }

        int client_size = sizeof(client);
        client_socket = accept(server_socket, (struct sockaddr*)&client, &client_size); // Accept Client Connection
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed");
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }

        recv(client_socket, (char*)&receivedEntry, sizeof(struct LogEntry), 0);
        enqueue(queue, &receivedEntry); // Add logs to the queue
        insertEntry(logTable, &receivedEntry); // Add logs to the hash table

        dequeueOldEntries(queue); // To remove logs older than a day

        closesocket(client_socket); // Close the Client Connection

    } while(1);

    closesocket(server_socket); // Close the Server Connection
    WSACleanup();

    return 0;
}
