#pragma once
#include <vector>
#include <optional>
#include "models.h"
#include "Poco/BasicEvent.h"

using json = nlohmann::json;

namespace Interfaces
{
  class IMarketDataGateway
  {
  public:
    Poco::BasicEvent<Models::IncrementalTicker> incremental_ticker;
    Poco::BasicEvent<Models::OrderBookInfo> order_book_info;
  };
} // namespace Interfaces
