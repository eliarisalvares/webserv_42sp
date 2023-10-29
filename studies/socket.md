### TCP e Socket

Antes de tudo, para que um cliente possa se comunicar com um servidor, é necessário que ambos estejam **conectados***.

Há muitos processos rodando ao mesmo tempo em um host (o seu computador, por exemplo). Cada processo é identificado por um número, chamado de **porta**. Um processo pode ser um servidor web, um servidor de e-mail, um servidor de arquivos, etc. Um servidor web usará uma porta para poder enviar e receber dados. A porta padrão para um servidor web é a porta 80. Para poder usar uma porta do host, o servidor web deve se conectar a ela. A conexão é chamada de **socket**.

Em Unix, como sabemos, **tudo é um arquivo**. Um socket é um arquivo especial que permite que um cliente (não todos os clientes, apenas o cliente que se conectou a ele) se comunique com o servidor. O cliente pode enviar dados para o servidor e o servidor pode enviar dados para o cliente. O servidor pode ter vários sockets, um para cada cliente conectado a ele.
