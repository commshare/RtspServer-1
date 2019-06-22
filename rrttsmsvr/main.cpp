#include "rtsp_svr.h"
#include "rtmp_server.h"
int main(int argc, char **argv)
{
 // startRtspServer(argc,argv);
  rtmpsvr_main();
}

/*
zhangbin@DESKTOP-D724TVQ MINGW64 /e/YY/RtspServer-1 (master)
$ ffmpeg.exe -re -i test.h264 -f flv rtmp://127.0.0.1/live/stream
ffmpeg version N-94077-g01d8c72b95 Copyright (c) 2000-2019 the FFmpeg developers
  built with gcc 9.1.1 (GCC) 20190621
  configuration: --enable-gpl --enable-version3 --enable-sdl2 --enable-fontconfig --enable-gnutls --enable-iconv --enable-libass --enable-libdav1d --enable-libbluray --enable-libfreetype --enable-libmp3lame --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libopenjpeg --enable-libopus --enable-libshine --enable-libsnappy --enable-libsoxr --enable-libtheora --enable-libtwolame --enable-libvpx --enable-libwavpack --enable-libwebp --enable-libx264 --enable-libx265 --enable-libxml2 --enable-libzimg --enable-lzma --enable-zlib --enable-gmp --enable-libvidstab --enable-libvorbis --enable-libvo-amrwbenc --enable-libmysofa --enable-libspeex --enable-libxvid --enable-libaom --enable-libmfx --enable-amf --enable-ffnvcodec --enable-cuvid --enable-d3d11va --enable-nvenc --enable-nvdec --enable-dxva2 --enable-avisynth --enable-libopenmpt
  libavutil      56. 29.100 / 56. 29.100
  libavcodec     58. 53.100 / 58. 53.100
  libavformat    58. 28.100 / 58. 28.100
  libavdevice    58.  7.100 / 58.  7.100
  libavfilter     7. 55.100 /  7. 55.100
  libswscale      5.  4.101 /  5.  4.101
  libswresample   3.  4.100 /  3.  4.100
  libpostproc    55.  4.100 / 55.  4.100
Input #0, h264, from 'test.h264':
  Duration: N/A, bitrate: N/A
	Stream #0:0: Video: h264 (High), yuv420p(progressive), 960x408 [SAR 272:273 DAR 640:273], 30 fps, 30 tbr, 1200k tbn, 60 tbc
Stream mapping:
  Stream #0:0 -> #0:0 (h264 (native) -> flv1 (flv))
Press [q] to stop, [?] for help
Output #0, flv, to 'rtmp://127.0.0.1/live/stream':
  Metadata:
	encoder         : Lavf58.28.100
	Stream #0:0: Video: flv1 (flv) ([2][0][0][0] / 0x0002), yuv420p, 960x408 [SAR 272:273 DAR 640:273], q=2-31, 200 kb/s, 30 fps, 1k tbn, 30 tbc
	Metadata:
	  encoder         : Lavc58.53.100 flv
	Side data:
	  cpb: bitrate max/min/avg: 0/0/200000 buffer size: 0 vbv_delay: -1
frame=    7 fps=0.0 q=5.5 size=      71kB time=00:00:00.20 bitrate=2884.2kbits/sframe=   22 fps= 22 q=7.5 size=     110kB time=00:00:00.70 bitrate=1287.3kbits/sav_interleaved_write_frame(): Unknown error
[flv @ 000001ff555084c0] Failed to update header with correct duration.
[flv @ 000001ff555084c0] Failed to update header with correct filesize.
Error writing trailer of rtmp://127.0.0.1/live/stream: Error number -10054 occurred
frame=   37 fps= 25 q=4.4 Lsize=     239kB time=00:00:01.20 bitrate=1629.4kbits/s speed=0.813x
video:244kB audio:0kB subtitle:0kB other streams:0kB global headers:0kB muxing overhead: unknown
Conversion failed!

zhangbin@DESKTOP-D724TVQ MINGW64 /e/YY/RtspServer-1 (master)
$

*/