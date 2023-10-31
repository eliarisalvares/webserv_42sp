## Pool

Headers necessários para usar no C/C++:
```c
#include <iostream>
#include <vector>
#include <poll.h>
#include <unistd.h>  // For close()
```

#### Define Pollfd Structures:
The `poll()` function uses an array of _pollfd_ structures to specify the file descriptors you want to monitor and the events you're interested in. Each _pollfd_ structure contains the following fields:
- fd: The file descriptor you want to monitor.
- events: The events you want to monitor (e.g., POLLIN for read events, POLLOUT for write events).
- revents: This field is filled by poll to indicate which events occurred.

#### Call poll:
The poll function is called with the array of pollfd structures and a timeout. It will block until one or more of the specified events occur or until the specified timeout expires. A timeout value of -1 means that it will block indefinitely until an event occurs.

#### Check Events:
After poll returns, you can check the revents field of each pollfd structure to determine which events have occurred. 

#### Error Handling:
It's important to handle errors that may occur during poll. poll can return -1 in case of an error, and you should handle it accordingly.

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

----

## Select

- `select()` gives you the power to monitor several sockets at the same time. It’ll tell you which ones are ready for reading, which are ready for writing, and which sockets have raised exceptions, if you really want to know that.
- `select()`, though very portable, is terribly slow when it comes to giant numbers of connections
- It uses a set of bit masks to specify the file descriptors to monitor and the events of interest. These bit masks are updated by the select function to indicate which file descriptors are ready for I/O.
- It is part of the Berkeley Sockets API and is available on a wide range of Unix-based systems, including Linux.
- `select()` is an older and widely supported I/O multiplexing system call that is available on various operating systems, including Unix-based systems. It monitors multiple file descriptors for read, write, or exception events and blocks until one or more of these events occur.

Headers necessários para usar no C/C++:
```c
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int select(int numfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
```

Here are some key points about the `select()` function:

##### Monitoring Multiple File Descriptors:
`select()` is typically used to monitor multiple file descriptors (e.g., sockets, pipes, files) for specific I/O events.
It takes in three sets of file descriptor sets: readfds, writefds, and exceptfds, allowing you to specify which file descriptors you want to monitor for read, write, or exceptional conditions.

##### Blocking or Non-Blocking:
`select()` can be used in both blocking and non-blocking modes. In blocking mode, it will block until at least one of the specified file descriptors is ready for I/O. In non-blocking mode, it returns immediately.

##### Timeout:
You can specify a timeout value, which determines how long `select()` will wait for I/O events before returning. A timeout of NULL means it will block indefinitely until an event occurs.

##### Return Value:
`select()` modifies the file descriptor sets provided to it and returns the total number of file descriptors that are ready for I/O. If the return value is 0, it means that the timeout occurred without any events. If it's -1, an error occurred.

##### FD_SET and FD_ISSET:
The FD_SET(fd, set) macro is used to add a file descriptor fd to a set.
The FD_ISSET(fd, set) macro is used to check if a file descriptor fd is part of a set.

##### Limitations:
`select()` has some limitations, including a maximum limit on the number of file descriptors that it can monitor (usually 1024, although this limit can vary between systems).
It can be less efficient compared to more modern I/O multiplexing mechanisms like poll, epoll, and kqueue.

##### Usage Scenarios:
`select()` is suitable for basic I/O multiplexing needs, especially when you have a limited number of file descriptors to monitor.
It can be useful for simple network servers, where you need to manage a small number of connections efficiently.

##### Portability:
One of the advantages of `select()` is its portability. It's available on a wide range of Unix-based systems, making it a useful choice for cross-platform development.