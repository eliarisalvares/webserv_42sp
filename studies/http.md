## HTTP, cliente-servidor e o protocolo

### O que é HTTP?

HTTP é um protocolo de comunicação que permite que os clientes se comuniquem com os servidores. O cliente é quem faz a solicitação e o servidor é quem processa e responde à solicitação. O cliente mais comum é um navegador, que é responsável por enviar solicitações ao servidor em nome do usuário e exibir os resultados do processamento na tela. A promessa formal (protocolo) de qual formato de cadeia de caracteres enviar a solicitação ao servidor e a resposta ao cliente é HTTP.

### O que é um servidor web?

Um servidor web é um programa que usa o protocolo HTTP para atender às solicitações de clientes. O servidor web é responsável por processar a solicitação e enviar uma resposta, que geralmente é a página da web solicitada. O servidor web também pode ser responsável por processar solicitações de outros protocolos, como o protocolo HTTPS, que é uma versão segura do HTTP.

### O que é um cliente web?

Um cliente web é um programa que usa o protocolo HTTP para enviar solicitações a um servidor web. O cliente web é responsável por enviar solicitações e exibir as respostas recebidas do servidor web. O cliente web mais comum é um navegador, que é responsável por enviar solicitações ao servidor em nome do usuário e exibir os resultados do processamento na tela.

### O que é um método HTTP?

Um método HTTP é um verbo que indica a ação a ser executada no recurso identificado pela solicitação. Os métodos HTTP que devem ser implementados são:

- GET: solicita uma representação do recurso especificado. As solicitações usando GET devem apenas recuperar dados.
- POST: solicita que o servidor aceite a entidade incluída na solicitação como um novo subordinado do recurso identificado pela URI. POST é usado para criar um novo recurso.
- DELETE: solicita que o servidor exclua o recurso identificado pela URI.
