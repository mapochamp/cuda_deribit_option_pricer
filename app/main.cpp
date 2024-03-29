#include <iostream>
#include <memory>
#include "deribitws.h"
#include "deribit_gateway.h"
#include "models.h"
#include "option_map_manager.h"

using json = nlohmann::json;

int main()
{
    DeribitWebsocket ws_cli;
	DeribitMarketDataGateway md(ws_cli);
	OptionMapManager omm(md, ws_cli);

    ws_cli.connect();
}
