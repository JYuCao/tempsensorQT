#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"

#include <QDebug>
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initConnect();
    initPlot();
}

void MainWindow::initConnect()
{
    connect(ui->actionOpen_Serial, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionClose_Serial, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionConfig, &QAction::triggered, this, &MainWindow::on_btnConfig_clicked);

    m_serial = new QSerialPort();
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    connect(ui->m_plot, SIGNAL(mousePress(QMouseEvent *)), this, SLOT(slot_SameTimeMousePressEvent4Plot(QMouseEvent *)));
}

void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = settingsDialog.settings();

    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);

    m_serial->open(QIODevice::ReadWrite);

    startPlot();
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
}

void MainWindow::startPlot()
{
    ui->m_plot->xAxis->setRange(0, TIME_BASE);
    ui->m_plot->yAxis->setRange(20, 40);
    ui->m_plot->addGraph();
}

void MainWindow::clearPlot()
{
    ui->m_plot->removeGraph(0);
    time = 0;
}

void MainWindow::initPlot()
{
    ui->m_plot->xAxis->setRange(0, TIME_BASE);
    ui->m_plot->yAxis->setRange(20, 40);
}

void MainWindow::readData()
{
    QByteArray d = m_serial->readAll();

    data = getData(&d);

    static double max = data;
    static double min = data;

    if (data > max) {
        max = data;
        ui->lineEdit_maxvalue->setText(QString::number(max, 'f', 2));
    }
    if (data < min) {
        min = data;
        ui->lineEdit_minvalue->setText(QString::number(min, 'f', 2));
    }
    ui->lineEdit_current->setText(QString::number(data, 'f', 2));

    ui->m_plot->graph(0)->addData(time, data);

    ui->m_plot->replot();

    QThread::msleep(100);
    QApplication::processEvents();

    if (time > TIME_BASE)
    {
        ui->m_plot->xAxis->setRange(0, time);
    }
    time += 1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnConfig_clicked()
{
    settingsDialog.exec();
}

double MainWindow::getData(QByteArray * data)
{
    // qDebug()<< "位数：" << data.size();
    char buf[6];
    for (qsizetype i = 0; i < data->size(); ++i)
    {
        // qDebug()<< i << "：" << data.at(i);
        buf[i] = data->at(i);
    }
    return atof(buf);
}

void MainWindow::on_btnReplot_clicked()
{
    closeSerialPort();
    clearPlot();
    openSerialPort();
    startPlot();

    clearPoints();
}

void MainWindow::clearPoints()
{
    for (int i = 0; i < tracers.size(); ++i) {
        tracers[i]->deleteLater();
        textTips[i]->deleteLater();
    }
    tracers.clear();
    textTips.clear();
}

void MainWindow::slot_SameTimeMousePressEvent4Plot(QMouseEvent *e)
{
    // 获取点击位置的图形
    QCPGraph *graph = qobject_cast<QCPGraph *>(ui->m_plot->plottableAt(e->pos(), true));
    if (!graph)
        return;

    // 获取点击位置的坐标
    double x = ui->m_plot->xAxis->pixelToCoord(e->pos().x());
    double y = ui->m_plot->yAxis->pixelToCoord(e->pos().y());
    QPointF pressPos(x, y);

    double range = 0.3;
    for (int i = 0; i < tracers.size(); ++i) {
        double tracerX = tracers[i]->position->key();
        double tracerY = tracers[i]->position->value();
        
        // 判断标记点是否在给定范围内
        if (qAbs(tracerX - x) < range && qAbs(tracerY - y) < range) {
            removePoint(i);
            return;
        }
    }

    // 创建新的标记点和文本标签
    appendPoint(graph, x, y);

    ui->m_plot->replot();
}

void MainWindow::appendPoint(QCPGraph * graph, double x, double y)
{
    QCPItemTracer *m_timeTracer = new QCPItemTracer(ui->m_plot);
    m_timeTracer->setGraph(graph);
    m_timeTracer->setGraphKey(x);
    m_timeTracer->setInterpolating(true);
    m_timeTracer->setStyle(QCPItemTracer::tsCircle);
    m_timeTracer->setPen(QColor(255, 255, 255));
    m_timeTracer->setBrush(QBrush(QColor(255, 0, 0), Qt::SolidPattern));
    m_timeTracer->setSize(10);

    QCPItemText *m_timeTextTip = new QCPItemText(ui->m_plot);
    m_timeTextTip->position->setParentAnchor(m_timeTracer->position);   // 将文本标签的位置设置为相对于标记点的位置
    m_timeTextTip->position->setCoords(0, -15);                         // 相对于标记点的位置，向上偏移15个单位
    m_timeTextTip->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    QFont font;
    font.setPixelSize(10);
    m_timeTextTip->setFont(font);
    m_timeTextTip->setPen(QPen(Qt::black));
    m_timeTextTip->setBrush(Qt::white);
    m_timeTextTip->setText("X轴: " + QString::number(x, 'f', 2) + "\nY轴: " + QString::number(y, 'f', 2));

    textTips.append(m_timeTextTip);
    tracers.append(m_timeTracer);
}

void MainWindow::removePoint(int i)
{
    tracers[i]->deleteLater();
    textTips[i]->deleteLater();
    tracers.removeAt(i);
    textTips.removeAt(i);
    ui->m_plot->replot();
}
