#pragma once
#include <optional>
#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"
#include "json.hpp"
#include "models.h"
#include "deribitws.h"
#include "interfaces.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/DateTime.h"
#include "Poco/Timespan.h"

class DeribitMarketDataGateway
{
private:
  DeribitWebsocket &ws;
  void subscribe_to_quote(const void *, Models::ConnectivityStatus &cs);

public:
  DeribitMarketDataGateway(DeribitWebsocket &ws);
  ~DeribitMarketDataGateway();
};
