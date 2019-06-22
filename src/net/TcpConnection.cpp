#include "TcpConnection.h"
#include "SocketUtil.h"
#include "net/Logger.h"
using namespace xop;

TcpConnection::TcpConnection(TaskScheduler *taskScheduler, int sockfd)
	: _taskScheduler(taskScheduler)
	, _readBufferPtr(new BufferReader) //tcp connection 1v1 对应_readBufferPtr
	, _writeBufferPtr(new BufferWriter(1000))
	, _channelPtr(new Channel(sockfd))
{
    _isClosed = false;

	//epoll触发TcpConnection的handleRead
    _channelPtr->setReadCallback([this]() { this->handleRead(); });
    _channelPtr->setWriteCallback([this]() { this->handleWrite(); });
    _channelPtr->setCloseCallback([this]() { this->handleClose(); });
    _channelPtr->setErrorCallback([this]() { this->handleError(); });

    SocketUtil::setNonBlock(sockfd);
    SocketUtil::setSendBufSize(sockfd, 512 * 1024);
    SocketUtil::setKeepAlive(sockfd);

    _channelPtr->enableReading();
    _taskScheduler->updateChannel(_channelPtr);
}

TcpConnection::~TcpConnection()
{
    int fd = _channelPtr->fd();
    if (fd > 0)
    {
        SocketUtil::close(fd);
    }
}

void TcpConnection::send(std::shared_ptr<char> data, uint32_t size)
{
    if (_isClosed)
        return;

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _writeBufferPtr->append(data, size);
    }
    
    _taskScheduler->addTriggerEvent([this]{ this->handleWrite(); });    
    return;
}

void TcpConnection::send(const char *data, uint32_t size)
{
    if (_isClosed)
        return;

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _writeBufferPtr->append(data, size);
    }
    
    _taskScheduler->addTriggerEvent([this]{ this->handleWrite(); });
    return;
}

void TcpConnection::close()
{
    if (_isClosed)
        return;

    this->handleClose();
}
//TcpConnection 的handleRead 读取数据然后让rtmp 的connection来从中读取
void TcpConnection::handleRead()
{
    if (_isClosed)
        return;
	//从socket读取数据，现在改成zl的全读出来
    int ret = _readBufferPtr->readFd(_channelPtr->fd());
    if (ret <= 0)
    {
	  FLOG()<<"====READ NO DATA, CLOSE ";
        this->handleClose();
        return;
    }
	//具体的connection 去读刚刚读到的数据
     if (_readCB)
     {
        _taskScheduler->addTriggerEvent([this]{   
            if(_readBufferPtr->size() > 0)
            {   
			  //回调给rtmp的connection
                bool ret = _readCB(shared_from_this(), *_readBufferPtr);
                if (false == ret)
                {
                    this->handleClose();
                }
            }
        });
     }
}

void TcpConnection::handleWrite()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_isClosed)
        return;

    int ret = 0;
    bool empty = false;
    do
    {
        ret = _writeBufferPtr->send(_channelPtr->fd());
        if (ret < 0)
        {
            handleClose();
            return;
        }
        empty = _writeBufferPtr->isEmpty();
    } while (0);

    if (empty)
    {
        if (_channelPtr->isWriting())
        {
            _channelPtr->disableWriting();
            _taskScheduler->updateChannel(_channelPtr);
        }
    }
    else if(!_channelPtr->isWriting())
    {
        _channelPtr->enableWriting();
        _taskScheduler->updateChannel(_channelPtr);
    }
}

void TcpConnection::handleClose()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_isClosed)
    {
        _isClosed = false;
        _taskScheduler->removeChannel(_channelPtr);

        if (_closeCB)
            _closeCB(shared_from_this());

        _disconnectCB(shared_from_this());
    }
}

void TcpConnection::handleError()
{
	this->handleClose();
}
