RFC
RFC significa Request for Comments. É um documento que descreve um padrão de internet.

O RFC 7231 é uma das partes fundamentais que compõem a especificação HTTP/1.1. Ele define os aspectos semânticos do protocolo HTTP e inclui, entre outras coisas, informações sobre os métodos HTTP, códigos de status e cabeçalhos. Vamos detalhar um pouco mais:

O RFC 7231 faz parte de uma série de documentos que definem o protocolo HTTP/1.1, atualizando e substituindo a especificação anterior (RFC 2616). Ele foi projetado para esclarecer ambiguidades, corrigir erros e adicionar informações que faltavam na especificação anterior.

Todos os retornos HTTP de acordo com RFC 7231
100s: Informacional
100 Continue
101 Switching Protocols
200s: Sucesso
200 OK
201 Created
202 Accepted
203 Non-Authoritative Information
204 No Content
205 Reset Content
300s: Redirecionamento
300 Multiple Choices
301 Moved Permanently
302 Found
303 See Other
305 Use Proxy
307 Temporary Redirect
400s: Erro do cliente
400 Bad Request
402 Payment Required
403 Forbidden
404 Not Found
405 Method Not Allowed
406 Not Acceptable
408 Request Timeout
409 Conflict
410 Gone
411 Length Required
413 Payload Too Large
414 URI Too Long
415 Unsupported Media Type
417 Expectation Failed
426 Upgrade Required
500s: Erro do servidor
500 Internal Server Error
501 Not Implemented
502 Bad Gateway
503 Service Unavailable
504 Gateway Timeout
505 HTTP Version Not Supported
O que é um cabeçalho HTTP?
O cabeçalho permite passar informações adicionais entre o cliente e o servidor. O cabeçalho é composto por um nome e um valor separados por dois pontos. O cabeçalho é enviado antes do corpo da mensagem.

Formato de uma solicitação HTTP e uma resposta HTTP (com cabeçalhos) (o subject fala sobre usar o curl para realizar requisições)
O formato básico de uma solicitação HTTP é:

<method> <request-target> <version>
<headers>

<entity-body>
O formato básico de uma resposta HTTP é:

<version> <status> <reason-phrase>
<headers>

<entity-body>
Exemplo de uma solicitação HTTP
GET / HTTP/1.1
Host: www.example.com
Accept-Language: fr
Exemplo de uma resposta HTTP
HTTP/1.1 200 OK
Date: Mon, 23 May 2005 22:38:34 GMT
Content-Type: text/html; charset=UTF-8
Content-Length: 138
Last-Modified: Wed, 08 Jan 2003 23:11:55 GMT
Server: Apache/
Connection: close

<html>
<head>
<title>An Example Page</title>
</head>
<body>
Hello World, this is a very simple HTML document.
</body>
</html>