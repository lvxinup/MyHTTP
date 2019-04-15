#include "HttpdServer.hpp"
#include<signal.h>
#include<unistd.h>

static void Usage(std::string proc_)
{
	std::cout << " Usage " << proc_ << " prot " << std::endl;	
}
// ./HttpdServer 8080
int main(int argc,char *argv[]) //argc：IP地址    argv[]：端口
{
	if(argc != 2)
	{
		Usage(argv[0]);
		exit(1);	
	}	
	
	signal(SIGPIPE,SIG_IGN);	
	HttpdServer *serp = new HttpdServer(atoi(argv[1]));
	serp->InitServer();	//建立连接
	serp->Start();		//开始接收数据，通信
	
	delete serp;
	return 0;
}
