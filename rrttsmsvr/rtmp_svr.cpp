#include <stdint.h>
#include <thread>
#include <memory>
#include <iostream>
#include <string>
//#include "Util/onceToken.h"

////////////RTMP����������///////////

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
  int clients = 0; // ��¼��ǰ�ͻ�������
  std::string ip = "127.0.0.1";//xop::NetInterface::getLocalIPAddress(); //��ȡ����ip��ַ
  std::string rtmpUrl;

  std::shared_ptr<xop::EventLoop> eventLoop(new xop::EventLoop());
  xop::RtmpServer server(eventLoop.get(), ip, rtmpPort);  //����һ��RTSP������
  rtmpUrl = "rtsp://" + ip + "/" + session->getRtspUrlSuffix();

  xop::MediaSession* session = xop::MediaSession::createNew("live"); //����һ��ý��Ự, url: rtsp://ip/live
  rtmpSrv->start<RtmpSession>(rtmpPort);//Ĭ��1935
   // �������Ƶ����ý��Ự, track0:h264, track1:aac
  session->addMediaSource(xop::channel_0, xop::H264Source::createNew());
  session->addMediaSource(xop::channel_1, xop::AACSource::createNew(44100, 2));


  eventLoop->loop(); //���߳����� RtmpServer 
  getchar();
  return 0;
}

