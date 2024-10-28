#include "pointslist.h"
#include "ui_pointslist.h"

PointsList::PointsList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PointsList)
{
    ui->setupUi(this);

    connect(this, &MainWindow::appendPoint, this, &PointsList::addPoint);
}

PointsList::~PointsList()
{
    delete ui;
}

void PointsList::addPoint(void)
{

}
