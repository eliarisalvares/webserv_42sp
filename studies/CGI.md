O que é CGI?
CGI significa Common Gateway Interface. É um padrão para executar programas em um servidor web. O servidor web pode ser configurado para executar um programa quando um recurso específico for solicitado. O programa pode ser escrito em qualquer linguagem de programação. O programa é executado e a saída é enviada de volta ao cliente. O programa pode ser usado para gerar uma página da web dinamicamente.

Uma analogia
Imagine que você esteja num restaurante. O menu (website) oferece várias opções de pratos (informações). Mas, em vez de escolher algo que já está pronto, você deseja algo personalizado, algo que ainda não foi preparado.

Aqui entra o CGI:

O Garçom (Web Browser): Você diz ao garçom o que deseja.
A Cozinha (Servidor Web): O garçom leva seu pedido à cozinha. A cozinha representa o servidor web.
O Chef (CGI): O chef recebe as instruções e começa a preparar algo novo, algo personalizado, de acordo com seu pedido.
O Prato Preparado (Resposta): O chef termina de preparar o prato e entrega ao garçom.
Entrega ao Cliente (Você vê a resposta no seu navegador): O garçom retorna e serve o prato personalizado para você.
No contexto web:

Você faz um pedido: Por exemplo, ao preencher um formulário em um site.
Navegador envia o pedido ao servidor: As informações do formulário são enviadas ao servidor web.
CGI entra em ação: O servidor usa um programa CGI para processar as informações recebidas.
Processamento: O programa CGI faz o que precisa fazer (como consultar um banco de dados, fazer cálculos, etc.).
Resposta é gerada: O programa CGI cria uma resposta adequada.
Resposta é enviada de volta ao navegador: Você vê o resultado no seu navegador, seja uma mensagem de confirmação, os resultados de uma pesquisa, etc.
CGI vs Frameworks de Backend
CGI (Common Gateway Interface):

É um protocolo que permite que um servidor web se comunique com programas externos (geralmente escritos em linguagens como Perl, C ou Python).
O CGI define como os dados do usuário são transmitidos do navegador para esse programa e como os resultados são enviados de volta ao navegador.
Pode ser considerado uma técnica antiga para lidar com funcionalidades dinâmicas em sites.
Backend:

Refere-se à parte do sistema que lida com a lógica, processamento de dados, interação com bancos de dados e outras operações que ocorrem "por trás das cortinas" e que o usuário final não vê diretamente.
O backend pode ser composto por diversos componentes, incluindo servidores web, servidores de aplicativos, bancos de dados e mais.
Ao falar sobre desenvolvimento web moderno, geralmente nos referimos a frameworks e plataformas de backend como Express (Node.js), Django (Python), Rails (Ruby), entre outros.
Diferenças:

Escopo: O CGI é apenas uma especificação para a comunicação entre o servidor web e um programa externo. Já o backend é uma parte maior do sistema que abrange toda a lógica e processamento que uacontecem no servidor.

Modernidade: O CGI é uma técnica mais antiga, enquanto muitas soluções de backend modernas não usam CGI, preferindo abordagens mais contemporâneas, como APIs RESTful.

Performance: O CGI cria um novo processo para cada requisição, o que pode ser ineficiente para sites com muito tráfego. Soluções de backend modernas geralmente têm maneiras mais eficientes de lidar com múltiplas requisições simultaneamente.

Uso: Embora o CGI ainda possa ser usado em alguns contextos, muitos desenvolvedores preferem usar frameworks de backend modernos devido à sua eficiência, segurança e facilidade de desenvolvimento.

Em resumo, enquanto o CGI é uma técnica específica para permitir a comunicação entre um servidor web e programas externos, o backend é um termo mais amplo que abrange toda a infraestrutura e lógica de servidor de um sistema ou aplicativo web.

Arquivo de configuração e CGI:
O arquivo deve conter a extensão para execução de CGI, que pode ser implementado usando Python ou Perl ou PHP ou qualquer outra linguagem de script. Por exemplo, se a url for /index.php, o servidor deve executar o arquivo index.php no diretório raiz do servidor.
O CGI não deve ser chamado diretamente, mas seu path deve ser configurado no arquivo de configuração como PATH_INFO.
Para requisições chunked, o servidor deve ser capaz de receber e enviar dados em chunks. O CGI vai esperar que EOF seja o fim do corpo da mensagem.
Se nenhum content_length for especificado, o servidor deve ler o corpo da mensagem até EOF.
O CGI deve ser requisita com o arquivo como primeiro argumento.
O CGI deve estar rodando no diretório correto para que paths relativos funcionem.
O programa deve funcionar com apenas um CGI.
O comportamento do servidor deve ser consistente com o NGINX.