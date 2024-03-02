#pragma once

#include <string>
#include <map>
#include <functional>
#include <future>
#include "util.h"
#include "models.h"
#include "ws.h"
#include "json.hpp"
#include "Poco/Util/Timer.h"
#include "Poco/Util/TimerTaskAdapter.h"
#include "Poco/Timestamp.h"
#include "Poco/Logger.h"
#include "Poco/BasicEvent.h"

using json = nlohmann::json;

class DeribitWebsocket
{
public:
  DeribitWebsocket();
  DeribitWebsocket(const std::string &uri);
  DeribitWebsocket(const std::string &uri, const std::string &api_key, const std::string &api_secret);
  ~DeribitWebsocket();

  Poco::BasicEvent<Models::ConnectivityStatus> connect_changed;

  void connect();
  void close();
  void send(json &j);
  void set_handler(const std::string &, std::function<void(json)>);

private:
  WS ws;
  std::string uri = "wss://test.deribit.com/ws/api/v2";
  std::string api_key = "";
  std::string api_secret = "";
  Models::ConnectivityStatus connect_status = Models::ConnectivityStatus::disconnected;

  std::map<std::string, std::function<void(json)>> handlers;
  Poco::Logger &logger = Poco::Logger::get("gr-deribitws");

  std::string signed_url();
  void on_message(const std::string &raw);
};
