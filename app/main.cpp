#include <iostream>
#include <memory>
#include <QApplication>
#include "deribitws.h"
#include "deribit_gateway.h"
#include "models.h"
#include "option_map_manager.h"
#include "vol_fitter.h"
#include "smile_gui.h"

using json = nlohmann::json;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create data structures
    Models::OptionsMapPtr optionMap = std::make_shared<Models::ExpiryMap>();
    DeribitWebsocket ws_cli;
    DeribitMarketDataGateway md(ws_cli);
    OptionMapManager omm(md, ws_cli, optionMap);
    VolFitter vol(omm, optionMap);

    // Set up additional subscription for ticker data
    ws_cli.set_on_open_cb([&ws_cli]() {
        std::cout << "Connected to Deribit, subscribing to option ticker data..." << std::endl;
        ws_cli.public_subscribe({"ticker.BTC-.*-C", "ticker.BTC-.*-P"});
    });

    // Create and show the main window
    MainWindow mainWindow(vol, omm);
    mainWindow.show();
    std::cout << "Main window created and shown" << std::endl;

    // Connect to Deribit - this will trigger DeribitMarketDataGateway subscription
    // and our additional ticker subscription in the on_open callback
    std::cout << "Connecting to Deribit..." << std::endl;
    ws_cli.connect();

    // Start Qt event loop
    return app.exec();
}
