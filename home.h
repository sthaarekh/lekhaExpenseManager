#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include"analytics.h"

namespace Ui {
class Home;
}

class Home : public QMainWindow
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = nullptr);
    ~Home();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Home *ui;
    Analytics *analytics;
};

#endif // HOME_H
