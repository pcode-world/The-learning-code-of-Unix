#以'#'开头的行表示注释 
#定义变量VAR，强制赋值为app VAR=test   
#在VAR之前定义的值后面再追加app这个值，这时该变量值扩展为testapp   VAR+=app    
#如果之前VAR没有被定义，则定义并使用testapp；否则使用之前的值。 VAR?=testapp  
# 第一条目标为总的目标,
# 依赖可以是文件(目录)或为其他目标 
# 动作可以是Linux命令，动作的那一行必须以TAB键开头 
#target:   depend1  depend2 depend3 ... 
#[TAB]  action1   
#[TAB ] action2 
#target1: [TAB] action1  [TAB] action2    	
nameserver=wujinlong_server
namecilent=wujinlong_client
pidserver=`ps -aux | grep ${nameserver}`
pidclient=`ps -aux | grep ${namecilent}`
all: gccclient gccserver
	@./${nameserver} -p 9418
	@./${namecilent} -p 9418 -i 111.175.32.85
gccclient:
	@gcc client.c -o ${namecilent}
gccserver:
	@gcc server_pthread.c -o ${nameserver} -lpthread -l sqlite3
shutdown:
	@kill -s SIGUSR1 ${pidclient}
	@kill -s SIGUSR2 ${pidserver}
clean:
	@rm -rf client ${nameserver} ${namecilent}
