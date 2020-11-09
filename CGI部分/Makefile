CC=arm-none-linux-gnueabi-gcc
AFLAGS=-Wall -g -lm -L ./lib -lsqlite3
OBJS=login.cgi set_wifi.cgi set_1_m0_env.cgi set_msg.cgi ctl_a9_dev.cgi set_2_m0_env.cgi ctl_1_m0_dev.cgi ctl_2_m0_dev.cgi take_photo.cgi env_1_m0_info.cgi env_2_m0_info.cgi \
goods_1_m0_sql.cgi goods_2_m0_sql.cgi show_photo.cgi

run:$(OBJS)

$(OBJS):%.cgi:%.c
	$(CC) $(AFLAGS) $< cgic.c -o $@
.PHONY:clean
clean:
	rm *.cgi

