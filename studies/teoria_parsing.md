## Encapsulamentos dos dados:
- a packet is born
- the packet is wrapped (“encapsulated”) in a header (and rarely a footer) by the first protocol (e.g, TFTP protocol)
- then the whole thing (TFTP header included) is encapsulated again by the next protocol (TCP/UDP)
- then again by the next (IP)
- then again by the final protocol on the hardware (physical) layer (say, Ethernet).

When another computer receives the packet, the hardware strips the Ethernet header, the kernel strips the IP and UDP headers, the TFTP program strips the TFTP header, and it finally has the data.

A layered model more consistent with Unix might be:

- Application Layer (telnet, ftp, etc.)
- Host-to-Host Transport Layer (TCP, UDP)
- Internet Layer (IP and routing)
- Network Access Layer (Ethernet, wi-fi, or whatever)


All you have to do for stream sockets is `send()` the data out. All you have to do for datagram sockets is encapsulate the packet in the method of your choosing and `sendto()` it out. The kernel builds the Transport Layer and Internet Layer on for you and the hardware does the Network Access Layer.

## Big-endian x Little-endian
- número de IP contém: network | host
- Big-Endian is also called Network Byte Order (guarda na memória do valor mais alto pro mais baixo esq -> dir)
- Little-Endian guarda na memória do valor mais baixo para o mais alto da dir -> esq
- Your computer stores numbers in Host Byte Order - não dá para saber se é little endian ou big endian
- A lot of times when you’re building packets or filling out data structures **you’ll need to make sure your two- and four-byte numbers are in Network Byte Order**. You just get to assume the Host Byte Order isn’t right, and you always run the value through a function to set it to Network Byte Order. The function will do the magic conversion if it has to, and this way your code is portable to machines of differing endianness. There are two types of numbers that you can convert: short (two bytes) and long (four bytes).  These functions work for the unsigned variations as well:

1. `htons()`: `h`ost `to` `n`etwork `s`hort
2. `htonl()`: `h`ost `to` `n`etwork `l`ong
3. `ntohs()`: `n`etwork `to` `h`ost `s`hort
4. `ntohl()`: `n`etwork `to` `h`ost `l`ong

You’ll want to convert the numbers to Network Byte Order before they go out on the wire, and convert them to Host Byte Order as they come in off the wire.


## structs

### socket descriptor
- it is an int

### struct addrinfo
- it is used to prepare the socket address structures for subsequent use
- It’s also used in host name lookups, and service name lookups.
- it’s one of the first things you’ll call when making a connection

```c
struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};
```

You’ll load this struct up a bit, and then call `getaddrinfo()`. It’ll return a pointer to a new linked list of these structures filled out with all the goodies you need

You can force it to use IPv4 or IPv6 in the `ai_family` field, or leave it as `AF_UNSPEC` to use whatever - your code can be IP version-agnostic.

You might not usually need to write to these structures; oftentimes, a call to getaddrinfo() to fill out your struct addrinfo for you is all you’ll need. You will, however, have to peer inside these structs to get the values out.

### struct sockaddr
- it holds socket address information for many types of sockets

```c
struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx - AF_INET (IPv4) or AF_INET6 (IPv6)
    char              sa_data[14];  // 14 bytes of protocol address
};
```

- `sa_data` contains a destination address and port number for the socket (não preencher na mão!)
- `struct sockaddr_in` (“in” for “Internet”) to be used with IPv4, `sockaddr_in6` for IPv6 helps deal with `struct sockaddr` (podemos castear um ponteiro dessas estruturas para a `struct sockaddr`)

```c
struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number - must be in Network Byte Order - using htons()
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // to turn in the same size as struct sockaddr - should be set to all zeros with the function memset()
};

// Internet address (a structure for historical reasons)
struct in_addr { // see struct in6_addr for IPv6
    uint32_t s_addr; // that's a 32-bit int (4 bytes) - the 4-byte IP address (in Network Byte Order)
};
```

- `struct sockaddr_storage`: it can holds both IPv4 and IPv6 structures. Sometimes you don’t know in advance if it’s going to fill out your `struct sockaddr` with an IPv4 or IPv6 address. So you pass in this parallel structure and then cast it to the type you need:
```c
struct sockaddr_storage {
    sa_family_t  ss_family;     // address family

   // ...
};
```
- you can see the address family in the `ss_family field`: check this to see if it’s AF_INET or AF_INET6 (for IPv4 or IPv6). Then you can cast it to the right struct if you wanna


## Http structure

the format of an HTTP message is:

```
<initial line, different for request vs. response>
Header1: value1 // zero or more header lines
Header2: value2
Header3: value3
...

<optional message body goes here, like file contents or query data;
 it can be many lines long, or even binary data $&*%@!^$@>
```

- Each part of a HTTP request is separated by a new line. Technically they should be \r\n but you are strongly encouraged to also accept \n as a newline.
- A HTTP request is terminated by two newlines. Technically they should be 4 bytes: \r\n\r\n but you are strongly encouraged to also accept 2 byte terminator: \n\n.

### Initial Request Line
A request line has three parts, separated by spaces: a method name, the local path of the requested resource, and the version of HTTP being used

- Method names are always uppercase
- The path is the part of the URL after the host name, also called the request URI
- The HTTP version always takes the form "HTTP/x.x", uppercase
- Parts of request line is separated by a space character. Technically there should be only one space though I've seen badly malformed requests that send multiple spaces. Browsers will never send more than one space

### Initial Response Line (Status Line)

It has three parts separated by spaces: the HTTP version, a response status code that gives the result of the request, and an English reasonphrase describing the status code.
- The HTTP version is in the same format as in the request line, "HTTP/x.x".
- The status code is meant to be computer-readable; the reason phrase is meant to be human-readable, and may vary
- The status code is a three-digit integer, and the first digit identifies the general category of response:
	- 1xx indicates an informational message only
	- 2xx indicates success of some kind
	- 3xx redirects the client to another URL
	- 4xx indicates an error on the client's part
	- 5xx indicates an error on the server's part

### Header Lines
Header lines provide information about the request or response, or about the object sent in the message body.
- one line per header, of the form "Header-Name: value", ending with CRLF, but you should handle LF correctly
- The header name is not case-sensitive (though the value may be). Header name can be either title-case or lowercase or mixed, all are valid
- Any number of spaces or tabs may be between the ":" and the value
- Header lines beginning with space or tab are actually part of the previous header line, folded into multiple lines for easy reading. The following two headers are equivalent:

```
Header1: some-long-value-1a, some-long-value-1b

HEADER1:    some-long-value-1a,
            some-long-value-1b
```

- HTTP 1.1 defines 46 headers, and one (Host:) is required in requests.


### The Message Body
An HTTP message may have a body of data sent after the header lines.
- In a response, this is where the requested resource is returned to the client (the most common use of the message body), or perhaps explanatory text if there's an error.
- In a request, this is where user-entered data or uploaded files are sent to the server.

If an HTTP message includes a body, there are usually header lines in the message that describe the body:
- Content-Type: header gives the MIME-type of the data in the body, such as text/html or image/gif.
- Content-Length: header gives the number of bytes in the body.

<hr>
"Be strict in what you send and tolerant in what you receive." Other clients and servers you interact with may have minor flaws in their messages, but you should try to work gracefully with them. In particular, the HTTP specification suggests the following:

Even though header lines should end with CRLF, someone might use a single LF instead. Accept either CRLF or LF.
The three fields in the initial message line should be separated by a single space, but might instead use several spaces, or tabs. Accept any number of spaces or tabs between these fields.




303 See Other (HTTP 1.1 only)
The resource has moved to another URL (given by the Location: response header), and should be automatically retrieved by the client. This is often used by a CGI script to redirect the browser to an existing file.
