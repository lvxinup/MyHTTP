#ifndef __PROTOCOL_UTIL_HPP__
#define __PROTOCOL_UTIL_HPP__

#include<iostream>
#include<string>
#include<string.h>
#include<strings.h>
#include<sstream>
#include<unordered_map>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/sendfile.h>
#include<sys/wait.h>
#include<map>
#include "Log.hpp"

#define OK 200
#define NOT_FOUND 404
#define BAD_REQUEST 400
#define SERVER_ERROR 500

#define WEB_ROOT "wwwroot"
#define HOME_PAGE "index.html"
#define PAGE_404 "404.html"

#define HTTP_VERSION "http/1.0"

std::unordered_map<std::string,std::string> stuffix_map
{
		{".html","text/html"},
		{".htm","text/html"},
		{".css","text/css"},
		{".js","application/x-javascript"}
};

class ProtocolUtil
{
		public:
				static void MakeKV(std::unordered_map<std::string,std::string> &kv_,std::string &str_)
				{
						std::size_t pos = str_.find(": ");
						if(std::string::npos == pos)
						{
								return;
						}

						std::string k_ = str_.substr(0,pos);
						std::string v_ = str_.substr(pos+2);

						kv_.insert(make_pair(k_,v_));
				}	
				static std::string IntToString(int code)
				{
						std::stringstream ss;
						ss << code;
						
						std::cout << "int to string test" << std::endl;
						return ss.str();
				}
				static std::string CodeToDesc(int code)
				{
						switch(code)
						{
								case 200:
										return "OK";
								case 400:
										return "Bad Request";
								case 404:
										return "Not Found";
								case 500:
										return "Internal Server Error";
								default:
										return "Unknow";
						}
				}
				static std::string SuffixToType(const std::string &suffix_)
				{
						return stuffix_map[suffix_];
				}	
};

class Request
{
		public:
				std::string rq_line;	//请求行
				std::string rq_head;	//请求报头
				std::string blank;	//空行
				std::string rq_text;	//正文
		private:
				std::string method;	//请求方法	
				std::string uri;	//请求URL
				std::string version;	//HTTP协议及版本
				bool cgi;	

				std::string path;	//路径
				std::string param;	//参数

				int resource_size;	//资源长度
				std::string resource_suffix;	//资源后缀 .html
				std::unordered_map<std::string,std::string> head_kv;
				int content_length;	//内容长度

		public:
				Request():blank("\n"), cgi(false), path(WEB_ROOT),resource_size(0),content_length(-1),resource_suffix(".html")
		{}
				std::string &GetParam()	//获取参数的接口
				{
						return param;
				}
				int GetResourceSize()//获取资源大小的接口
				{
						return resource_size;
				}
				void SetResourceSize(int rs_)//资源大小的设定
				{
						resource_size = rs_;	
				}	
				std::string &GetSuffix()//获取资源后缀的接口
				{
						return resource_suffix;
				}
				std::string &SetSuffix(std::string suffix_)
				{
					resource_suffix = suffix_;
				}
				std::string &GetPath()//获取路径的接口
				{
						return path;
				}
				void SetPath(std::string &path_)
				{
					path = path_;
				}	
				void RequestLineParse()
				{
						std::stringstream ss(rq_line);
						ss >> method >> uri >> version;
				}
				void UriParse()	//将资源和参数进行分离并保存在path,param中
				{
						if(strcasecmp(method.c_str(),"GET")==0)//GET方法才会有？
						{
								std::size_t pos_ = uri.find('?');//有？则带参
								//带参解析：前半部分为路径path,依旧放在uri中;后半部分作为参数保存到param中
								if(std::string::npos != pos_)
								{
										cgi = true;
										path += uri.substr(0, pos_);
										param = uri.substr(pos_+1);
								}
								else
								{	//找不到uri,path中不含参数
										path += uri;
								}
						}
						//post方法，path中比较干净，不含参数
						else
						{	//无？说明没有参数，则全部为路径放进path中
								//path中默认有一个值，wwwroot表明当前服务器主目录
								path += uri;
						}
						if(path[path.size() -1] == '/'){
								path += HOME_PAGE;//以/结尾则要拼上首页，每个目录下都有默认首页
						}		
				}
				bool RequestHeadParse()//请求头解析
				{
						int start = 0;
						while(start < rq_head.size())
						{
								std::size_t pos = rq_head.find('\n',start);
								if(std::string::npos == pos)
								{
										break;	
								}

								std::string sub_string_ = rq_head.substr(start,pos - start);
								if(!sub_string_.empty())
								{
										LOG(INFO,"substr is not empty!");
										ProtocolUtil::MakeKV(head_kv,sub_string_);
								}
								else
								{
										LOG(INFO,"substr is empty!");
										break;
								}
								start = pos + 1;
						}			
						return true;
				}
				int GetContentLength()
				{
						std::string cl_ = head_kv["Content-Length"];
						if(!cl_.empty())
						{
								std::stringstream ss(cl_);
								ss >> content_length;
						}	
						return content_length;
				}
				bool IsMethodLegal()
				{
						if(strcasecmp(method.c_str(),"GET") == 0 ||\
										(cgi = (strcasecmp(method.c_str(),"POST") == 0))){
								return true;
						}

						return false;
				}
				bool IsPathLegal()
				{
						struct stat st;
						if(stat(path.c_str(),&st)<0)
						{
								LOG(WARNING,"path not found!");
								return false;
						}

						if(S_ISDIR(st.st_mode))
						{
								path += "/";
								path +=HOME_PAGE;
						}
						else
						{
								if((st.st_mode & S_IXUSR ) ||\
								   (st.st_mode & S_IXGRP ) ||\
								   (st.st_mode & S_IXOTH ))
								{
										cgi = true;
								}	
						}

						resource_size = st.st_size;
						std::size_t pos = path.rfind(".");
						if(std::string::npos != pos)
						{	
								resource_suffix = path.substr(pos);
						}
						return true;
				}
				bool IsNeedRecvText()
				{
						if(strcasecmp(method.c_str(),"POST") == 0)
						{
								return true;
						}
						return false;
				}
				bool IsCgi()
				{
						return cgi;	
				}
				~Request()
				{}
};

class Response{
		public:
				std::string rsp_line;
				std::string rsp_head;
				std::string blank;
				std::string rsp_text;
				int fd;
				int code;
		public:
				Response():blank("\n"), code(OK)
		{}
				void MakeStatusLine()
				{
						rsp_line = HTTP_VERSION;
						rsp_line += " ";
						rsp_line += ProtocolUtil::IntToString(code);
						rsp_line += " ";
						rsp_line += ProtocolUtil::CodeToDesc(code);
						rsp_line += "\n";
							
						std::cout << "make status line success!" << std::endl;
				}

				void MakeResponseHead(Request *&rq_)
				{
						rsp_head = "Conten-Length: ";
						rsp_head += ProtocolUtil::IntToString(rq_->GetResourceSize());
						rsp_head += "\n";
						rsp_head += "Content-Type: ";
						std::string suffix_ = rq_->GetSuffix();
						rsp_head += ProtocolUtil::SuffixToType(suffix_);
						rsp_head += "\n";	
			
						std::cout << "make response head success!" << std::endl;
				}
				void OpenResource(Request *&rq_)
				{
						std::string path_ = rq_->GetPath();
						fd = open(path_.c_str(),O_RDONLY);
							
						std::cout << "open resource success!" << std::endl;
				}
				~Response()
				{
						if(fd >= 0)
						{
								close(fd);
						}
				}
};

class Connect{
		private:
				int sock;
		public:
				Connect(int sock_):sock(sock_)
		{}
				int RecvOneLine(std::string &line_)
				{
						char c = 'X';
						while(c != '\n'){
								ssize_t s = recv(sock, &c, 1, 0);
								if(s > 0){
										if( c == '\r' ){
												recv(sock, &c, 1, MSG_PEEK);
												if( c== '\n' ){
														recv(sock, &c, 1, 0);
												}
												else{
														c = '\n';
												}
										}

										line_.push_back(c);
								}
								else{
										break;
								}
						}
						return line_.size();
				}
				void RecvRequestHead(std::string &head_)
				{
						head_ = "";
						std::string line_;
						while(line_ != "\n")
						{		
								line_ = "";
								RecvOneLine(line_);
								head_ += line_;
						}	
				}
				void RecvRequestText(std::string &text_,int len_,std::string param_)
				{
						char c_;
						int i_ = 0;
						while( i_ < len_)
						{
								recv(sock,&c_,1,0);
								text_.push_back(c_);
                i_++;
						}	

						param_ = text_ ;
				}
				void SendResponse(Response *&rsp_,Request *&rq_,bool cgi_)
				{
						std::string &rsp_line_ = rsp_->rsp_line;
						std::string &rsp_head_ = rsp_->rsp_head;
						std::string &blank_ = rsp_->blank;

						send(sock,rsp_line_.c_str(),rsp_line_.size(),0);
						send(sock,rsp_head_.c_str(),rsp_head_.size(),0);
						send(sock,blank_.c_str(),blank_.size(),0);

						if(cgi_)
						{
								std::string &rsp_text_ = rsp_->rsp_text;
								send(sock,rsp_text_.c_str(),rsp_text_.size(),0);			
						}
						else
						{
								int &fd = rsp_->fd;
								sendfile(sock,fd,NULL,rq_->GetResourceSize());
						}
				}
				~Connect()
				{
						if(sock >= 0){
								close(sock);
						}
				}
};

class Entry{
		public:
				static void ProcessNonCgi(Connect *&conn_,Request *&rq_,Response *&rsp_)
				{
						int &code_ = rsp_->code;
						std::cout << "process is noncgi" << std::endl;
						rsp_->MakeStatusLine();
						rsp_->MakeResponseHead(rq_);
						rsp_->OpenResource(rq_);
						conn_->SendResponse(rsp_,rq_,false);	
				}
				static void ProcessCgi(Connect *&conn_,Request *&rq_,Response *&rsp_)
				{
						
						std::cout << "process is cgi" << std::endl;
						int &code_ = rsp_->code;
						int input[2];
						int output[2];
						std::string &param_ = rq_->GetParam();
						std::string &rsp_text_ = rsp_->rsp_text;

						pipe(input);
						pipe(output);

						pid_t id = fork();
						if(id < 0 )
						{
								code_ = NOT_FOUND;
								return;
						}	
						else if(id == 0 )
						{	//child
								close(input[1]);
								close(output[0]);

								const std::string &path_ = rq_->GetPath();	//获取路径

								std::string cl_env_ ="Content-Length=";	//环境变量
								cl_env_ += ProtocolUtil::IntToString(param_.size());
								putenv((char*)cl_env_.c_str());	//导出环境变量

								dup2(input[0],0);
								dup2(output[1],1);
								execl(path_.c_str(),path_.c_str(),NULL);	//执行谁	怎么执行
								exit(1);
						}
						else
						{	//parent
								close(input[0]);
								close(output[1]);

								size_t size_ = param_.size();
								size_t total_ = 0;//期望写入
								size_t curr_ = 0;//实际写入
								const char *p_ = param_.c_str();
								while(total_ < size_ &&\
												(curr_ = write(input[1],p_ + total_,size_ - total_))>0)
								{//写入所有参数
										total_ += curr_;
								}
								
								char c;
								while(read(output[0],&c,1) > 0)
								{
									rsp_text_.push_back(c);
								}
								waitpid(id,NULL,0);
						
								close(input[1]);	
								close(output[0]);
							
								rsp_->MakeStatusLine();
								rq_->SetResourceSize(rsp_text_.size());
								rsp_->MakeResponseHead(rq_);

								conn_->SendResponse(rsp_,rq_,true);
						}
				}
				static int ProcessResponse(Connect *&conn_,Request *&rq_,Response *&rsp_)
				{
					if(rq_->IsCgi())
					{
						ProcessCgi(conn_,rq_,rsp_);
					}
					else
					{
						ProcessNonCgi(conn_,rq_,rsp_);
					}
				}
				static void Process404(Connect *&conn_,Request *&rq_,Response *&rsp_)
				{
					std::string path_ = WEB_ROOT;
					path_ += "/";
					path_ += PAGE_404;
					struct stat st;
					stat(path_.c_str(),&st);
				
					rq_->SetResourceSize(st.st_size);
					rq_->SetSuffix(".html");
					rq_->SetPath(path_);
					
					ProcessNonCgi(conn_,rq_,rsp_);
				}
				static void HandlerError(Connect *&conn_,Request *&rq_,Response *&rsp_)
				{
					int &code_ = rsp_->code;
					switch(code_)
					{
						case 400:
							break;
						case 404:
							Process404(conn_,rq_,rsp_);
							break;
						case 500:
							break;
						case 503:
							break;
					}		
				}
				static int HandlerRequest(int sock_)
				{
						Connect *conn_ = new Connect(sock_);
						Request *rq_ = new Request();
						Response *rsp_ = new Response();

						int &code_ = rsp_->code;

						conn_->RecvOneLine(rq_->rq_line);
						rq_->RequestLineParse();
						if( !rq_->IsMethodLegal() ){
								conn_->RecvRequestHead(rq_->rq_head);
								code_ = BAD_REQUEST;
								goto end;
						}
						rq_->UriParse();
						
						if(!rq_->IsPathLegal())
						{
								conn_->RecvRequestHead(rq_->rq_head);
								code_ = NOT_FOUND;
								goto end;
						}	
						
						LOG(INFO,"request path is OK!");
					
						conn_->RecvRequestHead(rq_->rq_head);
						if(rq_->RequestHeadParse())
						{
							LOG(INFO,"parse head done");
						}
						else 
						{
							code_ = BAD_REQUEST;
							goto end;
						}

						if(rq_->IsNeedRecvText())
						{
							conn_->RecvRequestText(rq_->rq_text,rq_->GetContentLength(),rq_->GetParam());
						}
						//request recv done!
						ProcessResponse(conn_,rq_,rsp_);
end:
						if(code_ != OK){
							HandlerError(conn_,rq_,rsp_);
						}
						delete conn_;
						delete rq_;
						delete rsp_;
						return code_;
				}
};                                                      

#endif		
