FROM debian:bullseye-slim

RUN apt update -y && apt install nginx -y && apt install openssl -y

# COPY ./nginx.conf /etc/nginx/conf.d

EXPOSE 80

# - nginx docker image
CMD ["nginx", "-g", "daemon off;"]
