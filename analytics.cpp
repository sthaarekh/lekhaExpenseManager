#include "analytics.h"
#include "ui_analytics.h"

Analytics::Analytics(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Analytics)
{
    ui->setupUi(this);
}

Analytics::~Analytics()
{
    delete ui;
}
