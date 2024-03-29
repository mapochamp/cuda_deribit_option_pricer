#include "deribit_gateway.h"

DeribitMarketDataGateway::DeribitMarketDataGateway(DeribitWebsocket &ws)
    : ws(ws)
{
  ws.set_handler("orderbook",[this](json msg) { this->parse_valid_options_list(msg); });
  ws.set_handler("new_quote",[this](json msg) { this->parse_new_option_quote(msg); });
  ws.connect_changed += Poco::delegate(this, &DeribitMarketDataGateway::subscribe_to_quote);
}

DeribitMarketDataGateway::~DeribitMarketDataGateway()
{
  ws.connect_changed -= Poco::delegate(this, &DeribitMarketDataGateway::subscribe_to_quote);
}

void DeribitMarketDataGateway::parse_new_option_quote(json msg)
{
	float bid_iv = -1;
	float ask_iv = -1;
	float best_bid_price = -1;
	float best_ask_price = -1;
	float mark_iv = -1;
    float mark_price = -1;
	if(msg["params"]["data"].contains("bid_iv")) {
		bid_iv = msg["params"]["data"]["bid_iv"].get<float>();
	}
	if(msg["params"]["data"].contains("ask_iv")){
		ask_iv = msg["params"]["data"]["ask_iv"].get<float>();
	}
	if(msg["params"]["data"].contains("best_ask_price")) {
		best_ask_price = msg["params"]["data"]["best_ask_price"].get<float>();
	}
	if(msg["params"]["data"].contains("best_bid_price")){
		best_bid_price = msg["params"]["data"]["best_bid_price"].get<float>();
	}
	if(msg["params"]["data"].contains("mark_iv")){
		mark_iv = msg["params"]["data"]["mark_iv"].get<float>();
	}
	if(msg["params"]["data"].contains("mark_price")){
		mark_iv = msg["params"]["data"]["mark_price"].get<float>();
	}
    std::string instrument_name = msg["params"]["data"]["instrument_name"].get<std::string>();

    Models::IncrementalTicker quote(bid_iv,
            						ask_iv,
            						mark_iv,
            						best_ask_price,
            						best_bid_price,
            						mark_price,
            						instrument_name,
            						Poco::DateTime());

    this->incremental_ticker(this, quote);
}

void DeribitMarketDataGateway::parse_valid_options_list(json msg)
{
	int count = 0;
	for(const auto& option : msg["result"])
	{
		if(count > 10) {
			break;
		}
		Models::OptionType option_type = option["option_type"].get<std::string>()
			== "call" ?
			Models::OptionType::call :
			Models::OptionType::put;
		int instrument_id = option["instrument_id"].get<int>();
		int strike = option["strike"].get<int>();
		int expiration_timestamp = option["expiration_timestamp"].get<int>();
		std::string instrument_name = option["instrument_name"].get<std::string>();
		Models::OrderBookInfo newOption(option_type,
				instrument_id,
				strike,
				expiration_timestamp,
				instrument_name);

		this->order_book_info(this, newOption);
		count++;
	}
}

void DeribitMarketDataGateway::subscribe_to_quote(const void *, Models::ConnectivityStatus &cs)
{
    if (cs == Models::ConnectivityStatus::connected)
    {
        // get all tradable BTC options
        json j = {
            {"method", "public/get_instruments"},
            {"params", {
                           {"currency", "BTC"},
                           {"kind", "option"},
                           {"expired", false}
            }},
            {"jsonrpc", "2.0"},
            {"id", 0}
        };
        ws.send(j);
    }
}
