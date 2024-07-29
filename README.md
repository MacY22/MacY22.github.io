# tinyRedis 项目简介

本项目是在学习 redis 期间，为了更加深刻了解 redis 运行过程所写。项目框架参考 [https://build-your-own.org/redis/]()。主要完成工作如下：

- 数据结构。本项目主要涉及的数据结构有哈希表，跳表，双链表，堆等。其中哈希表采用链式哈希，并完成了扩容机制。跳表用于 zset 命令，用于维护有序数据。堆用于对数据设置过期时间。

- 网络部分。本项目服务端部分采用 epoll 多路复用，接受客户端命令-处理命令-返回结果这一流程都是单线程。

- 线程池。线程池主要用于对大 key 进行异步删除，防止阻塞主线程。

## 使用方法

```shell
mkdir build
cd build
cmake ..
make
./tinyRedis_server &
```

## 命令示例

- 无序数据相关：set, get, del
```
set key value
get key
del key
```
- 有序数据相关：zadd, zrem, zscore, zquery
```
zadd key score name
zrem key name
zscore key name
zquery key score name offset limit
```
- 设置过期时间：pexpire, pttl
```
pexpire key ttl_ms
pttl key
```

## 项目不足

- 数据结构编写中，接口定义不明确
- 网络部分没有采用面向对象编程，不便更改