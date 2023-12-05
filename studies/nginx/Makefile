all: run

run:
	docker-compose --project-name nginx up -d
# se rodar no terminal direto da raiz do projeto:
# docker-compose -f studies/nginx/docker-compose.yml --project-name nginx up -d

stop:
	docker stop nginx

clean:
	docker rm -f nginx
	docker rmi test

re: clean run

.PHONY: all clean fclean re
