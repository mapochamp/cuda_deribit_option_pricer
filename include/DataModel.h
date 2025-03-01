#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QObject>
#include <QVector>
#include <QPointF>
#include <tuple>
#include <vector>

/**
 * @brief The DataModel class is responsible for storing and managing data points
 * for the scatter plot. It emits signals when data changes to update the view.
 */
class DataModel : public QObject
{
    Q_OBJECT

public:
    explicit DataModel(QObject *parent = nullptr);
    
    /**
     * @brief Get all data points
     * @return Vector of points (x,y)
     */
    QVector<QPointF> getAllPoints() const;
    
    /**
     * @brief Add a single point to the dataset
     * @param point The point to add
     */
    void addPoint(const QPointF &point);
    
    /**
     * @brief Add multiple points to the dataset
     * @param points Vector of points to add
     */
    void addPoints(const QVector<QPointF> &points);
    
    /**
     * @brief Replace all current data with new data
     * @param points The new data points
     */
    void setAllPoints(const QVector<QPointF> &points);
    
    /**
     * @brief Clear all data points
     */
    void clearAllPoints();
    
    /**
     * @brief Convert from std::vector<std::tuple<float, float>> to QVector<QPointF>
     * @param stdPoints Vector of std::tuple points
     * @return Vector of QPointF points
     */
    static QVector<QPointF> convertStdPointsToQt(const std::vector<std::tuple<float, float>> &stdPoints);
    
signals:
    /**
     * @brief Signal emitted when data is changed
     * @param points The updated set of points
     */
    void dataChanged(const QVector<QPointF> &points);

private:
    QVector<QPointF> m_points; // Storage for data points
};

#endif // DATAMODEL_H
