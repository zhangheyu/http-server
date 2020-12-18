# http-server
使用C写的，基于gsoap的简单的嵌入式http服务器。


当前端口号默认为８８８８

编译方式(基于Ubuntun编译):

cd httpserver && make -j

编译生成的可执行程序在output目录

./http_server -p port指定要使用的端口，若不指定，默认为8888