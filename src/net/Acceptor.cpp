#include "Acceptor.h"
#include "EventLoop.h"
#include "SocketUtil.h"
#include "Logger.h"

using namespace xop;

//一个acceptor只有一个tcp socket 监听给定ip prot的读写，channel 是读写相关的处理
Acceptor::Acceptor(EventLoop* eventLoop, std::string ip, uint16_t port)
    : _eventLoop(eventLoop)
    , _tcpSocket(new TcpSocket)
{	
    _tcpSocket->create();
	//channel会绑定一个tcpsocket对象
    _acceptChannel.reset(new Channel(_tcpSocket->fd()));
    SocketUtil::setReuseAddr(_tcpSocket->fd());
    SocketUtil::setReusePort(_tcpSocket->fd());
    SocketUtil::setNonBlock(_tcpSocket->fd());
    _tcpSocket->bind(ip, port);
}

Acceptor::~Acceptor()
{
    _eventLoop->removeChannel(_acceptChannel);
    _tcpSocket->close();
}

int Acceptor::listen()
{
  //tcp监听
    if (!_tcpSocket->listen(1024))
    {
        return -1;
    }
	//socket可读，就意味着有新的连接
    _acceptChannel->setReadCallback([this]() { this->handleAccept(); });
    _acceptChannel->enableReading();
    _eventLoop->updateChannel(_acceptChannel);
    return 0;
}

void Acceptor::handleAccept()
{
    int connfd = _tcpSocket->accept();
	FLOG() << "handleAccept::TCP ACCEPT NEW FD " << connfd;
    if (connfd > 0)
    {
        if (_newConnectionCallback)		
        {
            _newConnectionCallback(connfd);
        }
        else
        {
            SocketUtil::close(connfd);
        }
    }
}

