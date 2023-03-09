// Made by: Mohammad Ibrahim Khan
// TCP Client

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage:  %s <port>\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[1]);

    printf("Port chosen: %d\n", port);

    // Address initialization (Source: TA code on D2L)
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // socket creation (Source: TA code on D2L)
    int mysocket1;
    mysocket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (mysocket1 == -1)
    {
        printf("socket() call failed\n");
    }
    else
    {
        printf("Socket created successfully!\n");
    }

    // connect request (client only) (Source: TA code on D2L)
    int status;
    status = connect(mysocket1, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
    if (status == -1)
    {
        printf("connect() call failed\n");
    }
    else if (status > 0)
    {
        printf("Connected to the server!\n");
    }

    // Asking the user for their UCID

    char snd_ucid[100];
    printf("Enter your UCID number: \n");
    scanf("%s", snd_ucid);

// Error checking for a valid UCID.
top:
    int j;
    int len = strlen(snd_ucid);

    if (len != 8)
    { // length must be 8 numbers
        memset(snd_ucid, 0, sizeof(snd_ucid));
        printf("A UCID consists of a total of 8 digits, and no letters/special characters. Please enter a valid UCID: \n");
        scanf("%s", snd_ucid);
        goto top;
    }
    for (j = 0; j < len; j++)
    {
        if (isdigit(snd_ucid[j]))
        {
            continue;
        }
        else
        {
            memset(snd_ucid, 0, sizeof(snd_ucid));
            printf("A UCID must contain only digits. Please enter a valid UCID: \n"); // UCID must only contain digits
            scanf("%s", snd_ucid);
            goto top;
        }
    }

    // Send the UCID to the server

    int count;
    count = send(mysocket1, snd_ucid, sizeof(snd_ucid), 0);
    if (count == -1)
    {
        printf("send() call failed.\n");
    }
    else
    {
        printf("Sent: %s to server\n");
    }

    // Accept/read current date and time received from the server
    // source used: https://www.youtube.com/watch?v=h_b16pSouM4&t=1028s&ab_channel=IdiotDeveloper

    char clientTime[30];
    recv(mysocket1, clientTime, 29, 0);
    printf("Time received from the server: %s\n", clientTime);

    // CREATE THE clientpasscode (17 and 18th spots in the time received are seconds)
    char clientpasscode[9];
    clientpasscode[0] = clientTime[17]; // first second in the time
    clientpasscode[1] = clientTime[18]; // second second in the time
    clientpasscode[2] = snd_ucid[4];    // 4th last character in the ucid
    clientpasscode[3] = snd_ucid[5];    // 3rd last character in the ucid
    clientpasscode[4] = snd_ucid[6];    // 2nd last character in the ucid
    clientpasscode[5] = snd_ucid[7];    // the last character in the ucid
    printf("\n");
    printf("Passcode made in the client is: %s ", clientpasscode);

    // Send THE clientpasscode to the server
    int sendPasscode;
    sendPasscode = send(mysocket1, clientpasscode, strlen(clientpasscode), 0);
    if (count == -1)
    {
        printf("send() call failed.\n");
    }
    else
    {
        printf("Sent: %s to server\n");
    }

    // File receive and writing it to a new file in the client directory
    // source used: https://github.com/psp316r/File-Transfer-Using-TCP-Socket-in-C-Socket-Programming

    int SIZE = 1024;
    int n;
    FILE *fp;
    char *filename = "clientfile.txt";
    char buffer[SIZE];

    fp = fopen(filename, "w");
    printf("Creating a new file....\n");
    if (fp == NULL)
    {
        perror("[-]Error in creating the file.\n");
        exit(1);
    }
    while (1)
    {
        n = recv(mysocket1, buffer, SIZE, 0);
        if (n <= 0)
        {
            break;
        }
        fprintf(fp, "%s", buffer);
        printf("%d bytes Received from the Server!\n", strlen(buffer));
        bzero(buffer, SIZE);
    }
    printf("Transferring complete! New file has been created with the server content. The file name is 'clientfile.txt' \n");

    fclose(fp);
    printf("The file has been closed on client side.\n");

    // Closing the socket and ending connection with the server
    close(mysocket1);

    printf("The client's socket has now been closed on the client side.\n");
}
