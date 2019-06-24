#include "rtmp/MediaSource.h"
#include "zlm/utils.h"
namespace toolkit
{

  void MediaInfo::parse(const string& url) {
	//string url = "rtsp://127.0.0.1:8554/live/id?key=val&a=1&&b=2&vhost=vhost.com";
	auto schema_pos = url.find("://");
	if (schema_pos != string::npos) {
	  _schema = url.substr(0, schema_pos);
	}
	else {
	  schema_pos = -3;
	}
	auto split_vec = split(url.substr(schema_pos + 3), "/");
	if (split_vec.size() > 0) {
	  auto vhost = split_vec[0];
	  auto pos = vhost.find(":");
	  if (pos != string::npos) {
		_host = _vhost = vhost.substr(0, pos);
		_port = vhost.substr(pos + 1);
	  }
	  else {
		_host = _vhost = vhost;
	  }
	}
	if (split_vec.size() > 1) {
	  _app = split_vec[1];
	}
	if (split_vec.size() > 2) {
	  string steamid;
	  for (int i = 2; i < split_vec.size(); ++i) {
		steamid.append(split_vec[i] + "/");
	  }
	  if (steamid.back() == '/') {
		steamid.pop_back();
	  }
	  auto pos = steamid.find("?");
	  if (pos != string::npos) {
		_streamid = steamid.substr(0, pos);
		_param_strs = steamid.substr(pos + 1);
		_params = Parser::parseArgs(_param_strs);
		if (_params.find(VHOST_KEY) != _params.end()) {
		  _vhost = _params[VHOST_KEY];
		}
	  }
	  else {
		_streamid = steamid;
	  }
	}

	GET_CONFIG(bool, enableVhost, General::kEnableVhost);
	if (!enableVhost || _vhost.empty() || _vhost == "localhost" || INADDR_NONE != inet_addr(_vhost.data())) {
	  _vhost = DEFAULT_VHOST;
	}
  }
}