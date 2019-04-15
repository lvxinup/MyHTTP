#ifndef __HTTPD_SERVER_HPP__
#define __HTTPD_SERVER_HPP__

#include<pthread.h>
#include "ProtocolUtil.hpp"
#include "ThreadPool.hpp"

 class HttpdServer
{
	private:
		int listen_sock;	//socket
		int port;		//端口号
		ThreadPool *tp;
	public:
		HttpdServer(int port_):port(port_),listen_sock(-1),tp(NULL)
		{}	//构造函数
		void InitServer()	//服务器初始化
		{
			listen_sock = socket(AF_INET,SOCK_STREAM,0);
			if(listen_sock < 0)
			{	
				LOG(ERROR,"create socket error!");
				exit(2);	
			}
			
			int opt_ = 1;
			setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt_,sizeof(opt_));

			struct sockaddr_in local_;
			local_.sin_family = AF_INET;
			local_.sin_port = htons(port);
			local_.sin_addr.s_addr = INADDR_ANY;

			if(bind(listen_sock,(struct sockaddr*)&local_,sizeof(local_))<0)	//为套接字绑定地址信息
			{
				LOG(ERROR,"bind socket error!");
				exit(3);	
			}
			
			if(listen(listen_sock,5)<0)	//开始监听
			{
				LOG(ERROR,"listen socket error!");				    
				exit(4);
			}
		
			LOG(INFO,"InitServer success!");
			tp = new ThreadPool;
			tp->initThreadPool();
		}
		
		void Start()
		{
			LOG(INFO,"Start Server begin!");
			for(;;)
			{	
				struct sockaddr_in peer_;
				socklen_t len_ = sizeof(peer_);
				int sock_ = accept(listen_sock,(struct sockaddr*)&peer_,&len_);
						//建立连接之后，开始接收数据，进行通信

				if(sock_<0)
				{	
					LOG(WARNING,"accept error!");
					continue;
				}
				Task t;
				t.SetTask(sock_,Entry::HandlerRequest);
				tp->PushTask(t);
			}
		}
		
		~HttpdServer()
		{
			if(listen_sock != -1)
			{
				close(listen_sock);
			}
			port = -1;
		}
};

#endif
