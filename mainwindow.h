#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

#include "settingsdialog.h"
#include "qcustomplot.h"

#define TIME_BASE  10       // 初始时间轴量程
#define CLINK_DISTANCE  10  // 标点距离判定
#define Y_MAX 40            // 纵轴最大值
#define Y_MIN 20            // 纵轴最小值

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
    /* 槽函数 */
    void on_btnConfig_clicked();    // 串口配置按钮

    void on_btnReplot_clicked();    // 重绘按钮

    void slot_SameTimeMousePressEvent4Plot(QMouseEvent *e); // 标点行为处理

    void on_btnPause_clicked();     // 暂停按钮

private:
    Ui::MainWindow      *ui;            // 主窗体类
    SettingsDialog      settingsDialog; // 设置窗口类
    QSerialPort         *m_serial;      // 串口类

    double data;        // 存储当前数据

    double time = 0;    // 记录当前时间

    /* 全局最值 */
    double max;
    double min;

    /* 用于曲线标点 */
    QList<QCPItemTracer*> tracers;  // 点集
    QList<QCPItemText*> textTips;   // 文本标签集

private:
    void openSerialPort();  // 开启串口接收
    void closeSerialPort(); // 关闭串口接收
    
    void startPlot();       // 开始画图
    void clearPlot();       // 清除曲线

    void readData();                // 读取数据
    double getData(QByteArray *);   // 处理数据

    /* 曲线标点 */
    void appendPoint(QCPGraph *, double, double);   // 增加点
    void removePoint(int i);                        // 删除点
    void clearPoints();                             // 清空所有点

    void calculateSteadyStateAndRiseTime(); // 计算稳态值与上升时间

    void outputPlotData();  // 输出曲线数据（调试用）
};
#endif // MAINWINDOW_H
