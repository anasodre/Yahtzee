MAIN = server
CC = gcc
CFLAGS = -Wall

# objetos a serem compilados
objs = server.o gameController.o

# Regras de compilacao
default: ${objs}
	${CC} ${CFLAGS} ${objs} -o ${MAIN}

# Remove arquivos temporarios
clean:
	-rm -f $(objs) *~

# Remove todos os arquivos do programa sem ser o codigo fonte
purge: clean
	-rm -f server

