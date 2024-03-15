#pragma once
#include <iostream>
#include "json.hpp"
#include "Poco/DateTime.h"
#include "Poco/DateTimeParser.h"
#include "Poco/DateTimeFormat.h"

using json = nlohmann::json;

namespace Models
{
  // === ENUMS ===
  enum class ConnectivityStatus
  {
    connected,
    disconnected
  };

  enum class OptionType
  {
      call,
      put
  };

  // === CLASSES ===
  class Timestamped
  {
  public:
    Poco::DateTime time;
    Timestamped();
    Timestamped(Poco::DateTime &&time);
    Timestamped(const Poco::DateTime &time);
  };

  class IncrementalTicker : public Timestamped
  {
      public:
          float estimated_delivery_price;
          float bid_iv;
          float ask_iv;
          float underlying_price;
          float mark_iv;
          float best_ask_price;
          float min_price;
          float mark_price;
          float index_price;
          std::string instrument_name;

          IncrementalTicker(const json &update, bool use_server_time = false);
          // TODO: Do we really need Poco:DateTime here?
          IncrementalTicker(float _estimated_delivery_price,
                            float _bid_iv,
                            float _ask_iv,
                            float _underlying_price,
                            float _mark_iv,
                            float _best_ask_price,
                            float _min_price,
                            float _mark_price,
                            float _index_price,
                            const std::string &_instrument_name,
                            Poco::DateTime &&_time)
              :

                  estimated_delivery_price(_estimated_delivery_price),
                  bid_iv(_bid_iv),
                  ask_iv(_ask_iv),
                  underlying_price(_underlying_price),
                  mark_iv(_mark_iv),
                  best_ask_price(_best_ask_price),
                  min_price(_min_price),
                  mark_price(_mark_price),
                  index_price(_index_price),
                  instrument_name(_instrument_name)
          {
          }
  };

  class OrderBookInfo
  {
      public:
        OptionType option_type;
        uint32_t instrument_id;
        uint32_t strike;
        uint32_t expiration_timestamp;
        std::string instrument_name;

        OrderBookInfo(OptionType _option_type,
                      uint32_t _instrument_id,
                      uint32_t _strike,
                      uint32_t _expiration_timestamp,
                      std::string _instrument_name) :
                      option_type(_option_type),
                      instrument_id(_instrument_id),
                      strike(_strike),
                      expiration_timestamp(_expiration_timestamp),
                      instrument_name(_instrument_name)
        {
        }
  };
  // === FUNCTIONS ===
} // namespace Models
