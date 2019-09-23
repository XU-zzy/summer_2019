CC=g++
OBJ = zzychat_client
all:
	$(CC)  -o  $(OBJ)  *.h    *.c     -lpthread  -I/usr/include/mysql/ -L/usr/lib/mysql -lmysqlclient   -g
#	$(CC)  -o $(OBJ)  ./*.o -I `pkg-config --libs --cflags gtk+-2.0` -pthread -g
#	rm -rf *.o
 
clean:
	-rm -rf *.o
