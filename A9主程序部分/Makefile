#CC=gcc
CC=arm-none-linux-gnueabi-gcc
AFLAGS=-Wall -c -g
LDFLAGS= -lpthread -L ./lib -lsqlite3
#LDFLAGS= -lpthread
OBJS=main.o data_global.o pthread_transfer.o pthread_analysis.o \
link_list.o pthread_client_request.o pthread_buzzer.o pthread_led.o \
pthread_camera.o pthread_sqlite.o sqlite_link_list.o pthread_infrared.o \
pthread_refresh.o pthread_sms.o pthread_uart_cmd.o uart_cache.o 

storage_mamage :$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
$(OBJS):%.o:%.c
	$(CC) $(AFLAGS) $< -o $@

.PHONY:clean
clean:
	rm *.o storage_mamage  #warehouse.db

