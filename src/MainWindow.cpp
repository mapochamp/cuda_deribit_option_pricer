#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Set window properties
    setWindowTitle("Scatter Plot Demo");
    resize(800, 600);
    
    // Create models and widgets
    m_dataModel = new DataModel(this);
    m_scatterPlotWidget = new ScatterPlotWidget(this);
    m_messageHandler = new MessageHandler(this);
    
    // Link components together
    m_scatterPlotWidget->setDataModel(m_dataModel);
    m_messageHandler->setDataModel(m_dataModel);
    
    // Set up the UI
    setupUI();
    
    // Initialize widgets and connections
    initializeWidgets();
}

MainWindow::~MainWindow()
{
    // Stop message handling when window closes
    m_messageHandler->stopListening();
}

void MainWindow::setupUI()
{
    // Create central widget and main layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Add scatter plot to layout
    mainLayout->addWidget(m_scatterPlotWidget, 1);
    
    // Create button group
    QGroupBox *controlsGroupBox = new QGroupBox("Controls", centralWidget);
    QHBoxLayout *controlsLayout = new QHBoxLayout(controlsGroupBox);
    
    // Create buttons
    m_startButton = new QPushButton("Start Listening", controlsGroupBox);
    m_stopButton = new QPushButton("Stop Listening", controlsGroupBox);
    m_clearButton = new QPushButton("Clear Data", controlsGroupBox);
    m_addSampleButton = new QPushButton("Add Sample Data", controlsGroupBox);
    
    // Add buttons to layout
    controlsLayout->addWidget(m_startButton);
    controlsLayout->addWidget(m_stopButton);
    controlsLayout->addWidget(m_clearButton);
    controlsLayout->addWidget(m_addSampleButton);
    
    // Add controls to main layout
    mainLayout->addWidget(controlsGroupBox);
    
    // Set central widget
    setCentralWidget(centralWidget);
}

void MainWindow::initializeWidgets()
{
    // Configure scatter plot
    m_scatterPlotWidget->setPlotTitle("Scatter Plot Demo");
    m_scatterPlotWidget->setAxisLabels("X Axis", "Y Axis");
    m_scatterPlotWidget->setMarkerSize(8.0);
    m_scatterPlotWidget->setMarkerColor(QColor(0, 114, 189));
    
    // Disable stop button initially
    m_stopButton->setEnabled(false);
    
    // Connect button signals to slots
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartListeningClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStopListeningClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::onClearDataClicked);
    connect(m_addSampleButton, &QPushButton::clicked, this, &MainWindow::onAddSampleDataClicked);
}

void MainWindow::onStartListeningClicked()
{
    m_messageHandler->startListening();
    
    // Update button states
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
}

void MainWindow::onStopListeningClicked()
{
    m_messageHandler->stopListening();
    
    // Update button states
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
}

void MainWindow::onClearDataClicked()
{
    m_dataModel->clearAllPoints();
}

void MainWindow::onAddSampleDataClicked()
{
    // Create some sample data points
    QVector<QPointF> samplePoints;
    
    // Add a circle pattern
    for (int i = 0; i < 20; ++i) {
        double angle = 2.0 * M_PI * i / 20.0;
        double radius = 8.0;
        double x = radius * std::cos(angle);
        double y = radius * std::sin(angle);
        samplePoints.append(QPointF(x, y));
    }
    
    // Add some random points inside the circle
    for (int i = 0; i < 10; ++i) {
        double angle = QRandomGenerator::global()->bounded(2.0 * M_PI);
        double radius = QRandomGenerator::global()->bounded(7.0);
        double x = radius * std::cos(angle);
        double y = radius * std::sin(angle);
        samplePoints.append(QPointF(x, y));
    }
    
    // Update the data model
    m_dataModel->setAllPoints(samplePoints);
}
