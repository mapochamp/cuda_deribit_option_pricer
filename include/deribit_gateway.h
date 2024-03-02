#pragma once
#include <optional>
#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"
#include "json.hpp"
#include "models.h"
//#include "bitmexhttp.h"
#include "deribitws.h"
#include "interfaces.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/DateTime.h"
#include "Poco/Timespan.h"

class BitmexMarketDataGateway : public Interfaces::IMarketDataGateway
{
private:
  BitmexWebsocket &ws;
  //std::string order_book_channel = "orderBook10:XBTUSD";
  //const std::string order_book_handle = "orderBook10";
  void subscribe_to_quote(const void *, Models::ConnectivityStatus &cs);

public:
  BitmexMarketDataGateway(BitmexWebsocket &ws);
  ~BitmexMarketDataGateway();
};
