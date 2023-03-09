// Made by: Mohammad Ibrahim Khan
// TCP Server

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Usage:  %s <port>\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[1]);
    if (port == 0)
    {
        printf("Please enter a valid port number! A port number can only be a numerical value.\n");
        exit(0);
    }
    printf("Port chosen: %d\n", port);

    // Address initialization (Source: TA code on D2L)
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htons(INADDR_ANY);

    // socket creation (Source: TA code on D2L)
    int mysocket1;
    mysocket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (mysocket1 == -1)
    {
        printf("socket() call failed\n");
    }
    else
    {
        printf("socket creation success!\n");
    }

    // binding(server only) (Source: TA code on D2L)
    int status;
    status = bind(mysocket1, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
    if (status == -1)
    {
        printf("bind() call failed\n");
    }
    else
    {
        printf("bind success!\n");
    }

    while (true)
    {
        // listening (server only) (Source: TA code on D2L)
        status = listen(mysocket1, 5);
        if (status == -1)
        {
            printf("listen() call failed\n");
        }
        else
        {
            printf("Listening...\n");
        }

        // connection acceptance (server only) (Source: TA code on D2L)
        int mysocket2;
        mysocket2 = accept(mysocket1, NULL, NULL);
        if (mysocket2 == -1)
        {
            printf("accept() call failed\n");
        }
        printf("Connection accepted! Asking client for UCID...\n");

        // receive UCID from the client (Source: TA code on D2L)
        int count;
        char rcv_ucid[100];
        count = recv(mysocket2, rcv_ucid, sizeof(rcv_ucid), 0);
        if (count == -1)
        {
            printf("recv() call failed.\n");
        }
        else
        {
            printf("The UCID received is: %s \n", rcv_ucid);
        }

        // Sending current date and time to the client
        // source used: https://www.youtube.com/watch?v=h_b16pSouM4&t=1028s&ab_channel=IdiotDeveloper

        time_t currentTime;
        time(&currentTime);
        printf("The client will receive the following time: %s", ctime(&currentTime));
        send(mysocket2, ctime(&currentTime), 30, 0);

        // Creating A PASSCODE in the server
        char serverTime[50];
        ctime_r(&currentTime, serverTime); // saving the time to use for creating the password in the server

        char serverpasscode[9];
        serverpasscode[0] = serverTime[17]; // first second in the time
        serverpasscode[1] = serverTime[18]; // second second in the time
        serverpasscode[2] = rcv_ucid[4];    // 4th last character in the ucid
        serverpasscode[3] = rcv_ucid[5];    // 3rd last character in the ucid
        serverpasscode[4] = rcv_ucid[6];    // 2nd last character in the ucid
        serverpasscode[5] = rcv_ucid[7];    // the last character in the ucid

        printf("\n");
        printf("Passcode made in the server is: %s \n", serverpasscode);

        // READ AN INTEGER FROM THE CLIENT( the password it made)
        int receivePasscode;
        char rcv_passcode[50];
        receivePasscode = recv(mysocket2, rcv_passcode, 50, 0);
        if (receivePasscode == -1)
        {
            printf("recv() call failed.\n");
        }
        else
        {
            printf("The passcode received from the client is: %s \n", rcv_passcode);
        }

        if (atoi(serverpasscode) == atoi(rcv_passcode))
        {
            printf("The passwords match! The server has been authorized to continue with the file transfer.\n");
        }
        else
        {
            printf("The passwords DO NOT match. The server has not been authorized to continue with the file transfer. Exiting server....\n");
            exit(1);
        }

        // Reading the data.txt file and sending its content over to the client
        // source used: https://github.com/psp316r/File-Transfer-Using-TCP-Socket-in-C-Socket-Programming
        FILE *fp;
        char *filename = "data.txt";

        fp = fopen(filename, "r");

        int SIZE = 1024;

        char data[1024] = {0};

        printf("Transferring data from data.txt file to client...\n");

        while (fgets(data, SIZE, fp) != NULL)
        {
            if (send(mysocket2, data, sizeof(data), 0) == -1)
            {
                perror("[-] Error in sending data");
                exit(1);
            }
            bzero(data, SIZE);
        }
        printf("Done transferring! Client has received the file contents.\n");

        fclose(fp);
        printf("The file has been closed on server side.\n");

        // Closing sockets and ending connection to the client

        close(mysocket2);

        printf("The client socket has now been closed on the server side.\n");
        printf("Restarting server listen command...\n");
    }
}
