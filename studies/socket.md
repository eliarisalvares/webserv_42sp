### TCP e Socket

Antes de tudo, para que um cliente possa se comunicar com um servidor, é necessário que ambos estejam **conectados***.

Há muitos processos rodando ao mesmo tempo em um host (o seu computador, por exemplo). Cada processo é identificado por um número, chamado de **porta**. Um processo pode ser um servidor web, um servidor de e-mail, um servidor de arquivos, etc. Um servidor web usará uma porta para poder enviar e receber dados. A porta padrão para um servidor web é a porta 80. Para poder usar uma porta do host, o servidor web deve se conectar a ela. A conexão é chamada de **socket**.

Em Unix, como sabemos, **tudo é um arquivo**. Um socket é um arquivo especial que permite que um cliente (não todos os clientes, apenas o cliente que se conectou a ele) se comunique com o servidor. O cliente pode enviar dados para o servidor e o servidor pode enviar dados para o cliente. O servidor pode ter vários sockets, um para cada cliente conectado a ele. Em resumo: o socket é uma forma de falar com outros programas usando standard Unix file descriptors.

- famosa frase "everything in Unix is a file!" = when Unix programs do any sort of I/O, they do it by reading or writing to a file descriptor. A file descriptor is simply an integer associated with an open file. But that file can be a network connection, a FIFO, a pipe, a terminal, a real on-the-disk file, or just about anything else. Everything in Unix is a file!
- a função `socket()` retorna um socket descriptor. Podemos nos comunicar por ele usando `send()` e `recv()` (chamadas de "chamadas de socket". Poderíamos ainda usar as funções `read()` e `write()`, mas as outras duas funções dão maior controle sobre a transmissão de dados
- há vários tipos de socket; no caso do projeto (e do foco do cara) é o socket de internet

Tipos de socket (tem mais além desses dois):
#### stream sockets
- two-way connected communication streams.
- Os conteúdos chegam no receptor na ordem em que eles foram transmitidos e são error-free.
- `telnet` e `ssh` usam esse tipo de socket
- web browsers usam o HTTP, que usa stream sockets para pegar páginas. Um exemplo usando telnet: "if you telnet to a web site on port 80, and type “GET / HTTP/1.0” and hit RETURN twice, it’ll dump the HTML back at you"
- esses sockets têm esse alto nível de transmissão de dados porque eles usam o Transmission Control Protocol (TCP), que garante que os dados cheguem sequencialmente e sem erro
- TCP/IP (Internet Protocol): IP lida com roteamento da internet e não é geralmente responsável pela integridade dos dados

#### datagram sockets
- são chamados de sem conexão - não temos que manter uma conexão aberta como no caso dos stream sockets; não é necessário uma conexão para o envio dos dados
- não são confiáveis
- ele pode chegar (pelo que eu entendi pode ser que não chegue)
- Pode chegar fora de ordem
- Se chegar, os dados dentro do pacote estarão livres de erros.
- usam o UDP (User Datagram Protocol)
- usados quando TCP não está disponível ou quando a perda de alguns dados não é importante
- uso: multiplayer games, streaming audio, video conferencing
- escolha: quando precisa de velocidade


#### Portas
- Sockets usam o número de IP e o número da porta (16 bits): o IP é como o endereço de um hotel e a porta, o número do quarto.
- Há números padrão de portas. Conseguimos ver em sistemas Unix no arquivo `/etc/services`
- Portas com número abaixo de 1024 são especiais e geralmente requerem privilégios no OS

