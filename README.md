MyFileTransfer
Autor

Galateanu Petru-Ioan
Facultatea de Informatică, Universitatea "Alexandru Ioan Cuza" din Iași
Descriere

Această documentație descrie implementarea unui server FTP scris în limbajul C utilizând protocolul TCP. Proiectul include autentificare utilizatori, transfer de fișiere și alte funcționalități de bază ale unui server FTP.
Cuprins

    Introducere
    Tehnologii aplicate
    Structura aplicației
    Aspecte de implementare
    Concluzii

Introducere

Proiectul își propune să implementeze un server FTP simplu care utilizează protocolul TCP pentru transferul fișierelor între client și server într-un mod eficient și securizat. Serverul va permite autentificarea utilizatorilor, manipularea fișierelor și gestionarea directoarelor.
Tehnologii aplicate
Networking (TCP/IP)

    Sistem de socket-uri:
        Funcțiile socket(), bind(), listen(), accept() sunt utilizate pentru a crea și gestiona conexiunile de rețea.
        Funcțiile send() și recv() trimit și primesc date între server și client.
    Protocol TCP: Serverul utilizează protocolul TCP, care asigură o comunicare fiabilă și orientată pe conexiune.

Procesare concurentă

    Crearea proceselor cu fork(): Serverul folosește funcția fork() pentru a crea un proces nou pentru fiecare client.

Manipulare fișiere

    Funcțiile fopen(), fwrite(), fread(), remove() și rename() sunt utilizate pentru manipularea fișierelor.

Securitate de bază

    White-listing pentru autentificare: Serverul validează utilizatorii pe baza unui fișier whitelist.txt, iar parolele sunt criptate și decriptate cu o funcție manuală.

Manipularea directoarelor

    Funcțiile mkdir(), chdir() și getcwd() sunt folosite pentru manipularea și navigarea între directoare.

Structuri de date și criptare custom

    Utilizarea unei structuri account pentru stocarea informațiilor despre utilizatori și criptarea parolelor folosind un algoritm simplu de substituție.

Structura aplicației
Diagrama

Diagrama proiectului
Prezentarea componentelor principale ale diagramei

    Client: Trimite datele de autentificare criptate și primește răspunsurile de la server.
    Server: Verifică datele de autentificare și gestionează comenzile trimise de client.

Aspecte de implementare
Arhitectura generală

Aplicația are o arhitectură client-server, utilizând protocolul TCP/IP.
Autentificare

Serverul decriptează parola primită de la client și validează utilizatorii folosind fișierul whitelist.txt.
Operațiuni gestionate de server

    Comanda help (help): Oferă informații utilizatorului referitor la comenzile disponibile.
    Operații pe directoare:
        mkdir <nume>: Creează un director denumit nume.
        cd <dir>: Schimbă directorul curent.
        list: Afișează fișierele din directorul curent.
    Operații pe fișiere:
        rename <nume1> <nume2>: Redenumirea unui fișier.
        delete <nume>: Șterge un fișier.
        upload <path> <nume>: Încarcă un fișier pe server.
        download <fisier>: Descarcă un fișier de pe server.

Concluzii
Final

În cadrul acestui proiect, am implementat un server FTP simplu, utilizând protocolul TCP, pentru a demonstra aplicarea principiilor fundamentale ale rețelelor de calculatoare, a protocoalelor de comunicație și a tehnologiilor de securitate.
Potențiale îmbunătățiri

    Implementarea unui sistem de logare și monitorizare, care să înregistreze toate operațiunile și încercările de autentificare într-un fișier log pentru detectarea potențialelor atacuri.
