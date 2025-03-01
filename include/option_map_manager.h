#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "models.h"
#include "interfaces.h"
#include "deribit_gateway.h"
#include "deribitws.h"
#include "Poco/Delegate.h"

class OptionMapManager
{
public:
  OptionMapManager(Interfaces::IMarketDataGateway &md, DeribitWebsocket &ws, Models::OptionsMapPtr data);
  ~OptionMapManager();

  void update_option_map_incremental(const void *, Models::IncrementalTicker &option);
  void update_option_map_init(const void *, Models::OrderBookInfo &option);
  std::unordered_map<int, Models::Option> get_latest_calls(int expiry);
  std::unordered_map<int, Models::Option> get_latest_puts(int expiry);
  void subscribe_to_option(const std::string &instrument_name);

  // Make optionsMap accessible
  Models::OptionsMapPtr getOptionsMap() const { return optionsMap; }

  Poco::BasicEvent<Models::OptionsMapUpdate> optionsMapUpdate;

private:
  int parse_strike_from_instrument_name(std::string instrument_name);
  Models::OptionType parse_option_type_from_instrument_name(const std::string &instrument_name);
  int parse_expiration_from_instrument_name(const std::string& input);

  Interfaces::IMarketDataGateway &md;
  DeribitWebsocket &ws;
  Models::OptionsMapPtr optionsMap;
  Models::OptionsMapUpdate mapUpdate;
  std::unordered_map<int, Models::Option> latest_calls;
  std::unordered_map<int, Models::Option> latest_puts;
  int sub_count = 0;
};
