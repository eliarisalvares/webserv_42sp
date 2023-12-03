Definições já fixadas.

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

	# Configurações para rotas  (opcional)
	location / {
		root /var/www/html				# Diretório raiz da rota
		index index.html				# Páginas de índice padrão da rota
	}

	location /about {
		root /var/www/html
		index about.html
	}
}```


Definições ainda em processo de parseamento
```
	# Configuração para rota que redireciona  (opcional)
	location /redirect {
		root /var/www/html
		redirect /about		# Redireciona para a rota /about
	}

	# Configuração para rota que aceita uploads de arquivos  (opcional)
	location /upload {
		root /var/www/html
		index upload.html
		allowed_methods GET POST		# informa quais métodos são permitidos nessa rota (opcional)
		upload_dir /var/www/uploads	# Diretório onde os arquivos enviados serão salvos (opcional)
		max_body_size 300000			# Tamanho permitido do arquivo em bytes (opcional)
	}

	# Configuração para rota que executa um CGI  (opcional)
	location /delete {
		root /var/www/html/delete
		index delete.html
		allowed_methods GET DELETE		
	}

	# Configuração para rota que executa um CGI  (opcional)
	location /cgi {
		root /var/www/cgi
		index game.py				# arquivo do programa a ser executado
	}

	location /autoindex {
		root /var/www/html/autoindex
		autoindex true			# habilita listagem de diretórios
	}
```