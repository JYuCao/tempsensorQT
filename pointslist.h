#ifndef POINTSLIST_H
#define POINTSLIST_H

#include <QDialog>

#include "mainwindow.h"

#include <QMetaType>

typedef struct {
    QString x;
    QString y;
} MuItemData;

Q_DECLARE_METATYPE(MuItemData)

namespace Ui {
class PointsList;
}

class PointsList : public QDialog
{
    Q_OBJECT

public:
    explicit PointsList(QWidget *parent = nullptr);
    ~PointsList();

private:
    Ui::PointsList *ui;

    void addPoint(void);
    void removePoint();

};

#endif // POINTSLIST_H
