#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include "json.hpp"
#include "models.h"
#include "interfaces.h"
#include "Poco/Delegate.h"
#include "deribitws.h"

class OptionMapManagerBase
{
public:
  virtual std::unordered_map<int, Models::Option> get_latest_calls(int expiry) = 0;
  virtual std::unordered_map<int, Models::Option> get_latest_puts(int expiry) = 0;
};

class OptionMapManager : public OptionMapManagerBase
{
private:
  DeribitWebsocket &ws;
  Interfaces::IMarketDataGateway &md;

  int sub_count = 1;

  // <key: strike, value: option>
  std::unordered_map<int, Models::Option> latest_calls;
  std::unordered_map<int, Models::Option> latest_puts;

  Models::ExpiryMap optionsMap;

  void update_option_map_incremental(const void *, Models::IncrementalTicker &option);
  void update_option_map_init(const void *, Models::OrderBookInfo &option);
  void subscribe_to_option(const std::string &instrument_name);

  int parse_strike_from_instrument_name(std::string instrument_name);
  int parse_expiration_from_instrument_name(const std::string &instrument_name);
  Models::OptionType parse_option_type_from_instrument_name(const std::string &instrument_name);

    // Map of month abbreviations to their numeric representation
    std::unordered_map<std::string, std::string> monthMap = {
        {"JAN", "01"}, {"FEB", "02"}, {"MAR", "03"}, {"APR", "04"}, {"MAY", "05"}, {"JUN", "06"},
        {"JUL", "07"}, {"AUG", "08"}, {"SEP", "09"}, {"OCT", "10"}, {"NOV", "11"}, {"DEC", "12"}
    };

public:
  OptionMapManager(Interfaces::IMarketDataGateway &md, DeribitWebsocket &ws);
  ~OptionMapManager();

  std::unordered_map<int, Models::Option> get_latest_calls(int expiry) override;
  std::unordered_map<int, Models::Option> get_latest_puts(int expiry)  override;
};
