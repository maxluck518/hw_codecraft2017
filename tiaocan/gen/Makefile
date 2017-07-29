 #$@表示目标，$^表示所有依赖，$<表示依赖中的第一个
CPPFLAGS= -Iinclude
CFLAGS= -g
LDFLAGS= 
CC=g++
#CC=arm-linux-gcc

src = $(wildcard *.cpp)
obj = $(patsubst %.c,%.o,$(src))
target = app

$(target):$(obj)
	$(CC) $^ $(LDFLAGS) -o $@

%.o:%.c
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

.PHONY:clean
	#彻底清除过程生成文件
clean:
	-rm -f *.o
	-rm -f $(target)
