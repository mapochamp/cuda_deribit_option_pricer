#include "DataModel.h"

DataModel::DataModel(QObject *parent) 
    : QObject(parent)
{
    // Initialize with empty data
}

QVector<QPointF> DataModel::getAllPoints() const
{
    return m_points;
}

void DataModel::addPoint(const QPointF &point)
{
    m_points.append(point);
    
    // Notify listeners that data has changed
    emit dataChanged(m_points);
}

void DataModel::addPoints(const QVector<QPointF> &points)
{
    if (points.isEmpty()) {
        return;
    }
    
    m_points.append(points);
    
    // Notify listeners that data has changed
    emit dataChanged(m_points);
}

void DataModel::setAllPoints(const QVector<QPointF> &points)
{
    m_points = points;
    
    // Notify listeners that data has changed
    emit dataChanged(m_points);
}

void DataModel::clearAllPoints()
{
    m_points.clear();
    
    // Notify listeners that data has changed
    emit dataChanged(m_points);
}

QVector<QPointF> DataModel::convertStdPointsToQt(const std::vector<std::tuple<float, float>> &stdPoints)
{
    QVector<QPointF> qtPoints;
    qtPoints.reserve(static_cast<int>(stdPoints.size()));
    
    for (const auto &point : stdPoints) {
        float x = std::get<0>(point);
        float y = std::get<1>(point);
        qtPoints.append(QPointF(x, y));
    }
    
    return qtPoints;
}
