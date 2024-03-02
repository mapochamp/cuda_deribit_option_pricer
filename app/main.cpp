#include <iostream>
#include "deribitws.h"
#include "deribit_gateway.h"

using json = nlohmann::json;

int main()
{
  DeribitWebsocket ws_cli;
  DeribitMarketDataGateway md(ws_cli);

  ws_cli.connect();
}
