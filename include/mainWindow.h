#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "ScatterPlotWidget.h"
#include "DataModel.h"
#include "MessageHandler.h"

/**
 * @brief The MainWindow class is the main application window
 * It contains the scatter plot widget and controls for managing the data
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /**
     * @brief Start listening for messages
     */
    void onStartListeningClicked();
    
    /**
     * @brief Stop listening for messages
     */
    void onStopListeningClicked();
    
    /**
     * @brief Clear all data points
     */
    void onClearDataClicked();
    
    /**
     * @brief Add some example data points for testing
     */
    void onAddSampleDataClicked();

private:
    /**
     * @brief Set up the UI components
     */
    void setupUI();
    
    /**
     * @brief Initialize the widgets and connect signals/slots
     */
    void initializeWidgets();

    ScatterPlotWidget *m_scatterPlotWidget;
    DataModel *m_dataModel;
    MessageHandler *m_messageHandler;
    
    // UI controls
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    QPushButton *m_clearButton;
    QPushButton *m_addSampleButton;
};

#endif // MAINWINDOW_H
