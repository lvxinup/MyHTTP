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
1.检查输入格式是否正确。(IP地址+端口号 即192.168.43.177:8080)

2.构建一个HttpdServer对象，然后初始化服务器，为套接字绑定地址信息，进入监听状态。同时创建含有5个线程的线程池，并对线程池进行初始化。

3.线程池初始化互斥量和条件变量，并维护一个任务队列，5个线程创建好后先线程分离，再依次进行互斥量的获取，判断队列是否有任务，有任务则分配一个线程处理任务，同时thread_total_num--，无任务则挂起等待。

4.此时服务器准备就绪，等待客户端发起HTTP请求进行处理。

### 服务端接收HTTP请求
1.服务器启动后，当HTTP请求到来时，HttpdServer对象调用accept函数接收HTTP请求，将HTTP请求封装成任务(t.SetTask(sock_,Entry::HandlerRequest))，放入任务队列中，同时唤醒一个线程来处理任务，调用ProtocolUtil.hpp中Entry::HandlerRequest来处理任务。

2.HandlerRequest对HTTP请求进行解析，调用RecvOneLine接收请求行，将所有的"\n","\r\n","\r"都转换为\n。(因为在不同的环境下换行的表示方法不一样，所以将不同环境下的换行都转换为"\n"，保持统一)。

3.将接收的一行依次写入 method 、 uri 、 version中。(ss >> method >> uri >> version)。

4.判断 method 、 path 是否合法，调用 IsMethodLegal 判断请求方法是否正确，不正确返回错误码。然后调用 UriParse 函数对URL进行解析，如果为 GET 方法，则 URL 以?为分隔，左边为资源路径，右边为参数；如果为POST方法，直接将URL设置为资源路径。

5.提取出资源路径后，调用IsPathLegal判断资源路径是否合法，同时判断是否为CGI请求。

6.此时调用RecvRequestHead ，接收请求报头，并以"\n" 为分隔，对substr的每行调用 MAKE_KV 将请求报头的信息，放入unordered_map<std::string,std::string>中，此时 kv 是key,value的数据词典，存储的是content-type、content-length等报头信息。

7.通过IsNeedRecvText函数，判断请求方法是否为POST来判断是否需要接收请求正文，然后将请求正文放入text中，到这里服务器完成对HTTP请求的处理。
