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

void DeribitMarketDataGateway::subscribe_to_quote(const void *, Models::ConnectivityStatus &cs)
{
    if (cs == Models::ConnectivityStatus::connected)
    {
        json j = {
            {"method", "public/subscribe"},
            {"params", {
                           {"channels", {"incremental_ticker.BTC-PERPETUAL"}}
            }},
            {"jsonrpc", "2.0"},
            {"id", 2}
        };
        ws.send(j);
    }
}
