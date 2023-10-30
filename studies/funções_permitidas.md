
## system calls
- fazer os checks de erro com relação às chamadas!!
Aproximadamente na ordem que devem ser chamadas:

### getaddrinfo(), freeaddrinfo(), gai_strerror()
- prepara as estruturas que vamos precisar (não faz a conexão de fato)
- devolve um ponteiro para uma linked-list, res, de resultados

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,     // the host name to connect to e.g. "www.example.com" or IP
                const char *service,  // e.g. "http" or port number - ftp, telnete, smtp...
                const struct addrinfo *hints,
                struct addrinfo **res);

// hints example:
struct addrinfo hints;

memset(&hints, 0, sizeof hints); // make sure the struct is empty
hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
hints.ai_flags = AI_PASSIVE;     // fill in my IP for me


// getaddrinfo() use example:
int status;
struct addrinfo hints;
struct addrinfo *servinfo;

// listen on your host’s IP address, port 3490; doesn't do any listening or network setup
if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
}

// servinfo now points to a linked list of 1 or more struct addrinfos

// ... do everything until you don't need servinfo anymore ....

freeaddrinfo(servinfo); // free the linked-list


// other example: you’re a client who wants to connect to a particular server, “www.example.net” on port 3490
// here we don´t define hints.ai_flags:
memset(&hints, 0, sizeof hints); // make sure the struct is empty
hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
hints.ai_socktype = SOCK_STREAM;
status = getaddrinfo("www.example.net", "3490", &hints, &servinfo);
```

Exemplo de uso: `studies/showip.c`

### socket(), getprotobyname()

- Retorna um socket file descriptor para usar em outras funções, ou -1 no caso de erro. Seta errno para o valor do erro

```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```
- domain: `PF_INET` ou `PF_INET6` (IPv4 or IPv6)
- type: `SOCK_STREAM` ou `SOCK_DGRAM`
- protocol: se 0, ele escolhe o protocolo adequado para o tipo (TCP or UDP)
- podemos usar `getprotobyname()` para pegar o protocolo que quiser, “tcp” ou “udp”

```c
int s;

struct addrinfo hints, *res;
// fill out the "hints" struct as in the examples before

getaddrinfo("www.example.com", "http", &hints, &res);
// you should do error-checking on getaddrinfo(), and walk the "res" linked
// and also list looking for valid entries instead of just assuming the first one is good

s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```

### bind(), setsockopt()
- associa o socket com uma porta na máquina hospedeira (fazer isso se for usar `listen()` para verificar novas conexões em uma porta específica. Se for apenas fazer uma conexão usando `connect()` (porque você é um cliente, não um servidor), isso provavelmente é desnecessário
- se erro retorna -1 e seta o errno
```c
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
```

- sockfd: o socket file descriptor returnado por `socket()`
- my_addr: um pointero para uma `struct sockaddr`, que contém as informações sobre endereço (porta e IP)
- addrlen: o tamanho em bytes desse endereço

```c
struct addrinfo hints, *res;
int sockfd;

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE;     // fill in my IP for me - bind to the IP of the host it’s running on.
// If you want to bind to a specific local IP address, drop the AI_PASSIVE and put an IP address in for the first argument to getaddrinfo()
getaddrinfo(NULL, "3490", &hints, &res);

// make a socket:
sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// bind it to the port we passed in to getaddrinfo():
bind(sockfd, res->ai_addr, res->ai_addrlen);
```

**Todas as portas abaixo da 1024 são reservadas (a não ser que você seja um superuser). Podemos usar qualquer porta acima dessa, até 65535 (desde que não estejam sendo usadas por outro programa).**

- error on rerunning a server: "Address already in use" - significa que um socket que estava conectado ainda está segurando a porta no kernel e, portando, monopolizando-a. Para poder reutilizar a porta, podemos usar `setsockopt()`:
```c
int yes=1;

// lose the pesky "Address already in use" error message
if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
    perror("setsockopt");
    exit(1);
}
```

### conect()

```c
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
```

- sockfd: socket file descriptor retornado por `socket()`
- serv_addr: uma `struct sockaddr` contendo o IP a ser conectado e a porta em que se vai conectar
- addrlen: o tamanho em bytes da struct de endereço do servidor
- Todas essas informações podem ser obtidas com uma chamada `getaddrinfo()`
- retorna -1 se erro e seta a `errno`

Exemplo de uma conexão por socket para "www.example.com", na porta 3490:
```c
struct addrinfo hints, *res;
int sockfd;

// load up address structs with getaddrinfo():
memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
getaddrinfo("www.example.com", "3490", &hints, &res);

// make a socket:
sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// connect!
connect(sockfd, res->ai_addr, res->ai_addrlen);
```

### listen(), accept()
- espera por conexões que chegam (`liste()`) e lida com elas (`acept()`)
- ambas as funções retornam -1 em caso de erro e setam a `errno`

```c
int listen(int sockfd, int backlog);
```

- sockfd: o socket file descriptor retornado por `socket()`
- backlog: o número de conexões permitidas na fila de espera na entrada. Novas conexões vão esperar nessa fila até que se use `accept()`. A maioria dos sistemas limita esse número silenciosamente para em torno de 20.

```c
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
- retorna um novo socket file descriptor para ser usado em uma conexão única. O socket original continua escutando  por novas conexões e este novo pode ser utilizado para enviar e receber informações (com `send()` e `recv()`)
- sockfd: o socket descriptor que está escutando.
- addr: um ponteiro para uma struct sockaddr_storage com a informação sobre a conxão.
- addrlen: sizeof(struct sockaddr_storage) antes de ser passada para `accept()`. `accept()` não colocará mais bytes do que esses; se colocar menos, ela irá alterar o valor de `addrlen` para refletí-lo.


- Se for lidar com apenas uma conexão pode-se usar `close()` para fechar sockfd para prevenir que novas conexões aconteçam.
- Geralmente, o servidor lida com múltiplas conexões utilizando `fork()`. Ele espera por uma conexão, aceita-a, e então cria um processo filho para lidar com elagit stash

### send(), recv()
- para comunicação em stream sockets ou datagram sockets conectados

```c
int send(int sockfd, const void *msg, int len, int flags);
```
- sockfd: o socket descriptor para qual você quer enviar dados (pode ser o retornado por `socket()` ou o por `accept()`).
- msg: ponteiro para o dado retornado
- len: o tamanho dos dados em bytes.
- flags: setar para 0. (olhar o man de send())
- retorna o número de bytes realmente enviado. Pode ser que ele não conseiga enviar todo o dado de uma vez. Se o valor retornado for menor do que o valor de `len`, cabe a você enviar os dados restantes. Se o tamanho for pequeno (mais ou menos menor ou igual a 1K), ela provavelmente vai conseguir enviar tudo de uma vez.
- retorna -1 se erro e seta a `errno`

```c
char *msg = "Hello world!";
int len, bytes_sent;
// ...
len = strlen(msg);
bytes_sent = send(sockfd, msg, len, 0);
// ...
```

```c
int recv(int sockfd, void *buf, int len, int flags);
```

- sockfd: o socket descriptor a ser lido
- buf: o buffer onde será colocado os dados
- len: o tamanho máximo do buffer
- flags: setar para 0. (olhar o man de recv())
- retorna o número de bytes realmente lidos no buffer, -1 se erro e seta a `errno`
- pode retornar 0, que significa que o outro lado fechou a conexão

### fcntl() - blocking
Muitas funções podem bloquear, esperando algum acontecimento:
- accept()
- todas as recv()

Quando criamos um socket descriptor com `socket()`, o kernel o seta para que possa ser bloqueado. Se não quiser que ele possa serr bloqueado, use `fcntl()`:

```c
#include <unistd.h>
#include <fcntl.h>
//...
sockfd = socket(PF_INET, SOCK_STREAM, 0);
fcntl(sockfd, F_SETFL, O_NONBLOCK);
```

- se tentar ler de um non-blocking socket que não possui dados nele, ele não será bloqueado, retornará -1 e a errno será setada para `EAGAIN` ou `EWOULDBLOCK` (ambos erro 11: "Resource temporarily unavailable")

### pool()
- uma forma de monitorar um monte de sockets junto e lidar com os que têm dados prontos para leitura, sem ter que ficar pesquisando continuamente para ver quais estão com dados para leitura. O processo pode "dormir", esperando dados e salvando recursos do sistema.
- passamos um array de socket fds e o tipo de eventos que estamos monitorando. O SO bloqueia na chamada do poll() até que um dos eventos aconteça ou até que se atinja um timeout especificado pelo usuário.

```c
#include <poll.h>

int poll(struct pollfd fds[], nfds_t nfds, int timeout);
```
- fds: array de `struct pollfd` com os socket fds a serem monitorados
- nfds: quantidade de socket fds
- timeout: timeout em milisegundos. Pode ser negativo - se quiser esperar para sempre
- retorna o número de elementos no array que tiveram a ocorrência do evento (não fala especificamente quais sockets tiveram eventos; precisa loopar)

```c
struct pollfd {
    int fd;         // the socket descriptor
    short events;   // bitmap of events we're interested in
    short revents;  // when poll() returns, bitmap of events that occurred
};
```

| EVENTOS |                  DESCRIÇÃO                  |
| ------- | ------------------------------------------- |
| POLLIN  | alertar quando pode ler dados (`recv()`)    |
| POLLOUT | alertar quando pode enviar dados (`send()`) |

- após `poll()` retornar, checkamos se `revents` contém `POLLIN` ou `POLLOUT`, obtendo o evento que ocorreu.
- podemos fazer mais coisas com `poll()` (olhar o man)
-

### select()
- `select()` gives you the power to monitor several sockets at the same time. It’ll tell you which ones are ready for reading, which are ready for writing, and which sockets have raised exceptions, if you really want to know that.
- `select()`, though very portable, is terribly slow when it comes to giant numbers of connections

```c
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int select(int numfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
```

