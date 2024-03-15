#include "deribitws.h"

DeribitWebsocket::DeribitWebsocket()
{
  ws.configure(this->uri);
  ws.set_on_message_cb([this](json msg) { this->on_message(msg); });
  ws.set_on_open_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::connected;
    this->connect_changed(this, this->connect_status);
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
  });
  ws.set_on_close_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::disconnected;
    this->connect_changed(this, this->connect_status);
  });
}

DeribitWebsocket::DeribitWebsocket(const std::string &uri, const std::string &api_key, const std::string &api_secret)
    : uri(uri), api_key(api_key), api_secret(api_secret)
{
  std::string signed_uri = this->signed_url();
  ws.configure(signed_uri);
  ws.set_on_message_cb([this](json msg) { this->on_message(msg); });
  ws.set_on_open_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::connected;
    this->connect_changed(this, this->connect_status);
  });
  ws.set_on_close_cb([this]() {
    this->connect_status = Models::ConnectivityStatus::disconnected;
    this->connect_changed(this, this->connect_status);
  });
}

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
          // TODO: we've received a subscription response this means we got an
          // update in vols/price/ or something in one of the options we've
          // subbed to.  we will now parse what we need and update our map
          //if(this->handlers.count(msg["params"]["instrument_name"].get<std:string>()))
          //{
          //    this->handlers[msg["params"]["instrument_name"].get<std::string>()](msg);
          //}
          //std::cout << msg["params"]["instrument_name"].get<std::string>() << std::endl;
      }
      else
      {
          this->logger.warning("Get message on un-specified table");
      }
      return;
  }
  else
  {
      std::cout << "***********************************" << std::endl;
      std::cout << "got something other than a subscription" << std::endl;;
      //std::cout << "GET_RAW_PAYLOAD()" << std::endl;
      //std::cout << msg.dump() << std::endl;
      //std::cout << "\nDUMP" << std::endl;
      // directly print the entire JSON message
      //std::cout << msg.dump(4) << std::endl;
      std::cout << "***********************************\n" << std::endl;
  }
}

void DeribitWebsocket::set_handler(const std::string &h_name, std::function<void(json)> handler)
{
  this->handlers.insert({h_name, handler});
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
