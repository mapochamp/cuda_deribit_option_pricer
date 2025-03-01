#include "ScatterPlotWidget.h"
#include <QVBoxLayout>

ScatterPlotWidget::ScatterPlotWidget(QWidget *parent)
    : QWidget(parent),
      m_dataModel(nullptr)
{
    setupUI();
}

void ScatterPlotWidget::setupUI()
{
    // Create the scatter series
    m_series = new QScatterSeries(this);
    m_series->setName("Data Points");
    m_series->setMarkerSize(10.0);
    m_series->setColor(QColor(0, 114, 189)); // Default blue color
    
    // Create chart and chart view
    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->setTitle("Scatter Plot");
    m_chart->legend()->hide();
    
    // Create axes
    m_axisX = new QValueAxis(this);
    m_axisY = new QValueAxis(this);
    m_axisX->setTitleText("X Axis");
    m_axisY->setTitleText("Y Axis");
    m_axisX->setRange(-10, 10);
    m_axisY->setRange(-10, 10);
    
    // Attach axes to the chart
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_series->attachAxis(m_axisX);
    m_series->attachAxis(m_axisY);
    
    // Create chart view
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    
    // Set up layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_chartView);
    setLayout(layout);
}

void ScatterPlotWidget::setDataModel(DataModel *model)
{
    // Disconnect from old model if any
    if (m_dataModel) {
        disconnect(m_dataModel, &DataModel::dataChanged, this, &ScatterPlotWidget::updatePlot);
    }
    
    m_dataModel = model;
    
    // Connect to new model
    if (m_dataModel) {
        connect(m_dataModel, &DataModel::dataChanged, this, &ScatterPlotWidget::updatePlot);
        
        // Initial update with current data
        updatePlot(m_dataModel->getAllPoints());
    }
}

void ScatterPlotWidget::updatePlot(const QVector<QPointF> &points)
{
    // Clear existing points
    m_series->clear();
    
    // Add new points
    m_series->append(points);
    
    // Auto-adjust axes if enabled
    if (m_autoAdjustAxes) {
        autoAdjustAxes();
    }
}

void ScatterPlotWidget::autoAdjustAxes()
{
    if (m_series->count() == 0) {
        return;
    }
    
    // Find the bounds of the data
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();
    
    const QVector<QPointF> points = m_series->points();
    for (const QPointF &point : points) {
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }
    
    // Add some margin
    const qreal marginX = (maxX - minX) * 0.1;
    const qreal marginY = (maxY - minY) * 0.1;
    
    // Update axes ranges
    if (minX != maxX) {
        m_axisX->setRange(minX - marginX, maxX + marginX);
    }
    
    if (minY != maxY) {
        m_axisY->setRange(minY - marginY, maxY + marginY);
    }
}

void ScatterPlotWidget::setPlotTitle(const QString &title)
{
    m_chart->setTitle(title);
}

void ScatterPlotWidget::setAxisLabels(const QString &xLabel, const QString &yLabel)
{
    m_axisX->setTitleText(xLabel);
    m_axisY->setTitleText(yLabel);
}

void ScatterPlotWidget::setXRange(qreal min, qreal max)
{
    m_axisX->setRange(min, max);
    m_autoAdjustAxes = false;
}

void ScatterPlotWidget::setYRange(qreal min, qreal max)
{
    m_axisY->setRange(min, max);
    m_autoAdjustAxes = false;
}

void ScatterPlotWidget::setMarkerSize(qreal size)
{
    m_series->setMarkerSize(size);
}

void ScatterPlotWidget::setMarkerColor(const QColor &color)
{
    m_series->setColor(color);
}
