#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <zconf.h>

#define SOCKET_PORT 56987
#define PROXY_SOCKET_PORT 56990
#define BUFFER_MAX_SIZE 1024

int sockfd;
int sockfdl;
pthread_t t;

int client_sock;
int intechos_sock;
char client_is_present = 0;
char intechos_is_present = 0;

void* intechosToClient(void *);

void signalHandler(int sign)
{
    if(sign == SIGINT)
    {
        pthread_cancel(t);
        close(sockfd);
        close(sockfdl);
        close(intechos_sock);
        close(client_sock);
        exit(0);
    }
}

int main(int argc, char ** argv) {
    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        printf("\ncan't catch SIGINT\n");
    }

    pthread_create(&t, NULL, intechosToClient, NULL);

    struct sockaddr_in serv_addr;

    for( ; ; )
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0); // generate file descriptor
        if (sockfd < 0) {
            perror("ERROR opening socket");
            exit(1);
        }


        memset((char *) &serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        // memcpy(server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t) server->h_length);
        serv_addr.sin_port = htons(PROXY_SOCKET_PORT);


        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR binding");
            close(sockfd);
            continue;
        }

        listening:
        if (listen(sockfd, 1) < 0) {
            perror("ERROR connecting");
            close(sockfd);
            continue;
        }

        struct sockaddr_un client_name;
        socklen_t client_name_len = sizeof(struct sockaddr_un);
        intechos_sock = accept(sockfd, (struct sockaddr *) &client_name, &client_name_len);

        if (intechos_sock < 0) {
            perror("ERROR opening INTechOS socket");
            close(sockfd);
            continue;
        }

        intechos_is_present = 1;

        while (intechos_is_present) {
            char rbuff[BUFFER_MAX_SIZE];
            ssize_t rbytes;

            rbytes = recv(intechos_sock, rbuff, sizeof(rbuff), 0); // similar to read(), but return -1 if socket closed

            if (rbytes < 0) {
                perror("ERROR socket is unavailable");
                close(intechos_sock);
                goto listening;
            }

            for (ssize_t i = rbytes - 1; i < sizeof(rbuff); i++) rbuff[i] = '\0';


            if (client_is_present) {
                if (write(client_sock, rbuff, sizeof(rbuff)) < 0) {
                    close(client_sock);
                    client_is_present = 0;
                }
            }
        }

        close(sockfd);
    }
    return 0;
}


void* intechosToClient(void *null)
{
    for( ; ; )
    {
        struct sockaddr_in serv_addr;

        sockfdl = socket(AF_INET, SOCK_STREAM, 0); // generate file descriptor
        if (sockfdl < 0) {
            perror("ERROR opening socket");
            exit(1);
        }


        memset((char *) &serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        // memcpy(server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t) server->h_length);
        serv_addr.sin_port = htons(SOCKET_PORT);


        if (bind(sockfdl, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR binding");
            close(sockfdl);
            continue;
        }

        listeningl:if (listen(sockfdl, 1) < 0) {
        perror("ERROR connecting");
        close(sockfdl);
        continue;
    }

        struct sockaddr_un client_name;
        socklen_t client_name_len = sizeof(struct sockaddr_un);
        client_sock = accept(sockfdl, (struct sockaddr *) &client_name, &client_name_len);

        if (client_sock < 0) {
            perror("ERROR opening client socket");
            close(sockfdl);
            continue;
        }

        client_is_present = 1;

        while (client_is_present)
        {
            char rbuff[BUFFER_MAX_SIZE];
            ssize_t rbytes;

            rbytes = recv(client_sock, rbuff, sizeof(rbuff), 0); // similar to read(), but return -1 if socket closed

            if(rbytes < 0)
            {
                perror("ERROR socket is unavailable");
                close(client_sock);
                goto listeningl;
            }

            for(ssize_t i=rbytes-1 ; i<sizeof(rbuff) ; i++) rbuff[i] = '\0';


            if(intechos_is_present)
            {
                if(write(intechos_sock, rbuff, sizeof(rbuff)) < 0)
                {
                    close(intechos_sock);
                    intechos_is_present = 0;
                }
            }
        }

        close(sockfdl);

    }

}