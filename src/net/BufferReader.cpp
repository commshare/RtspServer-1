// PHZ
// 2018-5-15

#include "BufferReader.h"
#include "Socket.h"
#include <cstring>


#define LOCK_GUARD(mtx) lock_guard<decltype(mtx)> lck(mtx)



using namespace xop;

//for rtmp begin
uint32_t xop::readUint32BE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint32_t value = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    return value;
}

uint32_t xop::readUint32LE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint32_t value = (p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0];
    return value;
}

uint32_t xop::readUint24BE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint32_t value = (p[0] << 16) | (p[1] << 8) | p[2];
    return value;
}

uint32_t xop::readUint24LE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint32_t value = (p[2] << 16) | (p[1] << 8) | p[0];
    return value;
}

uint16_t xop::readUint16BE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint16_t value = (p[0] << 8) | p[1];
    return value; 
}

uint16_t xop::readUint16LE(char* data)
{
    uint8_t* p = (uint8_t*)data;
    uint16_t value = (p[1] << 8) | p[0];
    return value; 
}
//for rtmp end
const char BufferReader::kCRLF[] = "\r\n";

BufferReader::BufferReader(uint32_t initialSize)
    : _buffer(new std::vector<char>(initialSize)), _mtx_event(true)
{
	_buffer->resize(initialSize);

	//默认服务器端
	_enableRecv = true;

}	

BufferReader::~BufferReader()
{
	
}
//返回0，啥都没读到
int BufferReader::onRead(int sockfd, bool isUdp) {
  int ret = 0;
  int sock = sockfd;
  if (!_readBuffer) {
	_readBuffer = std::make_shared<toolkit::BufferRaw>(isUdp ? 1600 : 128 * 1024);
  }
  int nread = 0;
  struct sockaddr peerAddr;
  socklen_t len = sizeof(struct sockaddr);
  while (_enableRecv) {
	do {
	  nread = recvfrom(sock, _readBuffer->data(), _readBuffer->getCapacity() - 1, 0, &peerAddr, &len);
	} while (-1 == nread && UV_EINTR == get_uv_error(true));

	if (nread == 0) {
	  if (!isUdp) {
		//emitErr(SockException(Err_eof, "end of file"));
		FLOG("====EOF ======");
	  }
	  return ret;
	}

	if (nread == -1) {
	  if (get_uv_error(true) != UV_EAGAIN) {
		//onError(pSock);
		FLOG() << " ===UV_EAGAIN ERR !!!";
    }
	  return ret;
}
	ret += nread;
	_readBuffer->data()[nread] = '\0';
	_readBuffer->setSize(nread);

	LOCK_GUARD(_mtx_event);
	_readCB(_readBuffer, &peerAddr, len);
	}
  return 0;
}
void BufferReader::setOnRead(const onReadCB &cb) {
  LOCK_GUARD(_mtx_event);
  if (cb) {
	_readCB = cb;
  }
  else {
	_readCB = [](const toolkit::Buffer::Ptr &buf, struct sockaddr *, int len) {
	  WarnL << "rtmp conn not set readCB len:"<<len;
	};
  }
}
int BufferReader::readFd(int sockfd)
{	
#if USE_RR_RTMP
  return onRead(sockfd);
#else
    uint32_t size = writableBytes();
    if(size < MAX_BYTES_PER_READ) // 重新调整BufferReader大小
    {
        uint32_t bufferReaderSize = _buffer->size();
        if(bufferReaderSize > MAX_BUFFER_SIZE)
        {
            return 0; // close
        }
        
        _buffer->resize(bufferReaderSize + MAX_BYTES_PER_READ);
    }
	//sendto函数 和 recvfrom 函数一般用于UDP协议中,但是如果在 TCP 中 connect 函数调用后也可以用.
    int bytesRead = ::recv(sockfd, beginWrite(), MAX_BYTES_PER_READ, 0);
    if(bytesRead > 0)
    {
        _writerIndex += bytesRead;
    }

    return bytesRead;
#endif
}


uint32_t BufferReader::readAll(std::string& data)
{
    uint32_t size = readableBytes();
    if(size > 0)
    {
        data.assign(peek(), size);
        _writerIndex = 0;
        _readerIndex = 0;
    }

    return size;
}

uint32_t BufferReader::readUntilCrlf(std::string& data)
{
    const char* crlf = findLastCrlf();
    if(crlf == nullptr)
    {
        return 0;
    }

    uint32_t size = crlf - peek() + 2;
    data.assign(peek(), size);
    retrieve(size);

    return size;
}

