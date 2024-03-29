#include "option_map_manager.h"
#include <openssl/asn1.h>
#include <stdexcept>
#include <regex>

OptionMapManager::OptionMapManager(Interfaces::IMarketDataGateway &md, DeribitWebsocket &ws)
    : md(md), ws(ws)
{
  md.incremental_ticker += Poco::delegate(this, &OptionMapManager::update_option_map_incremental);
  md.order_book_info += Poco::delegate(this, &OptionMapManager::update_option_map_init);
}

OptionMapManager::~OptionMapManager()
{
  md.incremental_ticker -= Poco::delegate(this, &OptionMapManager::update_option_map_incremental);
  md.order_book_info -= Poco::delegate(this, &OptionMapManager::update_option_map_init);
}

void OptionMapManager::update_option_map_incremental(const void *, Models::IncrementalTicker &option)
{
    int strike = parse_strike_from_instrument_name(option.instrument_name);
    int expiration = parse_expiration_from_instrument_name(option.instrument_name);
    Models::OptionType option_type = parse_option_type_from_instrument_name(option.instrument_name);

    Models::Option new_option(option.bid_iv,
                           	  option.ask_iv,
                           	  option.mark_iv,
                           	  option.best_ask_price,
                           	  option.best_bid_price,
                           	  option.mark_price,
                           	  option.instrument_name,
                           	  option_type,
                           	  strike,
                           	  expiration);

	Models::Option old_option = optionsMap[expiration][option_type][strike];
	if(new_option.bid_iv == -1)
	{
		new_option.bid_iv = old_option.bid_iv;
	}
	if(new_option.ask_iv == -1)
	{
		new_option.ask_iv = old_option.ask_iv;
	}
	if(new_option.best_ask_price == -1)
	{
		new_option.best_ask_price = old_option.best_ask_price;
	}
	if(new_option.best_bid_price == -1)
	{
		new_option.best_bid_price = old_option.best_bid_price;
	}
	if(new_option.mark_iv == -1)
	{
		new_option.mark_iv = old_option.mark_iv;
	}
	if(new_option.mark_price == -1)
	{
		new_option.mark_price = old_option.mark_price;
	}

	optionsMap[expiration][option_type][strike] = new_option;

	std::cout << "=====================================================" << std::endl;
	std::cout << "New option update: " 	<< new_option.instrument_name << std::endl;
	std::cout << "bid_iv: "            	<< new_option.bid_iv << std::endl;
	std::cout << "ask_iv: "            	<< new_option.ask_iv << std::endl;
	std::cout << "mark_iv: "           	<< new_option.mark_iv << std::endl;
	std::cout << "best_ask_price: " 	<< new_option.best_ask_price << std::endl;
	std::cout << "best_bid_price: " 	<< new_option.best_bid_price << std::endl;
	std::cout << "mark_price: " 		<< new_option.mark_price << std::endl;
	std::cout << "strike: " 			<< new_option.strike << std::endl;
	std::cout << "expiration: " 		<< new_option.expiration << std::endl;
	std::cout << "=====================================================" << std::endl;
}

void OptionMapManager::update_option_map_init(const void *, Models::OrderBookInfo &option)
{
	std::cout << "update option map init" << std::endl;
	int expiration = parse_expiration_from_instrument_name(option.instrument_name);
    Models::Option new_option(0,
                           	  0,
                           	  0,
                           	  0,
                           	  0,
                           	  0,
                           	  option.instrument_name,
                           	  option.option_type,
                           	  option.strike,
                           	  expiration);

	optionsMap[expiration][option.option_type][option.strike] = new_option;
	subscribe_to_option(option.instrument_name);
}
  
std::unordered_map<int, Models::Option> OptionMapManager::get_latest_calls(int expiry)
{
    return this->latest_calls;
}

std::unordered_map<int, Models::Option> OptionMapManager::get_latest_puts(int expiry)
{
    return this->latest_puts;
}

int OptionMapManager::parse_strike_from_instrument_name(std::string instrument_name)
{
    std::string delimiter = "-";
    std::string token;
    size_t pos = 0;
    int count = 0;

    while((pos = instrument_name.find(delimiter)) != std::string::npos)
    {
        token = instrument_name.substr(0, pos);
        if(count == 2) 
        {
            try {
                return std::stoi(token);
            } catch (const std::invalid_argument &e) {
                std::cout << e.what() << std::endl;
            }
            break;
        }
        instrument_name.erase(0, pos + delimiter.length());
        count++;
    }

    return 0;
}

Models::OptionType OptionMapManager::parse_option_type_from_instrument_name(const std::string &instrument_name)
{
    Models::OptionType type;
    const char& type_char = instrument_name.back();
    switch(type_char)
    {
        case 'C':
            type = Models::OptionType::call;
            break;
        case 'P':
            type = Models::OptionType::put;
            break;
        default:
            throw std::invalid_argument("Option Type not found in instrument name string");
            break;
    }
    return type;
}

int OptionMapManager::parse_expiration_from_instrument_name(const std::string& input) {
    std::regex expiryRegex(R"((\d{2})([A-Z]{3})(\d{2}))");
    std::smatch matches;
    std::regex_search(input, matches, expiryRegex);

    if (matches.size() < 4) { // We expect 4 matches: the whole string, day, month, and year
        return -1; // Return an invalid code
    }

    std::string day = matches[1];
    std::string monthStr = matches[2];
    std::string year = "20" + matches[3].str(); // Assuming all dates are in the 2000s
    std::string month = monthMap[monthStr]; // Convert month abbreviation to a number

    // Construct the date in YYYYMMDD format as a string first to ensure proper zero padding
    std::string dateString = year + month + day;

    // Convert the YYYYMMDD string to an integer
    return std::stoi(dateString);
}

void OptionMapManager::subscribe_to_option(const std::string &instrument_name)
{
	std::cout << "Subscribing to incremental_ticker." << instrument_name << std::endl;;
	json j = {
		{"method", "public/subscribe"},
		{"params", {
					   {"channels", {"incremental_ticker." + instrument_name}},
				   }},
		{"jsonrpc", "2.0"},
		{"id", sub_count}
	};
	ws.send(j);
	sub_count++;
}
