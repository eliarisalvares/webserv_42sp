# Webserv

Para rodar o projeto, é necessário um arquivo de configuração com a extensão ``.conf``, a ser definida pelo usuário.

Seguem algumas configurações:

###Configuration File

Os caminhos setados terão que ser existentes e estarão no ``/content``.
As páginas de erro deverão estar no caminho ``/content/error_pages/``

```# Configuração mínima para o webserv rodar

server {
	listen 8080						# Porta do servidor
}```

```# Exemplo de arquivo de configuração para servidor HTTP

# Configurações para o servidor
server {
	listen 8080						# Porta do servidor
	server_name localhost			# Nome do servidor (opcional)
	error_page 404 404.html			# Página de erro personalizada (opcional)
	client_max_body_size 1M			# Define o tamanho permitido do arquivo em bytes (opcional)
	root /var/						# Diretório raiz da rota (opcional)
	cgi .py python 3				# Define o uso do cgi - apenas python (opcional)
	allowed_methods GET POST DELETE # Define quais métodos são permitidos(opcional)
	index index.html				# Páginas de índice padrão da rota (opcional)
	buffer_size 256					# Buffersize para leitura - maior que 1 e menor que 1024 (opcional)
	autoindex true					# habilita listagem de diretórios (opcional)


	# Configurações para rotas  (opcional)
	location / {
		root /				# Diretório raiz da rota
		index index.html				# Páginas de índice padrão da rota
	}

	location /about {
		root /html
		index about.html
	}

	location /autoindex {
		root /var/www/html/autoindex
		autoindex true			# habilita listagem de diretórios
	}
}```



```
	# Configuração para rota que redireciona  (opcional)
	location /redirect {
		root /var/www/html
		redirect /about		# Redireciona para a rota /about
	}


```

Definições:

- client_max_body_size:
	Sets the maximum body size for client requests in bytes.
	Megabytes, M, and Kilobytes, K, are the accepted units.