#include <iostream>
#include <memory>
#include "deribitws.h"
#include "deribit_gateway.h"
#include "models.h"
#include "option_map_manager.h"
#include "vol_fitter.h"

using json = nlohmann::json;

int main()
{
    Models::OptionsMapPtr optionMap = std::make_shared<Models::ExpiryMap>();
    DeribitWebsocket ws_cli;
	DeribitMarketDataGateway md(ws_cli);
	OptionMapManager omm(md, ws_cli, optionMap);
    VolFitter vol(omm, optionMap);

    ws_cli.connect();
}
