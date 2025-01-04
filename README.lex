
\documentclass{article}
\usepackage{graphicx} % Required for inserting images
\usepackage{verbatim}
\usepackage[margin=2.5cm, top=1cm]{geometry}

\title{Tema 2}
\author{Galateanu Petru-Ioan}
\date{Faculteatea de Informatica, Universitatea "Alexandru Ioan Cuza" din Iasi}

\begin{document}

\maketitle

\section{Introducere}

Proiectul "MyFileTransferProtocol" isi propune sa implementeze o aplicatie client/server care faciliteaza transferul de fisiere intr-un mod sigur si eficient. Aplicatia urmareste sa ofere autentificare securizata a utilizatorilor, gestionarea accesului prin mecanisme de autorizare (whitelist/blacklist) si manipularea directoarelor si a fisierelor de pe server. 

\section{Tehnologii aplicate}
In cadrul proiectului s-au utilizat urmatoarele tehnologii:\\ 
\begin{itemize}
\item \textbf{TCP (Transmission Control Protocol) } \\
Pentru transferul de fisiere si comenzi intre client si server. TCP a fost ales datorita fiabilitatii sale in livrarea datelor.Acesta asigura livrarea garantata a datelor, aspect esential intr-o aplicatie care transmite fisiere, deoarece pierderea sau coruperea unui pachet poate compromite intregul fisier. De asemenea, TCP mentine ordinea pachetelor iar datele trimise de client sunt primite in aceeasi ordine de catre server. \\
Totodata, stabileste o conexiune persistenta intre client si server printr-un proces de 3-way handshake. Acestea garanteaza ca cele 2 parti sunt pregatite pentru transfer inainte ca datele sa fie trimise. Acest model este ideal in cazul de fata, deoarece este necesara o comunicare constanta intre client si server pe toata durata operatiunilor.
\item \textbf{Mecansim de codificare al parolelor} \\
Pentru a avea o transmitere securizata a parolei la autentificare am implementat un mecanism de codificare. Aceasta consta in inlocuirea fiecarui caracter al parolei cu un alt caracter conform unui algoritm simplu de substitutie. \\ \\ Astfel, clientrul trimite catre server parola criptata, iar serverul o decodifica si verifica autentificarea folosindu-se de fisierul whitelist. 

Aceast mecanism este eficient deoarece protejeaza parola exact in punctul cel mai vulnerabil al fluxului - momentul in care aceasta circula prin retea. In timpul transferului, parola este codificata si astfel devine inutilizabila pentru un atacator care ar incerca sa o intercepteze.
\item \textbf{Whitelist/Blacklist}\\
Un mecanism implementat pentru autorizarea accesului utilizatorilor. Toate conturile sunt salvate intr-un fisier \textit{whitelist.txt} in format:\textit{ username parola}.
\end{itemize}

\section{Structura aplicatiei}
\includegraphics[width = 15cm, height = 10cm]{schema.drawio.png}
\\ 
Diagrama cuprinde componentele principale:
\begin{itemize}
    \item \textbf{Client: } introduce datele de conectare ale utilizatorului, trimite comenzi si transfera fisiere catre server.
    \item \textbf{Server: } 
    \begin{itemize}
        \item Verifica datele de conectare folosind fisierul \textit{whitelist.txt}
        \item Gestioneaza comenzile clientului (crearea directoarelor, redenumirea fisierelor, stergerea fisierelor).
        \item Primeste si salveaza fisierele trimise de client.
    \end{itemize}
\end{itemize}
\newpage
\section{Aspecte de implementare}
\begin{itemize}
    \item \textbf{Mecanism de codificare si decodificare a parolelor}\\ \\ 
    Algoritmul de codificare si decodificare utilizat pentru protectia parolelor este unul simpl, bazat pe substituirea fiecarui caracter conform unui tabel de mapare.

    \begin{verbatim}
// client.c    
void encrypt(char pass[100]){
    char a;
    int i=0;
    int n = strlen(pass);

    while(i<n){
        switch (pass[i])
        {
        case 'a': pass[i]='%'; break;
        case 'b': pass[i]='@'; break;
        case 'c': pass[i]=')'; break;
        ...
        }
    i++;
    }
}

// server.c
void decrypt(char pass[100]){
    char a;
    int i=0;
    int n = strlen(pass);

    while(i<n){
        switch (pass[i])
        {
        case '%': pass[i] ='a'; break; 
        case '@': pass[i] ='b'; break; 
        case ')': pass[i] ='c'; break;
        ...
        }
    i++;
    }
}
    \end{verbatim}
\newpage
\item \textbf{Fluxul de autenitificare:} \\
1. Clientul trimite numele de utilizator si parola codificata catre server. \\
2. Serverul decodifica parola primita si o compara cu valorile stocate in \textit{whitelist.txt}. \\
3. Daca combinatia \(username \; parola\) este valida, clientul este conectat si poate trimite comenzi.
\begin{verbatim}
1.
//client.c
void connect_account(int server_socket)
{
    struct account acc;
    char c;
    int i=0;
    printf("\n[server] Username: ");
    fgets(acc.username, sizeof(acc.username), stdin);
    acc.username[strcspn(acc.username, "\n")] = 0;

    printf("\n[server] Password: ");
    
    fgets(acc.password, sizeof(acc.password), stdin);
    acc.password[strcspn(acc.password, "\n")] = 0;
    encrypt(acc.password);
    send(server_socket, acc, sizeof(acc), 0);
};
2.
//server.c
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

    while (fscanf(file, "%s %s", file_username, file_password)!=EOF)
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

void client_handler(int client_socket, int id)
{
...
if (n < 0)
{
    perror("[server] Error recieving data from client");
    close(client_socket);
    return;
}
decrypt(acc.password);

if (account_verify(acc.username, acc.password)){
    send(client_socket, "[server] Account found! Connected!", strlen("[server] Account found! Connected!"), 0);
    printf("[server] Account with ID: %d logged\n", id);
    connected = 1;
}
...
}

3.
//client.c
int main(int argc, char* argv[]){
...
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
        char buffer[BUFFSIZE];
        printf("Enter command: ");
        fgets(buffer, sizeof(buffer), stdin);
        buff[strcspn(buffer, "\n")] = 0;

        if(send(server_socket, buffer, strlen(buffer), 0) < 0){
            perror("[client] Error sending the command");
            break;
        }

        int n = recv(server_socket, buffer, BUFFSIZE, 0);

        buffer[n] = '\0';
        printf("%s", buffer);
        printf("\n\n");    
    }
}
...
}


\end{verbatim}
\includegraphics[width=8cm, height=3.4cm]{ss_server.jpeg}
\includegraphics[width=8cm, height=3.4cm]{ss_client.jpeg}
\item \textbf{Operatiuni gestionate de server}\\
Serverul gestioneaza mai multe tipuri de comenzi trimise de client:
\begin{itemize}
    \item \textbf{Creare directoare(mkdir $nume$):} \\
    Creeaza un director denumit $nume$.

    \begin{verbatim}
if(strncmp(command, "mkdir", 5) == 0){
    char directory_name[BUFF_SIZE];
    sscanf(buff+6, "%s", directory_name);

    if(mkdir(directory_name, 0777) == 0)
        send(client_socket, "[server]Directory created successfully!", 
        strlen("[server]Directory %s created successfully!"), 0);
    else
        send(client_socket, "[server]Failed to create directory",
        strlen("[server]Failed to create directory"),0);
}
    \end{verbatim}
    \item \textbf{Redenumire fisier(rename $nume$ $nume1$):}\\
    Redenumeste fisierul $nume$ in $nume1$.
    \begin{verbatim}
if(strncmp(command, "rename", 6) == 0){
    char old_name[BUFF_SIZE],new_name[BUFF_SIZE];
    sscanf(buff+7, "%s %s", old_name, new_name);

    if(rename(old_name, new_name) == 0)
        send(client_socket, "[server]File renamed successfully",
        strlen("[server]File renamed successfully"), 0);               
    else
        send(client_socket, "[server]Failed to rename the specified file",
        strlen("[server]Failed to rename the specified file"), 0);                
}        
    \end{verbatim}
    \item \textbf{Stergere fisier(delete $nume$):} \\
    Elimina fisierul $nume$ daca acesta nu este critic pentru aplicatie.
    \begin{verbatim}
if(strncmp(command, "delete", 6) == 0){
    char file_to_delete[BUFF_SIZE];
    sscanf(buff+7, "%s", file_to_delete);

    if(strncmp(file_to_delete,"server",6)!=0 && strncmp(file_to_delete,
    "client",6)!=0 && strncmp(file_to_delete,"whitelist",9)!=0)
        if(remove(file_to_delete) == 0){
            send(client_socket, "File deleted!", strlen("File deleted!"), 0);
            }
        else{
            send(client_socket, "Can't delete the file", 
            strlen("Can't delete the file"), 0);
            }
    else 
        send(client_socket, "Not allowed", strlen("Not allowed"), 0);
}        
    \end{verbatim}
    \newpage
    \item \textbf{Comanda help:} \\
    Ofera utilizatorului informatii referitoare la operatiile pe care le poate efectua.
    \begin{verbatim}
if(strncmp(command, "help", 4)==0){
    const char* help = 
        "\n[server] Available command list:\n"
        " > mkdir <dir_name> : creates a new directory named as specified.\n"
        " > rename <file_to_be_renamed> <new_name> : changes the name 
        of the <file_to_be_renamed> to <new_name>\n"
        " > delete <file> : removes the file\n";

    send(client_socket, help, strlen(help), 0);
}
    \end{verbatim}
\end{itemize}
\section{Concluzii}
Potentialele imbunatatiri ce pot fi aduse solutiei propuse ar fi:
\begin{itemize}
    \item \textbf{Sistem de logare si monitorizare}
    \\ Presupune inregistrarea tuturor operatiunilor si incercarilor de autentificare intr-un fisier (log) pentru detectarea atacurilor sau a abuzurilor.
    \item \textbf{Trensfer bidirectional de fisiere}
    \\ Implementarea dunctionalitatii de descarcare a fisierelor (download) de pe server catre client.
    \item \textbf{Premisiuni pentru utilizatori}
    \\ Introducerea unui sistem de permisiuni care sa defineasca cine poate crea, sterge sau redenumi un fisier.
    \item \textbf{Functionalitati pentru gestionarea utilizatorilor}
    \\ Adaugarea posibilitatii de inregistrare a utilizatorilor noi si gestionarea conturilor (schimbarea username-ului, resetarea parolei, etc.)
    \item \textbf{Interfata grafica}
    \\ Dezvoltarea unei interfete grafice pentru utilizatori, care sa simplifice utilizarea aplicatiei, eliminand necesitatea introducerii manuale a comenzilor.
\end{itemize}
\section{Referinte bibliografice}
\begin{enumerate}
    \item Geeks for Geeks. \textit{Differences between TCP and UDP}. https://www.geeksforgeeks.org/differences-between-tcp-and-udp/
    \item C++ Documentation. \textit{strcspn}. https://cplusplus.com/reference/cstring/strcspn/
    \item Geeks for Geeks. \textit{memset}. https://www.geeksforgeeks.org/memset-in-cpp/
    \item Diagram. \textit{Online Diagram Software}. https://app.diagrams.net/
    \item Visual Studio Code. \textit{Terminal Screenshots}. https://code.visualstudio.com/
    \item Linux Documentation. \textit{socket(7)}. https://man7.org/linux/man-pages/man7/socket.7.html
    \item C++ Documentation. \textit{strncmp}. https://cplusplus.com/reference/cstring/strncmp/
    \item Overleaf. \textit{online LaTeX editor}. https://www.overleaf.com/
    \item Geeks for Geeks. \textit{Socket Programming in C}. https://www.geeksforgeeks.org/socket-programming-cc/
    \item Linux Documentation. \textit{bind(2)}. https://man7.org/linux/man-pages/man2/bind.2.html
    \item Geeks for Geeks. \textit{TCP 3-Way Handshake Process}. https://www.geeksforgeeks.org/tcp-3-way-handshake-process/
    \item Geeks for Geeks. \textit{Create Directory or Folder with C/C++ Program}. https://www.geeksforgeeks.org/create-directoryfolder-cc-program/
    \item Geeks for Geeks. \textit{rename function in C}. https://www.geeksforgeeks.org/rename-function-in-ccpp/
    \item Geeks for Geeks. \textit{C program to delete a file}. https://www.geeksforgeeks.org/c-program-delete-file/
    \item Geeks fo Geeks. \textit{Switch Statement in C}. https://www.geeksforgeeks.org/c-switch-statement/
    \item C++ Documentation. \textit{sscanf}. https://cplusplus.com/reference/cstdio/sscanf/
    
\end{enumerate}
\end{itemize}
\end{document}
