#include "analytics.h"
#include "ui_analytics.h"
#include "home.h" // Include home.h here


Analytics::Analytics(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Analytics)
{
    ui->setupUi(this);
    homeWindow = new Home(); // Initialize homeWindow



}

Analytics::~Analytics()
{
    delete ui;
    delete homeWindow;
}

void Analytics::on_pushButton_clicked()
{
    this->hide();
    homeWindow->show();
}
