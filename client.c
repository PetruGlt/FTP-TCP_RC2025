#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define BUFFSIZE 1024

extern int errno;

struct account
{
    char username[100];
    char password[100];
};

struct account copy;

void create_account(){

    struct account acc;
    
    printf("\n[server] Username: ");

    fgets(acc.username, sizeof(acc.username), stdin);
    acc.username[strcspn(acc.username, "\n")] = 0;

    printf("\n[server] Password: ");

    fgets(acc.password, sizeof(acc.password), stdin);
    acc.password[strcspn(acc.password, "\n")] = 0;

    FILE *fd = fopen("/home/petru10/RC_PROJECT/workspace/FTP-TCP_RC2025/server_folder/whitelist.txt","a");
    char text[BUFFSIZE]="\n";

    snprintf(text, sizeof(text), "\n%s %s", acc.username, acc.password);
    fwrite(text,sizeof(char),strlen(text),fd);
    fclose(fd);

}

void delete_account(char *username){
    FILE *fd_whitelist = fopen("/home/petru10/RC_PROJECT/workspace/FTP-TCP_RC2025/server_folder/whitelist.txt","rb");

    FILE *fd_temp = fopen("/home/petru10/RC_PROJECT/workspace/FTP-TCP_RC2025/server_folder/temp.txt","wb");

    int found = 0;

    struct account acc;

    while(fscanf(fd_whitelist, "%s %s", acc.username, acc.password) != EOF)
        if(strcmp(acc.username,username) != 0)
            fprintf(fd_temp, "%s %s\n", acc.username, acc.password);
        else
            found = 1;
  
    if(found)
        printf("\n The account with the username: %s was deleted",username);
    else
        printf("\n%s is not whitelisted\n", username);
    
    fclose(fd_whitelist);
    fclose(fd_temp);

    remove("/home/petru10/RC_PROJECT/workspace/FTP-TCP_RC2025/server_folder/whitelist.txt");
    rename("/home/petru10/RC_PROJECT/workspace/FTP-TCP_RC2025/server_folder/temp.txt","/home/petru10/RC_PROJECT/workspace/FTP-TCP_RC2025/server_folder/whitelist.txt");

}

void encrypt(char pass[100])
{
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
    printf("\n[server] Username: ");

    fgets(acc.username, sizeof(acc.username), stdin);
    acc.username[strcspn(acc.username, "\n")] = 0;

    printf("\n[server] Password: ");

    fgets(acc.password, sizeof(acc.password), stdin);
    acc.password[strcspn(acc.password, "\n")] = 0;

    strcpy(copy.username, acc.username);
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
    long file_size = ftell(fd_file); // pozitia cursorului
    rewind(fd_file);                 // reseteaza pozitita cursorului

    send(server_socket, &file_size, sizeof(file_size), 0);

    int bytes_read;
    char buffer[BUFFSIZE];

    // trimit
    while ((bytes_read = fread(buffer, 1, BUFFSIZE, fd_file)) > 0)
        if (send(server_socket, buffer, bytes_read, 0) < 0)
        {
            perror("Error sending bytes to server");
            fclose(fd_file);
            return;
        }

    // printf("\nData sent\n"); //debug

    // trimit END dupa ce am terminat
    const char *end_signal = "END";
    send(server_socket, end_signal, 3, 0);

    fclose(fd_file);
    // printf("File uploaded completed!\n"); //debug
}

void download(const char *name, int server_socket)
{
    FILE *fd = fopen(name, "wb");
    if (fd == NULL)
    {
        perror("Error creating the file");
        return;
    }

    long file_size;
    if (recv(server_socket, &file_size, sizeof(file_size), 0) <= 0)
    {
        perror("Error receiving file size");
        fclose(fd);
        return;
    }

    printf("[client] File size: %ld bytes.\n", file_size);

    long total_received = 0;
    char buffer[BUFFSIZE];
    int bytes_read;

    while (total_received < file_size)
    {
        bytes_read = recv(server_socket, buffer, BUFFSIZE, 0);
        if (bytes_read <= 0)
        {
            perror("Error receiving file data");
            fclose(fd);
            return;
        }

        long bytes_to_write = bytes_read;

        if (total_received + bytes_read > file_size)
        {
            bytes_to_write = file_size - total_received;
        }

        if (fwrite(buffer, 1, bytes_to_write, fd) != bytes_to_write)
        {
            perror("Error writing to file");
            fclose(fd);
            return;
        }

        total_received += bytes_to_write;
    }

    printf("[client] Download completed -- %ld/%ld bytes recieved)\n", total_received, file_size);
    fclose(fd);
}

int port;

int main(int argc, char *argv[])
{

    int server_socket;
    struct sockaddr_in server;
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
        char dir[BUFFSIZE]="/home/petru10/RC_PROJECT/workspace/FTP-TCP_RC2025";

        if(chdir(copy.username) !=0){
            perror("Can't change dir");
        }

        while (1)
        {
            char buffer[BUFFSIZE];
            printf("Enter command: ");
            memset(buffer, 0, BUFFSIZE);
            fgets(buffer, sizeof(buffer), stdin);

            buffer[strcspn(buffer, "\n")] = '\0';

            if(strcmp(copy.username,"admin")==0 && strncmp(buffer,"create_account",14)==0){
                create_account();
                printf("\n Account created\n");
                continue;
            }
            else if(strcmp(copy.username,"admin")==0 && strncmp(buffer,"delete_account",14)==0){
                char *command_line = strtok(buffer, " ");
                char username[BUFFSIZE];

                command_line = strtok(NULL, " ");

                if (command_line != NULL)
                    strcpy(username, command_line);
                else
                    perror("invalid command");

                delete_account(username);
                printf("\n Account deleted\n");

                continue;
            }
            else if (strncmp(buffer, "upload", 6) == 0)
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

                // locatie
                path = strtok(NULL, " ");
                if (path != NULL)
                    strcpy(location, path);
                else
                    printf("invalid command");
                

                // trimit la server "upload locatie"
                char info_server[BUFFSIZE] = "upload ";
                strcat(info_server, location);
                char ready[BUFFSIZE];
                send(server_socket, info_server, strlen(info_server), 0);
                
                if (strncmp(location, "server", 6) != 0 && strncmp(location, "client", 6) != 0 && strncmp(location, "whitelist", 9) != 0)
                {
                    // primesc ready
                    recv(server_socket, ready, sizeof(ready), 0);
                    upload(file_to_upload, location, server_socket);
                    // printf("Am iesit\n"); //debug
                }
                else
                {
                    printf("\n[client] You are not allowed to do that.\n");
                    continue;
                }
            }
            else if (strncmp(buffer, "download", 8) == 0)
            {
                char *path = strtok(buffer, " ");
                char file_to_download[BUFFSIZE];
                char new_file_name[BUFFSIZE];

                if (path != NULL)
                    strcpy(file_to_download, path);
                else
                    perror("invalid command");

                path = strtok(NULL, " ");

                if (path != NULL)
                    strcpy(file_to_download, path);
                else
                    perror("invalid command");

                path = strtok(NULL, " ");

                if (path != NULL)
                    strcpy(new_file_name, path);
                else
                    perror("invalid command");

                char command_argument[BUFFSIZE] = "download ";

                // printf("\n%s\n", file_to_download); //debug 
                
                strcat(command_argument, file_to_download);
                send(server_socket, command_argument, strlen(command_argument), 0);

                if (strncmp(file_to_download, "server", 6) != 0 && strncmp(file_to_download, "client", 6) != 0 && strncmp(file_to_download, "whitelist", 9) != 0)
                {

                    printf("\nDownloading..\n");
                    download(new_file_name, server_socket);
                    // printf("I'm out\n");//debug
                    send(server_socket, "finish", strlen("finish"), 0);
                }
                else
                {
                    printf("\n[client ]You are not allowed to do that.\n");

                    continue;
                }
            }

            else if (send(server_socket, buffer, strlen(buffer), 0) < 0)
            {
                perror(" Error sending the command");
                break;
            }

            int n = recv(server_socket, buffer, BUFFSIZE, 0);

            buffer[n] = '\0';
            printf("%s", buffer);
            printf("\n\n");
        }
    }

    close(server_socket);
    return 0;
}
