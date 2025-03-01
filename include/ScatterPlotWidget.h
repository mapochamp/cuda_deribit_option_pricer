#ifndef SCATTERPLOTWIDGET_H
#define SCATTERPLOTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include "DataModel.h"

QT_CHARTS_USE_NAMESPACE

/**
 * @brief The ScatterPlotWidget class provides a widget for displaying a scatter plot
 * It uses Qt Charts to render the data points and automatically updates when data changes
 */
class ScatterPlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScatterPlotWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Set the data model to be used for this scatter plot
     * @param model Pointer to the data model
     */
    void setDataModel(DataModel *model);
    
    /**
     * @brief Set the title of the scatter plot
     * @param title The title text
     */
    void setPlotTitle(const QString &title);
    
    /**
     * @brief Set the labels for the X and Y axes
     * @param xLabel Label for X axis
     * @param yLabel Label for Y axis
     */
    void setAxisLabels(const QString &xLabel, const QString &yLabel);
    
    /**
     * @brief Set the range for the X axis
     * @param min Minimum value
     * @param max Maximum value
     */
    void setXRange(qreal min, qreal max);
    
    /**
     * @brief Set the range for the Y axis
     * @param min Minimum value
     * @param max Maximum value
     */
    void setYRange(qreal min, qreal max);
    
    /**
     * @brief Set the marker size for data points
     * @param size Size in pixels
     */
    void setMarkerSize(qreal size);
    
    /**
     * @brief Set the marker color for data points
     * @param color The color to use
     */
    void setMarkerColor(const QColor &color);
    
public slots:
    /**
     * @brief Update the scatter plot with new data points
     * @param points Vector of data points to display
     */
    void updatePlot(const QVector<QPointF> &points);
    
    /**
     * @brief Auto-adjust axes ranges to fit all data points
     */
    void autoAdjustAxes();

private:
    /**
     * @brief Set up the UI components
     */
    void setupUI();
    
    QChartView *m_chartView;
    QChart *m_chart;
    QScatterSeries *m_series;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
    DataModel *m_dataModel;
    
    bool m_autoAdjustAxes = true;  // Whether to auto-adjust axes ranges when data changes
};

#endif // SCATTERPLOTWIDGET_H
