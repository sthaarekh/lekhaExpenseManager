#include "home.h"
#include "ui_home.h"

// Constructor for the home class
Home::Home(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Home)
{
    ui->setupUi(this);

}

Home::~Home()
{
    delete ui;
}

void Home::on_pushButton_clicked()
{
    this->hide();
    analytics = new Analytics(this);
    analytics->show();
}

