﻿// PHZ
// 2018-5-15

#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Logger.h"
#include <cstdio>  

using namespace xop;
using namespace std;

TcpServer::TcpServer(EventLoop* eventLoop, std::string ip, uint16_t port)
    : _eventLoop(eventLoop),
      _acceptor(new Acceptor(eventLoop, ip, port))
{
    _ip = ip;
    _port = port;

	
	//收到新链接之后触发
    _acceptor->setNewConnectionCallback([this](SOCKET sockfd) { 
	  FLOG()<<"setNewConnectionCallback NEW CONN FD "<< sockfd;
	    //使用这个fd，创建新tcp conn，由rtmp具体实现
        TcpConnection::Ptr tcpConn = this->newConnection(sockfd);
        if (tcpConn)
        {
		    //tcp server 管理 rtmp 链接
            this->addConnection(sockfd, tcpConn);
			//断链回调
            tcpConn->setDisconnectCallback([this] (TcpConnection::Ptr conn){ 
                    auto taskScheduler = conn->getTaskScheduler();
                    int sockfd = conn->fd();
                    if (!taskScheduler->addTriggerEvent([this, sockfd] {this->removeConnection(sockfd); }))
                    {
					  FLOG() << "setDisconnectCallback removeConnection " << sockfd;
                        taskScheduler->addTimer([this, sockfd]() {this->removeConnection(sockfd); return false;}, 1);
                    }
            });
        }
    });
}

TcpServer::~TcpServer()
{
	
}

int TcpServer::start()
{
	return _acceptor->listen();
}

TcpConnection::Ptr TcpServer::newConnection(SOCKET sockfd)
{
  //创建了一个tcp conn
	return std::make_shared<TcpConnection>(_eventLoop->getTaskScheduler().get(), sockfd);
}

void TcpServer::addConnection(SOCKET sockfd, TcpConnection::Ptr tcpConn)
{
	std::lock_guard<std::mutex> locker(_conn_mutex);
	_connections.emplace(sockfd, tcpConn);
}

void TcpServer::removeConnection(SOCKET sockfd)
{
	std::lock_guard<std::mutex> locker(_conn_mutex);
	_connections.erase(sockfd);
}
