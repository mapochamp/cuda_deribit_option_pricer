#include "deribit_gateway.h"

DeribitMarketDataGateway::DeribitMarketDataGateway(DeribitWebsocket &ws)
    : ws(ws)
{
  ws.connect_changed += Poco::delegate(this, &DeribitMarketDataGateway::subscribe_to_quote);
}

DeribitMarketDataGateway::~DeribitMarketDataGateway()
{
  ws.connect_changed -= Poco::delegate(this, &DeribitMarketDataGateway::subscribe_to_quote);
}

// TODO: 
// 1. on connect we have to get all the BTC options for a available for trade -> on connect
// 2. sort and group them by exp date
// 3. create message handler lookuptable
//    3.1 subscribe to everything
//    3.2 manage subscription data

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
            {"id", 12}
        };
        ws.send(j);
    }
}
