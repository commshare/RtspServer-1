// PHZ
// 2018-5-15

#ifndef XOP_BUFFER_READER_H
#define XOP_BUFFER_READER_H

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>  
#include <memory>  
#include <mutex>
#include "net/Logger.h"

#define USE_RR_RTMP 1
#include "zlm/Buffer.h"
namespace toolkit
{

  template <class Mtx = recursive_mutex>
  class MutexWrapper {
  public:
	MutexWrapper(bool enable) {
	  _enable = enable;
	}
	~MutexWrapper() {}

	inline void lock() {
	  if (_enable) {
		_mtx.lock();
	  }
	}
	inline void unlock() {
	  if (_enable) {
		_mtx.unlock();
	  }
	}
  private:
	bool _enable;
	Mtx _mtx;
  };

}
namespace xop
{
    
uint32_t readUint32BE(char* data);
uint32_t readUint32LE(char* data);
uint32_t readUint24BE(char* data);
uint32_t readUint24LE(char* data);
uint16_t readUint16BE(char* data);
uint16_t readUint16LE(char* data);
    
class BufferReader
{
public:
  //接收数据回调
  typedef function<void(const toolkit::Buffer::Ptr &buf, struct sockaddr *addr, int addr_len)> onReadCB;
public:	
	static const uint32_t kInitialSize = 4096; //rtmp 4096
    BufferReader(uint32_t initialSize = kInitialSize);
    ~BufferReader();

    uint32_t readableBytes() const
    { return _writerIndex - _readerIndex; }

    uint32_t writableBytes() const
    {  return _buffer->size() - _writerIndex; }

    char* peek() 
    { return begin() + _readerIndex; }

    const char* peek() const
    { return begin() + _readerIndex; }

    const char* findFirstCrlf() const
    {    
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

    const char* findLastCrlf() const
    {    
        const char* crlf = std::find_end(peek(), beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

    void retrieveAll() 
    { 
        _writerIndex = 0; 
        _readerIndex = 0; 
    }

    void retrieve(size_t len)
    {
        if (len <= readableBytes())
        {
            _readerIndex += len;
            if(_readerIndex == _writerIndex)
            {
                _readerIndex = 0;
                _writerIndex = 0;
            }
        }
        else
        {
            retrieveAll();
        }
    }

    void retrieveUntil(const char* end)
    { retrieve(end - peek()); }

    int readFd(int sockfd);
    uint32_t readAll(std::string& data);
    uint32_t readUntilCrlf(std::string& data);

    uint32_t bufferSize() const 
    { return _buffer->size(); }
	    uint32_t size() const  //rtmp
    { 
#if USE_RR_RTMP
		  FLOG() << "BufferReader size() " << _readBuffer->size();
		  return _readBuffer->size();
#else
		  return _buffer->size(); 
#endif
		} 

private:
    char* begin()
    { return &*_buffer->begin(); }

    const char* begin() const
    { return &*_buffer->begin(); }

    char* beginWrite()
    { return begin() + _writerIndex; }

    const char* beginWrite() const
    { return begin() + _writerIndex; }

    std::shared_ptr<std::vector<char>> _buffer;
    size_t _readerIndex = 0;
    size_t _writerIndex = 0;

    static const char kCRLF[];
    static const uint32_t MAX_BYTES_PER_READ = 4096;
    static const uint32_t MAX_BUFFER_SIZE = 10240*100;
private:
  int onRead(int sockfdk, bool isUdp=false);
  void setOnRead(const onReadCB &cb);
public: //TODO g共享指针的复制和引用会增加计数么？
  toolkit::BufferRaw::Ptr _readBuffer;
private:
  atomic<bool> _enableRecv;

  onReadCB _readCB;
  toolkit::MutexWrapper<recursive_mutex> _mtx_event;

};

}

#endif


