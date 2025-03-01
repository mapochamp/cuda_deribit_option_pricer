#include "smile_gui.h"
#include <iostream>
#include <QtWidgets>

SimpleVolatilityChart::SimpleVolatilityChart(VolFitter& vol_fitter, OptionMapManager& omm, QWidget *parent)
    : QWidget(parent)
    , m_volFitter(vol_fitter)
    , m_optionManager(omm)
    , m_currentExpiry(0)
    , m_hasData(false)
{
    setupUI();
    
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &SimpleVolatilityChart::refreshData);
    m_updateTimer->start(1000);
    
    std::cout << "SimpleVolatilityChart created" << std::endl;
}

SimpleVolatilityChart::~SimpleVolatilityChart()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void SimpleVolatilityChart::setupUI()
{
    std::cout << "Setting up UI" << std::endl;
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Create the chart with sample data to ensure it displays
    m_chart = new QChart();
    m_chart->setTitle("Volatility Smile");
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    
    // Add a simple series to verify chart rendering
    QLineSeries *testSeries = new QLineSeries();
    testSeries->setName("Test Data");
    testSeries->append(0, 0);
    testSeries->append(10, 10);
    testSeries->append(20, 5);
    testSeries->append(30, 15);
    m_chart->addSeries(testSeries);
    
    // Create default axes
    m_chart->createDefaultAxes();
    
    // Configure axes appearance
    QAbstractAxis *xAxis = m_chart->axes(Qt::Horizontal).first();
    QAbstractAxis *yAxis = m_chart->axes(Qt::Vertical).first();
    
    if (xAxis && yAxis) {
        xAxis->setTitleText("Strike");
        yAxis->setTitleText("Implied Volatility (%)");
        xAxis->setRange(0, 100000);
        yAxis->setRange(0, 200);
    }
    
    // Create chart view
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumSize(600, 400);
    std::cout << "Chart created with test data" << std::endl;
    
    // Create control panel
    QHBoxLayout *controlLayout = new QHBoxLayout();
    
    m_expiryComboBox = new QComboBox();
    m_expiryComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_expiryComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &SimpleVolatilityChart::onExpiryChanged);
    
    m_refreshButton = new QPushButton("Refresh");
    connect(m_refreshButton, &QPushButton::clicked, this, &SimpleVolatilityChart::onRefreshClicked);
    
    m_statusLabel = new QLabel("Waiting for data...");
    
    controlLayout->addWidget(new QLabel("Expiry:"));
    controlLayout->addWidget(m_expiryComboBox);
    controlLayout->addWidget(m_refreshButton);
    controlLayout->addStretch();
    controlLayout->addWidget(m_statusLabel);
    
    mainLayout->addWidget(m_chartView);
    mainLayout->addLayout(controlLayout);
    
    setLayout(mainLayout);
    
    // Add some placeholder text but ensure it's visible
    QGraphicsTextItem *placeholder = new QGraphicsTextItem("Waiting for option data...");
    QFont font = placeholder->font();
    font.setPointSize(14);
    placeholder->setFont(font);
    placeholder->setDefaultTextColor(Qt::red);
    placeholder->setPos(m_chart->plotArea().center().x() - 150, m_chart->plotArea().center().y());
    m_chart->scene()->addItem(placeholder);
    
    // Set a reasonable size
    setMinimumSize(800, 600);
    std::cout << "UI setup complete" << std::endl;
}

void SimpleVolatilityChart::refreshData()
{
    updateExpiries();
    updatePlot();
}

void SimpleVolatilityChart::updateExpiries()
{
    int currentIndex = m_expiryComboBox->currentIndex();
    QString currentText = currentIndex >= 0 ? m_expiryComboBox->currentText() : "";
    
    auto optionsMap = m_optionManager.getOptionsMap();
    
    m_expiryComboBox->blockSignals(true);
    m_expiryComboBox->clear();
    
    std::cout << "SimpleVolatilityChart::updateExpiries() - Available expiries: ";
    for (const auto& [expiry, _] : *optionsMap) {
        if (expiry > 0) {
            m_expiryComboBox->addItem(QString::number(expiry));
            std::cout << expiry << " ";
        }
    }
    std::cout << std::endl;
    
    if (!currentText.isEmpty()) {
        int index = m_expiryComboBox->findText(currentText);
        if (index >= 0) {
            m_expiryComboBox->setCurrentIndex(index);
        }
    } else if (m_expiryComboBox->count() > 0) {
        // Select the first item if there was no previous selection
        m_expiryComboBox->setCurrentIndex(0);
        std::cout << "Auto-selected first expiry: " << m_expiryComboBox->currentText().toStdString() << std::endl;
    }
    
    if (m_expiryComboBox->count() > 0) {
        m_statusLabel->setText("Data available");
        m_hasData = true;
        std::cout << "Data available, hasData = true" << std::endl;
    } else {
        m_statusLabel->setText("Waiting for data...");
        m_hasData = false;
        std::cout << "No data available, hasData = false" << std::endl;
    }
    
    m_expiryComboBox->blockSignals(false);
}

void SimpleVolatilityChart::onExpiryChanged(int index)
{
    if (index >= 0) {
        bool ok;
        m_currentExpiry = m_expiryComboBox->itemText(index).toInt(&ok);
        if (ok) {
            std::cout << "Selected expiry: " << m_currentExpiry << std::endl;
            updatePlot();
        }
    }
}

void SimpleVolatilityChart::onRefreshClicked()
{
    refreshData();
}

void SimpleVolatilityChart::updatePlot()
{
    std::cout << "SimpleVolatilityChart::updatePlot() - Starting update" << std::endl;

    if (!m_hasData || m_currentExpiry <= 0) {
        std::cout << "SimpleVolatilityChart::updatePlot() - No data or invalid expiry: " 
                  << "hasData=" << m_hasData << ", expiry=" << m_currentExpiry << std::endl;
        return;
    }
    
    m_chart->removeAllSeries();
    
    auto optionsMap = m_optionManager.getOptionsMap();
    std::cout << "SimpleVolatilityChart::updatePlot() - Options map size: " << optionsMap->size() << std::endl;
    
    if (optionsMap->empty() || optionsMap->find(m_currentExpiry) == optionsMap->end()) {
        std::cout << "SimpleVolatilityChart::updatePlot() - No data for expiry " << m_currentExpiry << std::endl;
        return;
    }
    
    const auto& expiry_map = (*optionsMap)[m_currentExpiry];
    std::cout << "SimpleVolatilityChart::updatePlot() - Expiry map size: " << expiry_map.size() << std::endl;
    
    if (expiry_map.find(Models::OptionType::call) == expiry_map.end()) {
        std::cout << "SimpleVolatilityChart::updatePlot() - No call options for expiry " << m_currentExpiry << std::endl;
        return;
    }
    
    QScatterSeries *marketData = new QScatterSeries();
    marketData->setName("Market IV");
    marketData->setMarkerSize(10);
    marketData->setColor(QColor(0, 0, 255));
    
    int pointCount = 0;
    std::cout << "SimpleVolatilityChart::updatePlot() - Processing call options: " 
              << expiry_map.at(Models::OptionType::call).size() << " items" << std::endl;
              
    // Debug: Show all call options regardless of mark_iv
    for (const auto& [strike, option] : expiry_map.at(Models::OptionType::call)) {
        std::cout << "  Option at strike=" << strike 
                  << " has mark_iv=" << option.mark_iv
                  << ", bid_iv=" << option.bid_iv
                  << ", ask_iv=" << option.ask_iv << std::endl;
                  
        // If mark_iv is zero, try to use average of bid/ask IVs
        double iv_to_use = option.mark_iv;
        if (iv_to_use <= 0.0) {
            if (option.bid_iv > 0.0 && option.ask_iv > 0.0) {
                iv_to_use = (option.bid_iv + option.ask_iv) / 2.0;
                std::cout << "  Using average of bid/ask IVs: " << iv_to_use << std::endl;
            } else if (option.ask_iv > 0.0) {
                iv_to_use = option.ask_iv;
                std::cout << "  Using ask IV: " << iv_to_use << std::endl;
            } else if (option.bid_iv > 0.0) {
                iv_to_use = option.bid_iv;
                std::cout << "  Using bid IV: " << iv_to_use << std::endl;
            }
        }
        
        // Only add points with valid IV > 0
        if (iv_to_use > 0) {
            // Convert to percentage
            marketData->append(strike, iv_to_use * 100);
            std::cout << "  Added market point: Strike=" << strike << ", IV=" << iv_to_use * 100 << "%" << std::endl;
            pointCount++;
        } else {
            std::cout << "  Skipping point with zero/invalid IV" << std::endl;
        }
    }
    
    m_chart->addSeries(marketData);
    
    QLineSeries *fittedCurve = new QLineSeries();
    fittedCurve->setName("SVI Fit");
    fittedCurve->setColor(QColor(255, 0, 0));
    QPen pen = fittedCurve->pen();
    pen.setWidth(2);
    fittedCurve->setPen(pen);
    
    // Calculate strike range first
    double min_strike = 0, max_strike = 0;
    for (const auto& [strike, _] : expiry_map.at(Models::OptionType::call)) {
        if (min_strike == 0 || strike < min_strike) min_strike = strike;
        if (strike > max_strike) max_strike = strike;
    }
    
    std::cout << "SimpleVolatilityChart::updatePlot() - Strike range: " << min_strike << " to " << max_strike << std::endl;
    
    const auto& params = m_volFitter.get_fitted_params();
    std::cout << "SimpleVolatilityChart::updatePlot() - SVI params size: " << params.size() << std::endl;
    
    if (params.size() == 5 && pointCount > 0 && min_strike > 0 && max_strike > 0) {
        for (double strike = min_strike; strike <= max_strike; strike += 100) {
            double vol = m_volFitter.get_svi_vol(strike, 1.0, 1.0,
                params[0], params[1], params[2], params[3], params[4]);
            fittedCurve->append(strike, vol * 100); // Convert to percentage
            std::cout << "  Added fitted point: Strike=" << strike << ", IV=" << vol * 100 << "%" << std::endl;
        }
        
        m_chart->addSeries(fittedCurve);
    }
    
    if (pointCount > 0) {
        m_chart->createDefaultAxes();
        
        QValueAxis *xAxis = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Horizontal).first());
        QValueAxis *yAxis = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Vertical).first());
        
        if (xAxis && yAxis) {
            xAxis->setTitleText("Strike");
            yAxis->setTitleText("Implied Volatility (%)");
            
            xAxis->setGridLineVisible(true);
            yAxis->setGridLineVisible(true);
            
            // Set reasonable ranges
            if (min_strike > 0 && max_strike > 0) {
                double buffer = (max_strike - min_strike) * 0.1; // 10% buffer
                xAxis->setRange(min_strike - buffer, max_strike + buffer);
            }
            
            // Force a reasonable Y range even if data is sparse
            yAxis->setRange(0, 200);
            
            m_chart->setTitle(QString("Volatility Smile - Expiry: %1").arg(m_currentExpiry));
        } else {
            std::cout << "SimpleVolatilityChart::updatePlot() - Failed to get axes" << std::endl;
        }
    } else {
        // Add placeholder text since we don't have data points
        QGraphicsTextItem *placeholder = new QGraphicsTextItem("No valid IV data available for this expiry");
        placeholder->setPos(m_chart->plotArea().center());
        m_chart->scene()->addItem(placeholder);
    }
    
    std::cout << "SimpleVolatilityChart::updatePlot() - Plot updated with " << pointCount << " data points" << std::endl;
}

MainWindow::MainWindow(VolFitter& vol_fitter, OptionMapManager& omm, QWidget *parent)
    : QMainWindow(parent)
{
    m_chart = new SimpleVolatilityChart(vol_fitter, omm);
    setCentralWidget(m_chart);
    
    setWindowTitle("Volatility Smile Viewer");
    resize(900, 700);
    
    std::cout << "MainWindow created" << std::endl;
}