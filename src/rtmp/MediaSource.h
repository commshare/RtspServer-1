#pragma once
#include <mutex>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
using namespace std;
using namespace toolkit;
namespace toolkit {

  class MediaInfo {
  public:
	MediaInfo() {}
	MediaInfo(const string& url) {
	  parse(url);
	}
	~MediaInfo() {}

	void parse(const string& url);

	string& operator[](const string& key) {
	  return _params[key];
	}
  public:
	string _schema;
	string _host;
	string _port;
	string _vhost;
	string _app;
	string _streamid;
	StrCaseMap _params;
	string _param_strs;
  };

}