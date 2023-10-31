### Pool

Headers necessários para usar no C/C++:
```
#include <iostream>
#include <vector>
#include <poll.h>
#include <unistd.h>  // For close()
```

#### Define Pollfd Structures:
The _poll_ function uses an array of _pollfd_ structures to specify the file descriptors you want to monitor and the events you're interested in. Each _pollfd_ structure contains the following fields:
- fd: The file descriptor you want to monitor.
- events: The events you want to monitor (e.g., POLLIN for read events, POLLOUT for write events).
- revents: This field is filled by poll to indicate which events occurred.

#### Call poll:
The poll function is called with the array of pollfd structures and a timeout. It will block until one or more of the specified events occur or until the specified timeout expires. A timeout value of -1 means that it will block indefinitely until an event occurs.

#### Check Events:
After poll returns, you can check the revents field of each pollfd structure to determine which events have occurred. 

#### Error Handling:
It's important to handle errors that may occur during poll. poll can return -1 in case of an error, and you should handle it accordingly.

