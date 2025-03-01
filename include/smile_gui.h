#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QTimer>
#include <QtCharts>
#include "vol_fitter.h"
#include "option_map_manager.h"

class SimpleVolatilityChart : public QWidget
{
    Q_OBJECT

public:
    SimpleVolatilityChart(VolFitter& vol_fitter, OptionMapManager& omm, QWidget *parent = nullptr);
    ~SimpleVolatilityChart();

public slots:
    void refreshData();
    void updateExpiries();
    void onExpiryChanged(int index);
    void onRefreshClicked();

private:
    void setupUI();
    void updatePlot();
    
    // Chart components
    QChartView *m_chartView;
    QChart *m_chart;
    QComboBox *m_expiryComboBox;
    QPushButton *m_refreshButton;
    QLabel *m_statusLabel;
    
    // Data providers
    VolFitter& m_volFitter;
    OptionMapManager& m_optionManager;
    
    // Timer for periodic updates
    QTimer *m_updateTimer;
    
    // State tracking
    int m_currentExpiry;
    bool m_hasData;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(VolFitter& vol_fitter, OptionMapManager& omm, QWidget *parent = nullptr);

private:
    SimpleVolatilityChart *m_chart;
};