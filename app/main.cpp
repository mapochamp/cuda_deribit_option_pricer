#include <iostream>
#include "deribitws.h"
#include "deribit_gateway.h"

using json = nlohmann::json;

int main()
{
  BitmexWebsocket ws_cli;
  BitmexMarketDataGateway md(ws_cli);

  ws_cli.connect();
}
