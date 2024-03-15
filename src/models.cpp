#include "models.h"

namespace Models
{
  Timestamped::Timestamped()
  {
  }

  Timestamped::Timestamped(Poco::DateTime &&time)
      : time(std::move(time))
  {
  }

  Timestamped::Timestamped(const Poco::DateTime &time)
      : time(time)
  {
  }

  IncrementalTicker::IncrementalTicker(const json &update,
                                       bool use_server_time)
  {
      this->estimated_delivery_price = update["params"]["data"]["estimated_delivery_price"].get<float>();
      this->bid_iv = update["params"]["data"]["bid_iv"].get<float>();
      this->ask_iv = update["params"]["data"]["ask_iv"].get<float>();
      this->underlying_price = update["params"]["data"]["underlying_price"].get<float>();
      this->mark_iv = update["params"]["data"]["mark_iv"].get<float>();
      this->best_ask_price = update["params"]["data"]["best_ask_price"].get<float>();
      this->min_price = update["params"]["data"]["min_price"].get<float>();
      this->mark_price = update["params"]["data"]["mark_price"].get<float>();
      this->index_price = update["params"]["data"]["index_price"].get<float>();
      this->instrument_name = update["params"]["data"]["instrument_name"].get<std::string>();
  }
} // namespace Models
