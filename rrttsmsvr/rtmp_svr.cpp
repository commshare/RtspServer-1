#include <stdint.h>
#include <thread>
#include <memory>
#include <iostream>
#include <string>
//#include "Util/onceToken.h"

////////////RTMP服务器配置///////////

//namespace Rtmp {
//#define RTMP_FIELD "rtmp."
//#define RTMP_PORT 1935
//  const std::string kPort = RTMP_FIELD"port";
//  onceToken token1([]() {
//	mINI::Instance()[kPort] = RTMP_PORT;
//	}, nullptr);
//} //namespace RTMP


void rtmpsvr()
{
  uint16_t rtmpPort = 1935;// mINI::Instance()[Rtmp::kPort];
  int clients = 0; // 记录当前客户端数量
  std::string ip = "127.0.0.1";//xop::NetInterface::getLocalIPAddress(); //获取网卡ip地址
  std::string rtmpUrl;

  std::shared_ptr<xop::EventLoop> eventLoop(new xop::EventLoop());
  xop::RtmpServer server(eventLoop.get(), ip, rtmpPort);  //创建一个RTSP服务器
  rtmpUrl = "rtsp://" + ip + "/" + session->getRtspUrlSuffix();

  xop::MediaSession* session = xop::MediaSession::createNew("live"); //创建一个媒体会话, url: rtsp://ip/live
  rtmpSrv->start<RtmpSession>(rtmpPort);//默认1935
   // 添加音视频流到媒体会话, track0:h264, track1:aac
  session->addMediaSource(xop::channel_0, xop::H264Source::createNew());
  session->addMediaSource(xop::channel_1, xop::AACSource::createNew(44100, 2));


  eventLoop->loop(); //主线程运行 RtmpServer 
  getchar();
  return 0;
}

