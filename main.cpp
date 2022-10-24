#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <windows.h>

void showboard(char arr[], int p) 
{
    printf(" _________________\n");
    printf("|     |     |     |\n");
    printf("|  %c  |  %c  |  %c  |\n", arr[0], arr[1], arr[2]);

    printf("|_____|_____|_____|\n");
    printf("|     |     |     |\n");

    printf("|  %c  |  %c  |  %c  |\n", arr[3], arr[4], arr[5]);

    printf("|_____|_____|_____|\n");
    printf("|     |     |     |\n");

    printf("|  %c  |  %c  |  %c  |\n", arr[6], arr[7], arr[8]);

    printf("|_____|_____|_____|\n\n");
}

void setboard(char arr[], int p, int val)
{
    if (p == 1) {
        arr[val-49] = 'X';
    }
    else {
        arr[val-49] = 'O';
    }
    showboard(arr, p);
}

int fincheck(char arr[])
{
    if ((arr[0] == 'O' && arr[1] == 'O' && arr[2] == 'O') ||
        (arr[3] == 'O' && arr[4] == 'O' && arr[5] == 'O') ||
        (arr[6] == 'O' && arr[7] == 'O' && arr[8] == 'O') ||
        (arr[0] == 'O' && arr[3] == 'O' && arr[6] == 'O') ||
        (arr[1] == 'O' && arr[4] == 'O' && arr[7] == 'O') ||
        (arr[2] == 'O' && arr[5] == 'O' && arr[8] == 'O') ||
        (arr[0] == 'O' && arr[4] == 'O' && arr[8] == 'O') ||
        (arr[2] == 'O' && arr[4] == 'O' && arr[6] == 'O'))
    {
        printf("Player2 win!\n");
        return 1;
    }
    else if ((arr[0] == 'X' && arr[1] == 'X' && arr[2] == 'X') ||
        (arr[3] == 'X' && arr[4] == 'X' && arr[5] == 'X') ||
        (arr[6] == 'X' && arr[7] == 'X' && arr[8] == 'X') ||
        (arr[0] == 'X' && arr[3] == 'X' && arr[6] == 'X') ||
        (arr[1] == 'X' && arr[4] == 'X' && arr[7] == 'X') ||
        (arr[2] == 'X' && arr[5] == 'X' && arr[8] == 'X') ||
        (arr[0] == 'X' && arr[4] == 'X' && arr[8] == 'X') ||
        (arr[2] == 'X' && arr[4] == 'X' && arr[6] == 'X'))
    {
        printf("Player1 win!\n");
        return 2;
    }
    else if (arr[0] != ' ' && arr[1] != ' '&& arr[2] != ' '&& arr[3] != ' '&& arr[4] != ' '
        && arr[5] != ' '&& arr[6] != ' '&& arr[7] != ' '&& arr[8] != ' ')
    {
        printf("Draw\n");
        return 3;
    }
    return 0;
}


int main(void)
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);

    int role = 1;
    printf("Choose Player 1(client) or 2(server) \nPlayer2 do first\n>");
    scanf("%d", &role);
    getchar();

    SOCKET s = INVALID_SOCKET;
    
    char val[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    int count = role == 1 ? 0 : 1;

    if (role == 1) //client  
    {
        s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET)
        {
            fprintf(stderr, "[!] Failed to create a socket!\n");
            return 1;
        }

        char server_ip_address[20];
        printf("Server IP address: ");
        scanf("%s", server_ip_address);
        getchar();

        sockaddr_in sin = { 0 };
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = inet_addr(server_ip_address);
        sin.sin_port = htons(5000);
        if (connect(s, (const sockaddr*)&sin, sizeof sin) == SOCKET_ERROR)
        {
            fprintf(stderr, "[!] Failed to cinnect to the server!\n");
            return 1;
        }
        system("cls");
        printf("Player%d\n", role);
        showboard(val, role);
    }
    else // server
    {
        SOCKET listener_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listener_socket == INVALID_SOCKET)
        {
            fprintf(stderr, "[!] Failed to create a listener socket!\n");
            return 1;
        }

        sockaddr_in sin = { 0 };
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        sin.sin_port = htons(5000);
        if (bind(listener_socket, (const sockaddr*)&sin, sizeof sin) == SOCKET_ERROR)
        {
            fprintf(stderr, "[!] Failed to bind the listener socket!\n");
            return 1;
        }

        if(listen(listener_socket, SOMAXCONN) == SOCKET_ERROR)
        {
            fprintf(stderr, "[!] Failed to make the listener socket in the listeneing mode!\n");
            return 1;
        }

        sockaddr_in client_addr;
        int client_addr_len = sizeof client_addr;
        s = accept(listener_socket, (sockaddr*)&client_addr, &client_addr_len);
        if (s == INVALID_SOCKET)
        {
            fprintf(stderr, "[!] Failed to accept an incoming connection request!\n");
            return 1;
        }
        system("cls");
        printf("Player%d Your turn!\n", role);
        showboard(val,role);
        closesocket(listener_socket);
    }

    for (;;)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(s, &readfds);

        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        if (select(0, &readfds, NULL, NULL, &tv) > 0)
        {
            char message[256];
            const int message_len = recv(s, message, sizeof message - 1, 0);
            if (message_len >= 0) 
            {
                message[message_len] = '\0';
                system("cls");
                printf("Player%d Your turn!\n", role);

                setboard(val, role == 1 ? 2: 1 , message[0]);
                count = 1;
                if (fincheck(val) != 0)
                {
                    printf("Prass any key for exit\n");
                    getchar();
                    return 0;
                }
            }
        }

        if (_kbhit() && getch() == 13) 
        {
            char message_to_send[256];
            printf("message> ");
            fgets(message_to_send, sizeof message_to_send, stdin);
            if (*message_to_send >= '1' && *message_to_send <= '9' && val[message_to_send[0]-49] == ' ' && count == 1)
            {
                system("cls");
                printf("Player%d\n", role);
                setboard(val, role, message_to_send[0]);
                send(s, message_to_send, strlen(message_to_send), 0);
                count = 0;
                if (fincheck(val) != 0)
                {
                    printf("Prass any key for exit\n");
                    getchar();
                    return 0;
                }
            }
        }
    }
    return 0;
}

