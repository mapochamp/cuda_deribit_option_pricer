#pragma once
#include <optional>
#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"
#include "json.hpp"
#include "models.h"
#include "interfaces.h"
#include "deribitws.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/DateTime.h"
#include "Poco/Timespan.h"

class DeribitMarketDataGateway : public Interfaces::IMarketDataGateway
{
public:
  DeribitMarketDataGateway(DeribitWebsocket &ws);
  ~DeribitMarketDataGateway();
  void parse_valid_options_list(json msg);
  void parse_new_option_quote(json msg);

private:
  DeribitWebsocket &ws;
  void subscribe_to_quote(const void *, Models::ConnectivityStatus &cs);
};
