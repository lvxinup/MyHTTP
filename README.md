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
