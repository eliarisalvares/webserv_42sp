### Arquivos de configuração

O arquivo de configuração deve conter as seguintes informações:

- Porta e endereço para escutar de cada servidor
- Server name: o nome do servidor
- O primeiro servidor para um host:porta específico é o servidor padrão para esse host:porta, ou seja, se o cliente não especificar um host:porta, o servidor padrão deve ser usado.
- Error page: a página de erro a ser exibida em caso de erro
- Limite de tamanho para o corpo da mensagem
- Lista de métodos permitidos
- Lista de métodos que devem ser tratados como redirecionamentos
- Diretório ou arquivo onde o arquivo deve ser buscado. Por exemplo, se a url for /index.html, o servidor deve procurar o arquivo index.html no diretório raiz do servidor.
- Arquivo padrão para a resposta caso a requisição seja para um diretório. Por exemplo, se a url for /, o servidor deve procurar o arquivo index.html no diretório raiz do servidor.
- Execução de CGI baseada em extensão de arquivo. Por exemplo, se a url for /index.php, o servidor deve executar o arquivo index.php no diretório raiz do servidor.
- A rota deve aceitar arquivos a serem carregador pelo cliente e configurar onde eles devem ser salvos.

* CGI não deve ser chamado diretamente, mas seu path deve ser configurado no arquivo de configuração como PATH_INFO.
* Para requisições chunked, o servidor deve ser capaz de receber e enviar dados em chunks. O CGI vai esperar que EOF seja o fim do corpo da mensagem.
* Se nenhum content_length for especificado, o servidor deve ler o corpo da mensagem até EOF.
* O CGI deve ser requisita com o arquivo como primeiro argumento.
* O CGI deve estar rodando no diretório correto para que paths relativos funcionem.
* O programa deve funcionar com apenas um CGI.
* O servidor não deve nunca morrer ou ter vazamentos de memória
* O comportamento do servidor deve ser consistente com o NGINX.
