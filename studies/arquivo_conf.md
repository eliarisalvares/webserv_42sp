```# Exemplo de arquivo de configuração para servidor HTTP

# Configurações para o primeiro servidor
server {
	listen			8080				# Porta do servidor
	server_name		localhost			# Nome do servidor (opcional)
	error_pages		404 404.html		# Página de erro personalizada (opcional)

	# Configurações para rotas
	location / {
		root	/var/www/html				# Diretório raiz da rota
		indexes	index.html index2.html		# Páginas de índice padrão da rota
	}

	location /about {
		root	/var/www/html
		indexes	about.html
	}

	# Configuração para rota que redireciona
	location /redirect {
		root				/var/www/html
	t			/about		# Redireciona para a rota /about
	}

	# Configuração para rota que aceita uploads de arquivos
	location /upload {
		root			/var/www/html
		indexes			upload.html
		allowed_methods		GET POST		# informa quais métodos são permitidos nessa rota (opcional)
		upload_dir		/var/www/uploads	# Diretório onde os arquivos enviados serão salvos (opcional)
		max_body_size		300000			# Tamanho permitido do arquivo em bytes (opcional)
	}

	# Configuração para rota que executa um CGI
	location /delete {
		root			/var/www/html/delete
		indexes			delete.html
		allowed_methods		GET DELETE		
	}

	# Configuração para rota que executa um CGI
	location /cgi {
		root		/var/www/cgi
		indexes		game.py				# arquivo do programa a ser executado
	}

	location /autoindex {
		root			/var/www/html/autoindex
		autoindex		true			# habilita listagem de diretórios
	}
}```

