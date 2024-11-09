#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"
#include "qcustomplot.h"
#include <QSerialPort>

#define TIME_BASE  10
#define CLINK_DISTANCE  10

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnConfig_clicked();

    void on_btnReplot_clicked();

    void slot_SameTimeMousePressEvent4Plot(QMouseEvent *e);

    void on_btnPause_clicked();

private:
    Ui::MainWindow      *ui;
    SettingsDialog      settingsDialog;
    QSerialPort * m_serial;
    double data;
    double time = 0;
    QVector<QPointF> previousPoints;
    QList<QCPItemTracer*> tracers;
    QList<QCPItemText*> textTips;

private:
    void initConnect();
    void openSerialPort();
    void closeSerialPort();
    
    void initPlot();
    void startPlot();
    void clearPlot();

    void readData();
    double getData(QByteArray *);

    void clearMarkers();

    void appendPoint(QCPGraph * graph, double x, double y);
    void removePoint(int i);
    void clearPoints();

    void outputPlotData();
    void calculateSteadyStateAndRiseTime();
};
#endif // MAINWINDOW_H
