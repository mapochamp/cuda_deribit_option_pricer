#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
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
          float bid_iv;
          float ask_iv;
          float mark_iv;
          float best_ask_price;
          float best_bid_price;
          float mark_price;
          std::string instrument_name;

          IncrementalTicker(const json &update, bool use_server_time = false);
          // TODO: Do we really need Poco:DateTime here?
          IncrementalTicker(float _bid_iv,
                            float _ask_iv,
                            float _mark_iv,
                            float _best_ask_price,
                            float _best_bid_price,
                            float _mark_price,
                            const std::string &_instrument_name,
                            Poco::DateTime &&_time)
              :

                  bid_iv(_bid_iv),
                  ask_iv(_ask_iv),
                  mark_iv(_mark_iv),
                  best_ask_price(_best_ask_price),
                  best_bid_price(_best_bid_price),
                  mark_price(_mark_price),
                  instrument_name(_instrument_name)
          {
          }
  };

  class OrderBookInfo
  {
      public:
        OptionType option_type;
        int instrument_id;
        int strike;
        int expiration_timestamp;
        std::string instrument_name;

		OrderBookInfo();
        OrderBookInfo(OptionType _option_type,
                      int _instrument_id,
                      int _strike,
                      int _expiration_timestamp,
                      std::string _instrument_name) :
                      option_type(_option_type),
                      instrument_id(_instrument_id),
                      strike(_strike),
                      expiration_timestamp(_expiration_timestamp),
                      instrument_name(_instrument_name)
        {
        }
  };

  class Option
  {
      public:
          float bid_iv;
          float ask_iv;
          float mark_iv;
          float best_ask_price;
          float best_bid_price;
          float mark_price;
          std::string instrument_name;
          OptionType option_type;
          int strike;
          int expiration;

		  Option() :
                  bid_iv(0),
                  ask_iv(0),
                  mark_iv(0),
                  best_ask_price(0),
                  best_bid_price(0),
                  mark_price(0),
                  instrument_name("none"),
                  option_type(OptionType::call),
                  strike(0),
                  expiration(0)
		  {
		  }
          Option( float _bid_iv,
                  float _ask_iv,
                  float _mark_iv,
                  float _best_ask_price,
                  float _best_bid_price,
                  float _mark_price,
                  std::string _instrument_name,
                  OptionType _option_type,
                  int _strike,
                  int _expiration)
              :
                  bid_iv(_bid_iv),
                  ask_iv(_ask_iv),
                  mark_iv(_mark_iv),
                  best_ask_price(_best_ask_price),
                  best_bid_price(_best_bid_price),
                  mark_price(_mark_price),
                  instrument_name(_instrument_name),
                  option_type(_option_type),
                  strike(_strike),
                  expiration(_expiration)
          {
          }
  };

  // === DEFINES === 
  using StrikeMap = std::unordered_map<int, Option>;
  using OptionTypeMap = std::unordered_map<OptionType, StrikeMap>;
  using ExpiryMap = std::unordered_map<int, OptionTypeMap>;

  // === FUNCTIONS ===
} // namespace Models
