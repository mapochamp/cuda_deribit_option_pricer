#include "deribitws.h"

DeribitWebsocket::DeribitWebsocket()
{
  ws.configure(this->uri);
  ws.set_on_message_cb([this](json msg) { this->on_message(msg); });
  ws.set_on_open_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::connected;
    this->connect_changed(this, this->connect_status);
    if (this->on_open_callback) {
      this->on_open_callback();
    }
  });
  ws.set_on_close_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::disconnected;
    this->connect_changed(this, this->connect_status);
  });
}

DeribitWebsocket::DeribitWebsocket(const std::string &uri)
    : uri(uri)
{
  ws.configure(this->uri);
  ws.set_on_message_cb([this](json msg) { this->on_message(msg); });
  ws.set_on_open_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::connected;
    this->connect_changed(this, this->connect_status);
    if (this->on_open_callback) {
      this->on_open_callback();
    }
  });
  ws.set_on_close_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::disconnected;
    this->connect_changed(this, this->connect_status);
  });
}

#if 0
DeribitWebsocket::DeribitWebsocket(const std::string &uri, const std::string &api_key, const std::string &api_secret)
    : uri(uri), api_key(api_key), api_secret(api_secret)
{
  std::string signed_uri = this->signed_url();
  ws.configure(signed_uri);
  ws.set_on_message_cb([this](json msg) { this->on_message(msg); });
  ws.set_on_open_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::connected;
    this->connect_changed(this, this->connect_status);
    if (this->on_open_callback) {
      this->on_open_callback();
    }
  });
  ws.set_on_close_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::disconnected;
    this->connect_changed(this, this->connect_status);
  });
}
#endif

DeribitWebsocket::~DeribitWebsocket()
{
}

void DeribitWebsocket::on_message(const std::string &raw)
{
  json msg = json::parse(raw);

  if (msg.contains("error"))
  {
    this->logger.warning(msg["error"]);
    return;
  }

  // check if message is a subcription response
  if (msg.contains("method"))
  {
      if(msg["method"].get<std::string>() == "subscription")
      {
          this->handlers["new_quote"](msg);
      }
      else
      {
          this->logger.warning("Get message on un-specified table");
      }
      return;
  }
  else
  {
      if(msg.contains("result") && msg["result"].is_array())
	  {
		  const auto& result = msg["result"];
		  if(!result.empty() && result[0].is_object())
		  {
			  this->handlers["orderbook"](msg);
		  }
      } 
      else 
      {
          std::cerr << "The get_instruments 'result' key is missing or is not an array" << std::endl;
      }
  }
}

void DeribitWebsocket::set_handler(const std::string &h_name, std::function<void(json)> handler)
{
  this->handlers.insert({h_name, handler});
}

void DeribitWebsocket::set_on_open_cb(std::function<void()> cb)
{
  this->on_open_callback = cb;
}

void DeribitWebsocket::public_subscribe(const std::vector<std::string>& channels)
{
  json j = {
    {"method", "public/subscribe"},
    {"params", {
      {"channels", channels}
    }},
    {"jsonrpc", "2.0"},
    {"id", 42}
  };
  this->send(j);
  std::cout << "Subscribed to channels: ";
  for (const auto& channel : channels) {
    std::cout << channel << " ";
  }
  std::cout << std::endl;
}

void DeribitWebsocket::connect()
{
  ws.connect();
}

void DeribitWebsocket::close()
{
  ws.close();
}

void DeribitWebsocket::send(json &msg)
{
  ws.send(msg);
}

#if 0
std::string DeribitWebsocket::signed_url()
{
  std::string expires = std::to_string(util::get_seconds_timestamp(util::current_time()).count() + 60);
  std::string verb = "GET";
  std::string path = "/realtime";

  std::string data = verb + path + expires;
  std::string sign = util::encoding::hmac(std::string(api_secret), data);
  std::string signed_url = this->uri + "?api-expires=" + expires + "&api-signature=" + sign + "&api-key=" + this->api_key;

  return signed_url;
}
#endif
