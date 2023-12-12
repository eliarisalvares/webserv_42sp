/* RequestParser */

typedef enum e_abnf_rules {
	ALPHA, // (letters)
	// CR, // (carriage return)
	CRLF, // (CR LF)
	CTL, // (controls)
	DIGIT, // (decimal 0-9)
	DQUOTE, // (double quote)
	HEXDIG, // (hexadecimal 0-9/A-F/a-f)
	// HTAB, // (horizontal tab)
	// LF, // (line feed)
	OCTET, // (any 8-bit sequence of data)
	// SP, // (space)
	VCHAR, //(any visible US-ASCII character)
}           t_abnf_rules;


// Each part of a HTTP request is separated by a new line
// Note: Technically they should be \r\n but you are strongly encouraged to also accept \n as a newline
// Initial lines and headers should end in CRLF, though you should gracefully handle lines ending in just LF.
// Even though header lines should end with CRLF, someone might use a single LF instead. Accept either CRLF or LF.
// lidando apenas com CRLF por hora, verificar no RFC
void check_crlf(char c);

// The most common use of POST, by far, is to submit HTML form data to CGI scripts.
// In this case, the Content-Type: header is usually application/x-www-form-urlencoded,
// and the Content-Length: header gives the length of the URL-encoded form data
// Ex:
// POST /path/script.cgi HTTP/1.0
// From: frog@jmarshall.com
// User-Agent: HTTPTool/1.0
// Content-Type: application/x-www-form-urlencoded
// Content-Length: 32

// home=Cosby&favorite+flavor=flies


// You can use a POST request to send whatever data you want, not just form submissions.
// Just make sure the sender and the receiving program agree on the format.


// URL encoding
// HTML form data is usually URL-encoded to package it in a GET or POST submission. In a nutshell, here's how you URL-encode the name-value pairs of the form data:

// Convert all "unsafe" characters in the names and values to "%xx", where "xx" is the ascii value of the character, in hex. "Unsafe" characters include =, &, %, +, non-printable characters, and any others you want to encode-- there's no danger in encoding too many characters. For simplicity, you might encode all non-alphanumeric characters.
// Change all spaces to plusses.
// String the names and values together with = and &, like
// name1=value1&name2=value2&name3=value3
// This string is your message body for POST submissions, or the query string for GET submissions.
// For example, if a form has a field called "name" that's set to "Lucy", and a field called "neighbors" that's set to "Fred & Ethel", the URL-encoded form data would be
// name=Lucy&neighbors=Fred+%26+Ethel
// with a length of 34.

// Using GET to Submit Query or Form Data
// You can use GET to send small amounts of data to the server. The key is to
// understand just what the request URI is: It's not necessarily a file name,
// it's a string that identifies a data resource on the server. That may be a file
// name, but it may also be, for example, a specific query to a specific database.
// The result of that query may not live in a file, but it's a data resource all
// the same, identified by the search engine and the query data that together produce it.

// So, to send data to a CGI script using a GET request, include that data after
// the question mark in the URL (read about URL syntax for more details). For example:

// GET /path/script.cgi?field1=value1&field2=value2 HTTP/1.0


// CGI response
// Sending an Existing File Back as a Response
// If your HTML response is always the same, or if you want to respond with one of several existing files, you may find the "Location:" response header useful. Use it to redirect the browser to another URL.

// By way of example, if your CGI script prints the following line to STDOUT:

// Location: response.html
// followed by a blank line, then the remote browser will retrieve response.html and treat it as the response from your CGI script. You can redirect the browser to either a relative or absolute URL.
// In this situation, do not print the "Content-type:" response header.


// "Transfer-Encoding: chunked" header. All HTTP 1.1 clients must be able to receive chunked messages
// Servers aren't required to generate chunked messages; they just have to be able to receive them.

// POST ->

// chunked:
// Transfer-Encoding: chunked
// Content-Length: 0 (se tiver, deve ser 0 para evitar ambiguidade)


// 3. Chunk Format (RFC 7230, Section 4.1):
// Format: Each chunk starts with the size of the chunk in hexadecimal followed
// by a CRLF (carriage return and line feed) sequence, then the chunk data, and
// another CRLF.
// Example:
// http
// Copy code
// POST /example HTTP/1.1
// Host: example.com
// Transfer-Encoding: chunked

// 4
// data
// 5
// chunk
// 0
// Explanation: In this example, two chunks of data are sent. The size of the
// first chunk is 4, followed by the chunk data ("data"). The size of the second
// chunk is 5, followed by the chunk data ("chunk"). A chunk with size 0
// indicates the end of the message.
// 4. Trailer Headers (Optional):
// Header Format: Additional headers may be sent in trailers after the final
// chunk. Trailers are optional and can include headers like Content-MD5 or others.


// @app.route('/example', methods=['GET'])
// def handle_request():
//     user_input = request.headers.get('User-Input')

//     # Validate and sanitize user input
//     if '\r' in user_input or '\n' in user_input:
//         return "Invalid input", 400

/* parse_control_data */

/* methods */
void method(char c);
// general:
// Parts of request line is separated by a space character.
// Technically there should be only one space though I've seen badly malformed requests that send multiple spaces.
// Browsers will never send more than one space
// nginx aceita vários espaços e nós tb estamos aceitando
// não aceita espaço no início da primeira linha (não é letra maiúscula)

// The three fields in the initial message line should be separated by a single space, but might instead use several spaces, or tabs. Accept any number of spaces or tabs between these fields.

// Method names are always uppercase
// The method token is case-sensitive
// When a request method is received that is unrecognized or not implemented by an origin
// server, the origin server SHOULD respond with the 501 (Not Implemented) status code.
// When a request method is received that is known by an origin
// server but not allowed for the target resource, the origin server
// SHOULD respond with the 405 (Method Not Allowed) status code.

/* URI */
void uri(char c);
// URI enconding (tb serve pra POST) - não entendi se posso receber https
// URI normalization:
// normalizing the scheme and host to lowercase
// normalizing the port to remove any leading zeros.
// If port is elided from the URI, the default port for that scheme is used
// o host pode ser um IP, que pode estar em ipv4 ou ipv6...
// It is RECOMMENDED that all senders and recipients support, at a minimum, URIs with lengths of 8000 octets in protocol elements.

void protocol(char c);
// The HTTP version always takes the form "HTTP/x.x", uppercase

void version(char c);
// RFC 9110 - item 2.5. Protocol Version:
// HTTP's version number consists of two decimal digits separated by a "."
// (period or decimal point)
// version format: <major-version>.<minor-version>

// RFC 9110 - item 6.2. Control Data:
// A server can send a 505 (HTTP Version Not Supported) response if it wishes,
// for any reason, to refuse service of the client's major protocol version.
// if major-version != 1: refuse -> 505

// RFC 9110 - item 6.2. Control Data:
// A recipient that receives a message with a major version number that it
// implements and a minor version number higher than what it implements SHOULD
// process the message as if it were in the highest minor version within that
// major version to which the recipient is conformant
// temos que aceitar 1.2, 1.3, 1.11
// não precisamos aceitar 1.0?


void _check_uri(void);
// 	The generic syntax uses the slash ("/"), question mark ("?"), and
//    number sign ("#") characters to delimit components that are
//    significant to the generic parser's hierarchical interpretation of an
//    identifier

// se % -> caracter especial com valor em hexadecimal % + 2 caracteres hexadecimais
// For example, "%20" = SP
// "%" HEXDIG HEXDIG
// uppercase e lowercase HEXDIG são iguais
// For consistency, URI producers and
//    normalizers should use uppercase hexadecimal digits for all percent-
//    encodings.

// Reserved Characters:
// reserved    = gen-delims / sub-delims
// gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@" -> delimiting
//    characters that are distinguishable from other data within a URI
// sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
//                   / "*" / "+" / "," / ";" / "="

//  Unreserved Characters: Characters that are allowed in a URI but do not have a reserved purpose:
// ALPHA / DIGIT / "-" / "." / "_" / "~"


/* parse_headers */

void header(char c);
// headers
// servers should treat headers as an unordered set
// one line per header, of the form "Header-Name: value", ending with CRLF
// you should handle LF correctly
// HTTP 1.1 defines 46 headers, and one (Host:) is required in requests.

// A server MUST NOT apply a request to the target resource until it receives the
// entire request header section, since later header field lines might include
// conditionals, authentication credentials, or deliberately misleading duplicate
// header fields that could impact request processing


void _parse_field_name(char c);
// Field name:
// The requested field name. It MUST conform to the field-name syntax defined
// in Section 5.1, and it SHOULD be restricted to just letters, digits,
// and hyphen ('-') characters, with the first character being a letter.
// nginx não segue essas regras, aceita qualquer coisa, google tb...

// The header name is not case-sensitive (though the value may be) - Header
// name can be either title-case or lowercase or mixed, all are valid

// nos headers parece considerar um espaço no começo do nome do header como parte dele
// interpreta " Host" e não "Host" (dá erro porque considera que o header não está incluso)
// Header lines beginning with space or tab are actually part of the previous header line, folded into multiple lines for easy reading.
// the following two headers are equivalent:

// Header1: some-long-value-1a, some-long-value-1b
// HEADER1:    some-long-value-1a,
//             some-long-value-1b

void _parse_field_value(char c);
// Any number of spaces or tabs may be between the ":" and the value

void _check_host(void);
// header Host is mandatory and singleton

void _check_content_length(void);
// header Content-Length has only numbers, is singleton and has a maximum size

void _set_content_type(void);
// content-type is singleton and the last value passed is its final value
// (if it is not an empty value)


// Representation headers include: Content-Type, Content-Encoding, Content-Language, and Content-Location.

// Content-Type (ignora os parametros adicionais)
// application/x-www-form-urlencoded, multipart/form-data, or text/plain, image/...
// MIME type: type/subtype;parameter=value


// application/octet-stream -> arquivo binário -> vou recusar
// na vdd vou recusar tipos diferentes de imagem, application/x-www-form-urlencoded, multipart/form-data, or text/...
// Aceitar:
// application/x-www-form-urlencoded, multipart/form-data
// text/plain
// text/html
// text/css
// image/gif: Graphics Interchange Format (GIF)
// image/jpeg: Joint Photographic Expert Group image (JPEG)
// image/png: Portable Network Graphics (PNG)

// application/json -> tem parse específico... (provavelmente não vou ter tempo)

// - definir o type do arquivo que salva

// 403 Forbidden
// The 403 (Forbidden) status code indicates that the server understood the request but refuses to fulfill it. A server that wishes to make public why the request has been forbidden can describe that reason in the response content (if any).

// 408 Request Timeout
// The 408 (Request Timeout) status code indicates that the server did not receive a complete request message within the time that it was prepared to wait.


// 414 URI Too Long
// The 414 (URI Too Long) status code indicates that the server is refusing to service the request because the target URI is longer than the server is willing to interpret.


//  Unsupported Media Type
// The 415 (Unsupported Media Type) status code indicates that the origin server is refusing to service the request because the content is in a format not supported by this method on the target resource.

// The format problem might be due to the request's indicated Content-Type or Content-Encoding, or as a result of inspecting the data directly.

// If the problem was caused by an unsupported content coding, the Accept-Encoding response header field (Section 12.5.3) ought to be used to indicate which (if any) content codings would have been accepted in the request.

// On the other hand, if the cause was an unsupported media type, the Accept response header field (Section 12.5.1) can be used to indicate which media types would have been accepted in the request.

// 15.5.21. 422 Unprocessable Content
// The 422 (Unprocessable Content) status code indicates that the server understands the content type of the request content (hence a 415 (Unsupported Media Type) status code is inappropriate), and the syntax of the request content is correct, but it was unable to process the contained instructions. For example, this status code can be sent if an XML request content contains well-formed (i.e., syntactically correct), but semantically erroneous XML instructions.

// check de content-type - se POST apenas
// - verifica se é um tipo aceito
// - pega parametro se tiver (deppis do ;)



/* parse_body */

// Body

//  Request messages are never close-delimited because they are always explicitly
//  framed by length or transfer coding, with the absence of both implying the
//   request ends immediately after the header section

// A server MAY reject a request that contains a message body but not a
// Content-Length by responding with 411 (Length Required).

// If an HTTP message includes a body, there are usually header lines in the message that describe the body, eg (ver se são obrigatórios):
// Content-Type: header gives the MIME-type of the data in the body, such as text/html or image/gif.
// Content-Length: header gives the number of bytes in the body.

// A HTTP request is terminated by two newlines
// Note: Technically they should be 4 bytes: \r\n\r\n but you are strongly encouraged to also accept 2 byte terminator: \n\n.

// If a valid Content-Length header field is present without Transfer-Encoding,
// its decimal value defines the expected message body length in octets. If the
// sender closes the connection or the recipient times out before the indicated
// number of octets are received, the recipient MUST consider the message to be
// incomplete and close the connection.


/***********************************************************************/
/* chunked */

// RFC 9112 - 6.3.4
// If a Transfer-Encoding header field is present in a request and the chunked
// transfer coding is not the final encoding (0), the message body length cannot
// be determined reliably; the server MUST respond with the 400 (Bad Request)
//  status code and then close the connection.

// depois de ler cada chunck, devolve uma resposta com status code = 202 pra
// indicar pro cliente que foi aceito
// quando finalizar, retorna 200 ou 201 (CREATED)

// Example:
// 4␍␊            (chunk size is four octets)
// Wiki           (four octets of data)
// ␍␊             (end of chunk)

// 7␍␊            (chunk size is seven octets)
// pedia i        (seven octets of data)
// ␍␊             (end of chunk)

// B␍␊            (chunk size is eleven octets)
// n ␍␊chunks.    (eleven octets of data)
// ␍␊             (end of chunk)

// 0␍␊            (chunk size is zero octets, no more chunks)
// ␍␊             (end of final chunk with zero data octets)

// Result string: "Wikipedia in ␍␊chunks."
// Result print:
// "Wikipedia in
// chunks."


/* multipart/form-data */

// --------------------------f069bd9492f6146e
// ------------------------f069bd9492f6146e

// 2 DASHS - BOUNDARY - CRLF
// BODY_HEADERS:
// Content-Disposition: form-data; params (name="" pelo menos...) - CRLF
// Content-Type (opcional) - CRLF
// CRLF
// content ... CRLF
// 2 DASHS - BOUNDARY - CRLF

// --------------------------62774971962833fd
// Content-Disposition: form-data; name="file"; filename="txt.txt"
// Content-Type: text/plain

// Hello World
// --------------------------62774971962833fd--
