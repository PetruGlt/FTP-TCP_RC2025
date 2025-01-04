#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <dirent.h>

#define PORT 2023
#define BUFF_SIZE 1024

extern int errno;

struct account
{
    char username[100];
    char password[100];
};

void recieve_file(const char *name, int client_socket)
{
    FILE *fd = fopen(name, "wb");
    if (fd == NULL)
    {
        perror("[server] Error creating the file");
        return;
    }

    // dimensiunea fisierului 
    long file_size;
    recv(client_socket, &file_size, sizeof(file_size), 0);

    long total_received = 0;

    int bytes_read;
    char buffer[BUFF_SIZE];
    int end_marker_found = 0;
    // int counter = 0;  //debug

    while (total_received < file_size && (bytes_read = recv(client_socket, buffer, BUFF_SIZE, 0)) > 0)
    {
        total_received += bytes_read;

        char *end_marker = strstr(buffer, "END");
        
        if (end_marker)
        {
            size_t data_size = end_marker - buffer;
            if (fwrite(buffer, 1, data_size, fd) != data_size)
            {
                perror("[server] Error writing to file");
                fclose(fd);
                return;
            }

            end_marker_found = 1;
            break;
        }
        else
        {
            // printf("%d\n",counter++);  //debug
            if (fwrite(buffer, 1, bytes_read, fd) != bytes_read)
            {
                perror("[server] Error writing to file");
                fclose(fd);
                return;
            }
        }
    }

    printf("[server] File received\n");
    fclose(fd);

}

void decrypt(char pass[100])
{
    char a;
    int i = 0;
    int n = strlen(pass);

    while (i < n)
    {
        switch (pass[i])
        {
        case '%':
            pass[i] = 'a';
            break;
        case '@':
            pass[i] = 'b';
            break;
        case ')':
            pass[i] = 'c';
            break;
        case '&':
            pass[i] = 'd';
            break;
        case '$':
            pass[i] = 'e';
            break;
        case '#':
            pass[i] = 'f';
            break;
        case ';':
            pass[i] = 'g';
            break;
        case '1':
            pass[i] = 'h';
            break;
        case 'p':
            pass[i] = 'i';
            break;
        case '^':
            pass[i] = 'j';
            break;
        case 'y':
            pass[i] = 'k';
            break;
        case '-':
            pass[i] = 'l';
            break;
        case '}':
            pass[i] = 'm';
            break;
        case ':':
            pass[i] = 'n';
            break;
        case '4':
            pass[i] = 'o';
            break;
        case '+':
            pass[i] = 'p';
            break;
        case '`':
            pass[i] = 'q';
            break;
        case 's':
            pass[i] = 'r';
            break;
        case '>':
            pass[i] = 's';
            break;
        case '<':
            pass[i] = 't';
            break;
        case '~':
            pass[i] = 'u';
            break;
        case 'r':
            pass[i] = 'v';
            break;
        case 'a':
            pass[i] = 'w';
            break;
        case 'Y':
            pass[i] = 'x';
            break;
        case '7':
            pass[i] = 'y';
            break;
        case '"':
            pass[i] = 'z';
            break;

        case 'R':
            pass[i] = '1';
            break;
        case 'V':
            pass[i] = '2';
            break;
        case 'Q':
            pass[i] = '3';
            break;
        case 'T':
            pass[i] = '4';
            break;
        case 'Z':
            pass[i] = '5';
            break;
        case 'W':
            pass[i] = '6';
            break;
        case 'S':
            pass[i] = '7';
            break;
        case '.':
            pass[i] = '8';
            break;
        case 'i':
            pass[i] = '9';
            break;
        case 'D':
            pass[i] = '0';
            break;

        case '_':
            pass[i] = '!';
            break;
        case '!':
            pass[i] = '@';
            break;
        case 'C':
            pass[i] = '.';
            break;
        default:
            break;
        }
        i++;
    }
}

int account_verify(const char *username, const char *password)
{
    FILE *file = fopen("whitelist.txt", "r");
    if (file == NULL)
    {
        perror("[server] Error opening whitelist");
        return 0;
    }

    char file_username[50];
    char file_password[50];

    while (fscanf(file, "%s %s", file_username, file_password) != EOF)
    {
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0)
        {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

//respond_to_download(current_directory, file_to_download, client_socket);

void respond_to_download(const char *current_dir, const char *file_to_download, int client_socket){
    FILE *fd = fopen(file_to_download, "rb");
    if (fd == NULL)
    {
        perror("[server] Error opening the file");
        return;
    }

    fseek(fd, 0, SEEK_END);
    long file_size = ftell(fd);
    rewind(fd);

    send(client_socket, &file_size, sizeof(file_size), 0);

    int bytes_read;
    char buffer[BUFF_SIZE];

    while ((bytes_read = fread(buffer, 1, BUFF_SIZE, fd)) > 0)
    {
        if(send(client_socket, buffer, bytes_read, 0) < 0){
            perror("[server] Error sending to client");
            fclose(fd);
            return;
        }
    }

     //printf("\nData send from server to client.\n"); //debug
    const char *end_signal = "END\n"; // Adăugați o linie nouă sau terminator
    send(client_socket, end_signal, 3, 0);
    fclose(fd);
    fflush(stdout);
    printf("[server] File sent succesfully");

    // send(client_socket, "[server] File downloaded", sizeof("[server] File downloaded"), 0);
}

void client_handler(int client_socket, int id)
{
    struct account acc;
    int connected = 0;
    char buff[BUFF_SIZE];

    char *current_directory = "/home/petru10/RC_PROJECT/workspace/FTP-TCP_RC2025/server_folder"; // LOCATIA SERVERULUI

    // printf("%s\n", current_directory); // verificare director initial

    if (recv(client_socket, &acc, sizeof(acc), 0) <= 0)
    {
        perror("[server] Error recieving account info from client");
        close(client_socket);
        return;
    }

    decrypt(acc.password);
    
    if (account_verify(acc.username, acc.password))
    {
        send(client_socket, "[server] Account found! Connected!", strlen("[server] Account found! Connected!"), 0);
        printf("[server] Account with ID: %d logged in\n", id);
        connected = 1;
    }
    else
        send(client_socket, "[server] Failed to connect! Account not found!", strlen("[server] Failed to connect! Account not found!"), 0);
    
    while (connected)
    {
        memset(buff, 0, BUFF_SIZE);
        int bytes_recieved = recv(client_socket, buff, BUFF_SIZE - 1, 0);
        buff[bytes_recieved] = '\0';

        char command[BUFF_SIZE];
        strcpy(command, buff);

        if (strncmp(buff, "exit", 4) == 0)
        {
            send(client_socket, "[server] Goodbye!\n", strlen("[server] Goodbye!\n"), 0);
            close(client_socket);
            connected = 0;
        }

        else if (strncmp(command, "mkdir", 5) == 0)
        {
            char directory_name[BUFF_SIZE];
            sscanf(buff + 6, "%s", directory_name);

            if (mkdir(directory_name, 0777) == 0)
                send(client_socket, "[server]Directory created successfully!", strlen("[server]Directory %s created successfully!"), 0);
            else
                send(client_socket, "[server]Failed to create directory", strlen("[server]Failed to create directory"), 0);
        }

        else if (strncmp(command, "rename", 6) == 0)
        {
            char old_name[BUFF_SIZE], new_name[BUFF_SIZE];
            sscanf(buff + 7, "%s %s", old_name, new_name);

            if (rename(old_name, new_name) == 0)
                send(client_socket, "[server]File renamed successfully", strlen("[server]File renamed successfully"), 0);
            else
                send(client_socket, "[server]Failed to rename the specified file", strlen("[server]Failed to rename the specified file"), 0);
        }
        else if (strncmp(command, "delete", 6) == 0)
        {
            char file_to_delete[BUFF_SIZE];
            sscanf(buff + 7, "%s", file_to_delete);

            if (strncmp(file_to_delete, "server", 6) != 0 && strncmp(file_to_delete, "client", 6) != 0 && strncmp(file_to_delete, "whitelist", 9) != 0)
                if (remove(file_to_delete) == 0)
                    send(client_socket, "File deleted!", strlen("File deleted!"), 0);
                else
                    send(client_socket, "Can't delete the file", strlen("Can't delete the file"), 0);
            else
                send(client_socket, "Termina", strlen("Termina"), 0);
        }
        else if (strncmp(command, "help", 4) == 0)
        {
            const char *help =
                "\n[server] Available command list:\n"
                "\n"
                " > currentdir :\n"
                "    - Displays the path to the current directory.\n"
                " > list :\n"
                "    - Similar to 'ls' command, list the files in the current directory.\n" 
                " > cd <directory_name> :\n"
                "    - Changes the current directory to <directory_name> directory.\n"
                " > mkdir <dir_name> :\n"
                "    - Creates a new directory named as specified.\n"
                " > rename <file_to_be_renamed> <new_name> :\n"
                "    - Changes the name of the <file_to_be_renamed> to <new_name>\n"
                " > delete <file> :\n"
                "    - Removes the specified file\n"
                " > upload <path> <file_name> :\n"
                "    - Uploads in the current working directory, the file <path> and saves it as <file_name>.\n"
                "    - The current working directory is the one returned by 'currentdir' command.\n"
                " > download <file_name> :\n"
                "    - Downloads the file specified by <file_name> from the server.\n";

            send(client_socket, help, strlen(help), 0);
        }

        else if (strncmp(command, "upload", 6) == 0)
        {
            char name[BUFF_SIZE];
            sscanf(command + 7, "%s", name);

            send(client_socket, "[server] Ready to receive", strlen("[server] Ready to receive"), 0);
            //printf("inainte de recieve_file\n");
            recieve_file(name, client_socket);
            //printf("dupa recieve_file\n");
            send(client_socket, "[server] File received", strlen("[server] File received"), 0);
        }
        else if (strncmp(command, "cd", 2) == 0)
        {
            char new_location[BUFF_SIZE];
            sscanf(buff + 3, "%s", new_location);
            int ok = 0; // daca argumentul nu este ..
            if (strncmp(new_location, "..", 2) == 0)
            {
                ok = 1;
                if (chdir("..") == 0)
                {
                    char cwd[1024];
                    if (getcwd(cwd, sizeof(cwd)) != NULL)
                        send(client_socket, cwd, 100, 0);
                }
            }

            if (ok == 0) // daca argumentul nu este .., atunci inseamna ca e posibil sa avem un path
            {
                if (chdir(new_location) == 0)
                {
                    // debug printf("am intrat in if\n");
                    send(client_socket, "[server] Directory changed", sizeof("[server] Directory changed"), 0);
                }
                else
                {
                    // debug printf("Nu am intrat in if\n");
                    send(client_socket, "[server] Cannot change directory", sizeof("[server] Cannot change directory"), 0);
                }
                ok = 0; // refresh
            }
        }
        else if (strncmp(command, "currentdir", 11) == 0)
        {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                send(client_socket, cwd, 100, 0);
        }
        else if (strncmp(command, "list", 5) == 0)
        {
            char output[BUFF_SIZE];
            struct dirent *entry;
            DIR *dir = opendir(".");
            memset(output, 0, BUFF_SIZE);

            while ((entry = readdir(dir)) != NULL)
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) // excludem intrarile implicite ce reprezinta directorul curent si cel parinte
                    continue;

                strcat(output, entry->d_name);
                strcat(output, "\n");
            }
            closedir(dir);
            send(client_socket, output, sizeof(output), 0);
            
        }
        else if(strncmp(command, "download", 8) == 0){
            char file_to_download[BUFF_SIZE];
            sscanf(buff + 9, "%s", file_to_download);
            // printf("%s\n", file_to_download);
            send(client_socket, "ready", sizeof("ready"),0);

            respond_to_download(current_directory, file_to_download, client_socket);

            send(client_socket, "Download finished", sizeof("Download finished"),0);
            
        }
        else
            send(client_socket, "[server]Unknown command", strlen("[server]Unknown command"), 0);
    }
    close(client_socket);
}

int main()
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    int server_socket;
    int client_socket;
    socklen_t client_length = sizeof(client);
    int connected = 0;
    int id = 0;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("[server]Error command socket().\n");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Error command bind().\n");
        return errno;
    }

    if (listen(server_socket, 5) == -1)
    {
        perror("[server]Error command listen().\n");
        return errno;
    }

    while (1)
    {
        printf("[server] Listening on port %d... \n", PORT);
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(server_socket, &read_fds);

        client_socket = accept(server_socket, (struct sockaddr *)&client, &client_length);

        if (client_socket < 0)
        {
            perror("[server] accept() failed.\n");
            continue;
        }

        id++;

        printf("[server] Client reached! Recieved ID: %d\n", id);

        int pid = fork();
        if (pid < 0)
        {
            perror("[server] fork() failed");
            close(client_socket);
            continue;
        }

        if (pid == 0)
        {

            close(server_socket);
            client_handler(client_socket, id);
            exit(0);
        }
        else
        {
            close(client_socket);
        }
    }

    close(server_socket);
    return 0;
}
