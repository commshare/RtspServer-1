#include "rtmpserver/src/xop/RtmpServer.h"
#include "net/EventLoop.h"
//ffmpeg.exe -loglevel verbose -re -i test.h264 -f flv rtmp://127.0.0.1/live/stream

int rtmpsvr_main()
{
    xop::EventLoop eventLoop;  
    xop::RtmpServer server(&eventLoop, "0.0.0.0", 1935);        
    eventLoop.loop();
    
    return 0;
}
