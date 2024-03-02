#include "deribit_gateway.h"

BitmexMarketDataGateway::BitmexMarketDataGateway(BitmexWebsocket &ws)
    : ws(ws)
{
  ws.connect_changed += Poco::delegate(this, &BitmexMarketDataGateway::subscribe_to_quote);
}

BitmexMarketDataGateway::~BitmexMarketDataGateway()
{
  ws.connect_changed -= Poco::delegate(this, &BitmexMarketDataGateway::subscribe_to_quote);
}

void BitmexMarketDataGateway::subscribe_to_quote(const void *, Models::ConnectivityStatus &cs)
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
