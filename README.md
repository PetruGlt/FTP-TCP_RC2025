# MyFileTransfer
**Autor:** Galateanu Petru-Ioan  
**Facultatea de Informatica, Universitatea "Alexandru Ioan Cuza" din Iasi**  

## Abstract
Această documentație descrie implementarea unui server FTP scris în limbajul C utilizând protocolul TCP. Proiectul include autentificare utilizatori, transfer de fișiere și alte funcționalități de bază ale unui server FTP.

## Cuprins
- [Introducere](#Introducere)
- [Tehnologii aplicate](#Tehnologii-aplicate)
- [Structura aplicatiei](#Structura-aplicatiei)
- [Aspecte de implementare](#Aspecte-de-implementare)
- [Concluzii](#Concluzii)

## Introducere

- Proiectul de față își propune să implementeze un server FTP simplu, care utilizează protocolul TCP pentru a facilita transferul fișierelor între client și server într-un mod eficient și securizat. FTP (File Transfer Protocol) este un protocol standard de rețea utilizat pentru transferul fișierelor între un client și un server, fiind folosit frecvent în aplicațiile de administrare a serverelor și în procesul de partajare a fișierelor.

- Scopul principal al acestui proiect este de a crea o aplicație client-server bazată pe tehnologiile de programare C și socket programming, care va permite utilizatorilor să se autentifice pe server, să încarce și să descarce fișiere, și să gestioneze directoare.

- Proiectul va include funcționalități de bază, precum autentificarea utilizatorilor pe baza unui fișier de tip whitelist, manipularea fișierelor de pe server (upload, download, ștergere, redenumire), precum și protecția împotriva accesului neautorizat.

## Tehnologii aplicate

### Networking (TCP/IP)
- **Sistem de socket-uri:**
  - Funcțiile `socket()`, `bind()`, `listen()`, `accept()` sunt utilizate pentru a crea și gestiona conexiunile de rețea.
  - Funcțiile `send()` și `recv()` trimit și primesc date între server și client.

- **Protocol TCP:** Serverul utilizează protocolul TCP, care asigură comunicare fiabilă, orientată pe conexiune.

### Procesare concurentă
- **Crearea proceselor cu `fork()`:**
  - Serverul folosește funcția `fork()` pentru a crea un proces nou pentru fiecare client.

- **Managementul proceselor:** Procesul părinte continuă să accepte conexiuni, iar procesul copil gestionează fiecare client.

### Manipulare fișiere
- **I/O pentru fișiere:** Serverul implementează funcții pentru:
  - *Upload:* Funcția `recieve_file()` salvează fișiere primite de la client.
  - *Download:* Funcția `respond_to_download()` trimite fișiere clientului.

- **Funcții POSIX:** Utilizarea funcțiilor precum `fopen()`, `fwrite()`, `fread()`, `remove()` și `rename()` pentru manipularea fișierelor.
- **Directorul curent:** Comenzi precum `chdir()` și `getcwd()` sunt utilizate pentru a schimba și a afișa directorul curent.

### Securitate de bază
- **White-listing pentru autentificare:**
  - Serverul verifică dacă un utilizator este autorizat comparând datele din fișierul `whitelist.txt`.
  - Parolele sunt criptate și decriptate cu o funcție manuală (`decrypt()`).

- **Restricții:** Nu permite manipularea fișierelor critice (`server`, `client`, `whitelist`).

### Manipularea directoarelor
- **Crearea directoarelor:** Funcția `mkdir()` creează noi directoare.
- **Listarea fișierelor:** Utilizarea `opendir()` și `readdir()` pentru a afișa fișierele dintr-un director.

### Structuri de date și criptare custom
- **Structura `account`:** Stochează informații despre utilizator (nume și parolă).
- **Decriptare custom:** Algoritm simplu care mapează caractere criptate la cele originale.

### Managementul erorilor
- Funcția `perror()` este utilizată pentru afișarea mesajelor de eroare.

### Comunicare între server și client
- Implementarea comenzilor `mkdir`, `rename`, `delete`, `list`, `upload`, `download`, `help`.

### Proiectare modulară
- Funcțiile sunt separate pentru a gestiona fiecare funcționalitate (`client_handler()`, `recieve_file()`, `account_verify()` etc.).

### Utilizarea bibliotecilor POSIX
- Biblioteci precum `sys/socket.h`, `netinet/in.h`, `unistd.h`, `dirent.h` sunt utilizate pentru rețea, procese și manipularea directoarelor.

## Structura aplicației

### Diagrama

![Diagrama](https://github.com/PetruGlt/FTP-TCP_RC2025/blob/main/diagramaproiect.drawio.png)

### Prezentarea componentelor principale ale diagramei

- **Client:**
  - Primește de la tastatură datele introduse de utilizator (`username`, `parola`) și le criptează.
  - Trimite către server datele criptate.
  - Dacă primește de la server mesajul specific pentru o conectare reușită, accesează folderul cu numele `username` și primește comenzi de la utilizator.
  - Trimite serverului comenzile.

- **Server:**
  - Verifică datele de conectare folosind fișierul `whitelist.txt`.
  - Dacă datele de conectare sunt corecte, creează un folder cu numele utilizatorului care s-a conectat.
  - Primește comenzi de la client.
  - Gestionează comenzile clientului.

## Aspecte de implementare

### Arhitectura generală
Aplicația are o arhitectură client-server, utilizând protocolul TCP/IP.

### Autentificare
- Serverul decriptează parola primită de la client și validează utilizatorii folosind un fișier `whitelist.txt` care conține perechi de forma: `username parola`.

- Administratorul serverului este singurul utilizator care după înregistrare poate crea sau șterge utilizatori. Conturile pe care acesta le creează sunt salvate în `whitelist.txt`.

#### Fluxul de autentificare:
1. Clientul trimite numele de utilizator și parola codificată către server.
2. Serverul decodifică parola primită și o compară cu valorile stocate în `whitelist.txt`.
3. Dacă combinația `username parola` este validă, clientul este conectat și poate trimite comenzi.

## Concluzii

### Final
- În cadrul acestui proiect, am implementat un server FTP simplu, utilizând protocolul TCP, pentru a demonstra aplicarea principiilor fundamentale ale rețelelor de calculatoare, a protocoalelor de comunicație și a tehnologiilor de securitate.

### Potențiale îmbunătățiri
- **Sistem de logare și monitorizare:** Presupune înregistrarea tuturor operațiunilor efectuate pe server pentru o mai bună monitorizare și depanare.
