#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#define BUFFSIZE 1024

struct account
{
    char username[100];
    char password[100];
};

void encrypt(char pass[100])
{
    char a;
    int i = 0;
    int n = strlen(pass);

    while (i < n)
    {
        switch (pass[i])
        {
        case 'a':
            pass[i] = '%';
            break;
        case 'b':
            pass[i] = '@';
            break;
        case 'c':
            pass[i] = ')';
            break;
        case 'd':
            pass[i] = '&';
            break;
        case 'e':
            pass[i] = '$';
            break;
        case 'f':
            pass[i] = '#';
            break;
        case 'g':
            pass[i] = ';';
            break;
        case 'h':
            pass[i] = '1';
            break;
        case 'i':
            pass[i] = 'p';
            break;
        case 'j':
            pass[i] = '^';
            break;
        case 'k':
            pass[i] = 'y';
            break;
        case 'l':
            pass[i] = '-';
            break;
        case 'm':
            pass[i] = '}';
            break;
        case 'n':
            pass[i] = ':';
            break;
        case 'o':
            pass[i] = '4';
            break;
        case 'p':
            pass[i] = '+';
            break;
        case 'q':
            pass[i] = '`';
            break;
        case 'r':
            pass[i] = 's';
            break;
        case 's':
            pass[i] = '>';
            break;
        case 't':
            pass[i] = '<';
            break;
        case 'u':
            pass[i] = '~';
            break;
        case 'v':
            pass[i] = 'r';
            break;
        case 'w':
            pass[i] = 'a';
            break;
        case 'x':
            pass[i] = 'Y';
            break;
        case 'y':
            pass[i] = '7';
            break;
        case 'z':
            pass[i] = '"';
            break;

        case '1':
            pass[i] = 'R';
            break;
        case '2':
            pass[i] = 'V';
            break;
        case '3':
            pass[i] = 'Q';
            break;
        case '4':
            pass[i] = 'T';
            break;
        case '5':
            pass[i] = 'Z';
            break;
        case '6':
            pass[i] = 'W';
            break;
        case '7':
            pass[i] = 'S';
            break;
        case '8':
            pass[i] = '.';
            break;
        case '9':
            pass[i] = 'i';
            break;
        case '0':
            pass[i] = 'D';
            break;

        case '!':
            pass[i] = '_';
            break;
        case '@':
            pass[i] = '!';
            break;
        case '.':
            pass[i] = 'C';
            break;

        default:
            break;
        }
        i++;
    }
}

void connect_account(int server_socket)
{

    struct account acc;
    char c;
    int i = 0;
    printf("\n[server] Username: ");
    fgets(acc.username, sizeof(acc.username), stdin);
    acc.username[strcspn(acc.username, "\n")] = 0;

    printf("\n[server] Password: ");

    fgets(acc.password, sizeof(acc.password), stdin);
    acc.password[strcspn(acc.password, "\n")] = 0;
    encrypt(acc.password);
    send(server_socket, &acc, sizeof(acc), 0);
};

void upload(const char *file_to_upload, const char *location, int server_socket)
{

    FILE *fd_file = fopen(file_to_upload, "rb");
    if (fd_file == NULL)
    {
        perror("Can't open the file_to_upload");
        return;
    }

    // Obtinem dim. fisierului
    fseek(fd_file, 0, SEEK_END);
    long file_size = ftell(fd_file); //pozitia cursorului
    rewind(fd_file);// reseteaza pozitita cursorului

    send(server_socket, &file_size, sizeof(file_size), 0);
    
    int bytes_read;
    char buffer[BUFFSIZE];
    // trimit
    while ((bytes_read = fread(buffer, 1, BUFFSIZE, fd_file)) > 0)
    {
        if (send(server_socket, buffer, bytes_read, 0) < 0)
        {
            perror("Error sending bytes to server");
            fclose(fd_file);
            return;
        }
    }

    printf("\nData sent\n");
    // trimit END dupa ce am terminat
    const char *end_signal = "END\n"; // Adăugați o linie nouă sau terminator
    send(server_socket, end_signal, 3, 0);
    
    fclose(fd_file);
    printf("File uploaded completed!\n");
}
void download(const char *name, int server_socket) {
    FILE *fd = fopen(name, "wb");
    if (fd == NULL) {
        perror("[client] Error creating the file");
        return;
    }

    long file_size;
    if (recv(server_socket, &file_size, sizeof(file_size), 0) <= 0) {
        perror("[client] Error receiving file size");
        fclose(fd);
        return;
    }

    printf("[client] File size to receive: %ld bytes\n", file_size);

    long total_received = 0;
    char buffer[BUFFSIZE];
    int bytes_read;

    while (total_received < file_size) {
        bytes_read = recv(server_socket, buffer, BUFFSIZE, 0);
        if (bytes_read <= 0) {
            perror("[client] Error receiving file data");
            fclose(fd);
            return;
        }

        long bytes_to_write = bytes_read;

        // If receiving more than the remaining file size, adjust
        if (total_received + bytes_read > file_size) {
            bytes_to_write = file_size - total_received;
        }

        if (fwrite(buffer, 1, bytes_to_write, fd) != bytes_to_write) {
            perror("[client] Error writing to file");
            fclose(fd);
            return;
        }

        total_received += bytes_to_write;
    }

    printf("[client] File download completed (%ld/%ld bytes)\n", total_received, file_size);
    fclose(fd);

    // // Receive and print the server's final message
    // char end_message[BUFFSIZE];
    // int n = recv(server_socket, end_message, BUFFSIZE, 0);
    // if (n > 0) {
    //     end_message[n] = '\0';
    //     printf("[client] Server: %s", end_message);
    // }
}




extern int errno;

int port;

int main(int argc, char *argv[])
{
    int server_socket;
    struct sockaddr_in server;
    struct account acc;
    char buff[BUFFSIZE];

    if (argc != 3)
    {
        printf("Usage: %s <server_adress> <port>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[2]);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error command socket().\n");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect(server_socket, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("Error command connect().\n");
        return errno;
    }

    connect_account(server_socket);

    char status_account[BUFFSIZE];

    int n = recv(server_socket, status_account, sizeof(status_account), 0);
    if (n > 0)
    {
        status_account[n] = '\0';
        printf("%s\n", status_account);
    }
    if (strcmp(status_account, "[server] Account found! Connected!") == 0)
    {
        while (1)
        {
            fflush(stdout);
            char buffer[BUFFSIZE];
            printf("Enter command: ");
            memset(buffer, 0, BUFFSIZE);
            fgets(buffer, sizeof(buffer), stdin);

            buffer[strcspn(buffer, "\n")] = '\0';

            if (strncmp(buffer, "upload", 6) == 0)
            {
                char *path = strtok(buffer, " ");
                char file_to_upload[BUFFSIZE];
                char location[BUFFSIZE];

                // file_to_upload
                path = strtok(NULL, " ");
                if (path != NULL)
                    strcpy(file_to_upload, path);
                else
                    printf("invalid command");
                //

                // locatie
                path = strtok(NULL, " ");
                if (path != NULL)
                    strcpy(location, path);
                else
                    printf("invalid command");
                //

                // trimit la server "upload locatie"
                char info_server[BUFFSIZE] = "upload ";
                strcat(info_server, location);
                char ready[BUFFSIZE];
                send(server_socket, info_server, strlen(info_server), 0);
                //

                // primesc ready
                recv(server_socket, ready, sizeof(ready), 0);
                upload(file_to_upload, location, server_socket);
                // printf("Am iesit\n"); //
            }
            else if (strncmp(buffer, "download", 8) == 0){
                char* path = strtok(buffer, " ");
                char file_to_download[BUFFSIZE];
                char new_file_name[BUFFSIZE];
                
                if(path !=NULL)
                    strcpy(file_to_download, path);
                else
                    perror("invalid command");
                

                path = strtok(NULL," ");

                if(path != NULL)
                    strcpy(file_to_download, path);
                else
                    perror("invalid command");

                path = strtok(NULL," ");

                if(path != NULL)
                    strcpy(new_file_name, path);
                else
                    perror("invalid command");
                
                char command_argument[BUFFSIZE] = "download ";
                strcat(command_argument, file_to_download);
                send(server_socket, command_argument,strlen(command_argument), 0);

                printf("\nDownloading..\n");
                download(new_file_name, server_socket);
                printf("I'm out\n");
                send(server_socket, "finish", strlen("finish"),0);
                 
            }
        
            else if (send(server_socket, buffer, strlen(buffer), 0) < 0)
            {
                perror("[client] Error sending the command");
                break;
            }

       
            int n = recv(server_socket, buffer, BUFFSIZE, 0); // File received msg from [server]
           
            buffer[n] = '\0';
            printf("%s", buffer);
            printf("\n\n");
        }
    }

    close(server_socket);
    return 0;
}

// /home/petru10/SiteOS.sh - merge
// /home/petru10/RC_PROJECT/test-upload/specificatie.pdf nu merge
