<<<<<<< HEAD
# games 游戏

自己练手写的一些小游戏

1.仿经典windows扫雷。2016年1月。 https://github.com/liusaint/games/tree/master/mine

2.九宫格数独游戏。生成数独算法。2016年4月。https://github.com/liusaint/games/tree/master/soduku

3.简易贪吃蛇。2016年1月。https://github.com/liusaint/games/tree/master/snake

4.2048游戏。2016年5月。https://github.com/liusaint/games/tree/master/2048
=======
项目名称
----------------------------
9934小游戏-在线游戏平台

项目目标
--------------------------------------
搭建一个小型的HTTP服务器，服务器能够提供在线简历展示、计算器、在线游戏等服务。

项目功能
--------------------------------------
1.服务器对请求进行响应，错误资源会返回404页面。

2.HTTP服务器落地页为个人在线简历，展示个人信息、学习经历、项目经验、专业技能等。

3.根据请求访问资源路径，从url中提取参数，进行简单计算，并返回给用户。

4.服务器加入众多小游戏，如：2048、推箱子、贪吃蛇、别踩白块、扫雷、数独游戏等，能够实现在线游戏平台。(游戏涉及前端，所以借鉴网上资源)


提示：运行服务器前要切换到根模式下关闭防火墙
--------------------------------------



效果图
--------------------------------------


运行结果
![](https://github.com/lvxinup/githttp/blob/master/QQ%E5%9B%BE%E7%89%8720190316114001.png)
在线简历
![](https://github.com/lvxinup/githttp/blob/master/QQ%E5%9B%BE%E7%89%8720190316114011.png)
![](https://github.com/lvxinup/githttp/blob/master/QQ%E5%9B%BE%E7%89%8720190316114016.png)
计算器
![](https://github.com/lvxinup/githttp/blob/master/QQ%E5%9B%BE%E7%89%8720190316114020.png)
2048游戏
![](https://github.com/lvxinup/MyHTTP/blob/master/2048.png)
别踩白块
![](https://github.com/lvxinup/MyHTTP/blob/master/%E5%88%AB%E8%B8%A9%E7%99%BD%E5%9D%97.png)
扫雷
![](https://github.com/lvxinup/MyHTTP/blob/master/%E6%89%AB%E9%9B%B7.png)
推箱子
![](https://github.com/lvxinup/MyHTTP/blob/master/%E6%8E%A8%E7%AE%B1%E5%AD%90.png)
数独游戏
![](https://github.com/lvxinup/MyHTTP/blob/master/%E6%95%B0%E7%8B%AC%E6%B8%B8%E6%88%8F.png)
贪吃蛇
![](https://github.com/lvxinup/MyHTTP/blob/master/%E8%B4%AA%E5%90%83%E8%9B%87.png)

项目流程
--------------------------------------
### 启动服务器
1. 检查输入格式是否正确。(IP地址+端口号 即192.168.43.177:8080)

2. 构建一个HttpdServer对象，然后初始化服务器，为套接字绑定地址信息，进入监听状态。同时创建含有5个线程的线程池，并对线程池进行初始化。

3. 线程池初始化互斥量和条件变量，并维护一个任务队列，5个线程创建好后先线程分离，再依次进行互斥量的获取，判断队列是否有任务，有任务则分配一个线程处理任务，同时thread_total_num--，无任务则挂起等待。

4. 此时服务器准备就绪，等待客户端发起HTTP请求进行处理。

### 服务端接收HTTP请求
1. 服务器启动后，当 HTTP 请求到来时，HttpdServer 对象调用 accept 函数接收 HTTP 请求，将 HTTP 请求封装成任务(t.SetTask(sock_,Entry::HandlerRequest))，放入任务队列中，同时唤醒一个线程来处理任务，调用 ProtocolUtil.hpp中Entry::HandlerRequest 来处理任务。

2. HandlerRequest 对 HTTP 请求进行解析，调用 RecvOneLine 接收请求行，将所有的"\n","\r\n","\r"都转换为 \n。(因为在不同的环境下换行的表示方法不一样，所以将不同环境下的换行都转换为"\n"，保持统一)。

Unix系统里，每行结尾只有“<换行>”，即“\n”；Windows系统里面，每行结尾是“<回车><换行>”，即“\r\n”；Mac系统里，每行结尾是“<回车>”。一个直接后果是，Unix/Mac系统下的文件在Windows里打开的话，所有文字会变成一行；而Windows里的文件在Unix/Mac下打开的话，在每行的结尾可能会多出一个符号^M（在Unix下多出一个\r，在Mac下多出一个\n）。

3. 使用stringstream将接收的一行依次写入 method 、 uri 、 version 中。(ss >> method >> uri >> version)。

4. 判断 method 、 path 是否合法，调用 IsMethodLegal 判断请求方法是否正确(strcasecmp函数)，不正确返回错误码。然后调用 UriParse 函数对URL进行解析，如果为 GET 方法，则 URL 以?为分隔，左边为资源路径，右边为参数；如果为POST方法，直接将URL设置为资源路径。(这里如果找不到URL，j即path中不含参数，则将主页返回给用户，这里的主页设置为个人在线简历)

5. 检查path合法性,资源不存在将状态码code记录为404,并跳转至end。

6. 提取出资源路径后，调用IsPathLegal判断资源路径是否合法(stat函数，获取指定文件的相关信息，并将其写入buf所指向的区域)，同时判断是否为CGI请求。

7. 此时调用RecvRequestHead ，接收请求报头，并以"\n" 为分隔，对 substr 的每行调用 MAKE_KV 将请求报头的信息，放入unordered_map<std::string,std::string>中，此时 kv 是 key,value 的数据词典，存储的是 content-type 、content-length 等报头信息。

8. 通过IsNeedRecvText函数，判断请求方法是否为POST来判断是否需要接收请求正文，然后将请求正文放入text中.

9. 至此，服务器完成对HTTP请求的处理，开始构建响应。

### 服务端构建HTTP响应并返回

通过 ProcessResponse 函数将响应分为 CGI 和 非CGI两种，if分流。(这里的 CGI 判断是在判断资源路径是否合法时，将bool设置为true/false)

## 服务器构建 CGI 响应

1. 调用 rq_->GetParam() 获取参数，POST方法的参数在请求正文中。

2. 创建两条匿名管道 input 和 output 管道，创建子进程，并使用 if 分流。

3. 子进程接收父进程传递的参数,子进程处理 CGI 请求，调用 exec 函数族执行 path 路径所指的文件，处理完成后将处理结果返回给父进程。(调用 exec 函数族,替换数据区和代码段,就无法找到操作描述符了(文件描述符表不会被替换,但是你无法知道那个文件描述符是你想要的),所以要先将管道描述符重定向到0,1,还要将参数的长度提前以自定义环境变量的形式进行保存)

4. 父进程将参数 param 全部传入 input 管道,并等待子进程处理完毕。

5. 子进程执行完毕后再通过管道 output 将处理结果反馈给父进程,父进程接收后用来构建成响应正文。

## 服务器构建非 CGI 响应

1. 构建响应首行,默认版本为HTTP/1.0 ,状态码是之前的 code ,状态码描述是 code 所对应的描述。

2. 构建响应报头,本项目中只构建 Content-Length 和 Content-Type,这两个的值之前都已经 URL 解析中记录过了,在 resource_size 和 suffix 变量中。

3. 打开 path 所指的资源,将资源内容作为响应正文。

4. 发送时调用 send 依次向对端 socket 发送首行,报头,空行,正文,正文的长度用 resource_size 控制.

>>>>>>> c9d641e1296f6e0508dd50fad124a05bfd65a41e
