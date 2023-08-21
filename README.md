# mini_server_cluster

基于Linux平台下实现的多线程，多路复用封装的实现，涵盖多线程，多端口绑定。

test测试类 目前test中main是服务器主函数

#编译
cd build   
cmake ..  
make  

可执行文件生成再bin目录下main文件
文件生成成功之后，用nc进行远程访问连接
