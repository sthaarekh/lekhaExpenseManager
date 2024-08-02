#ifndef HOME_H
#define HOME_H

#include "analytics.h"
#include <QMainWindow>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QDialog>
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

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

    void on_lendButton_clicked();

    void on_borrowButton_clicked();

    void on_analyticsButton_clicked();

    void deleteItem(int id);



private:
    Ui::Home *ui;
    QMap<QFrame*, int> frameToIdMap;
    Analytics *analyticsWindow;
    QSqlDatabase db;
    bool validateInput(QString amount, QString description);
    bool validateInput(QLineEdit &amountEdit, QLineEdit &descriptionEdit, QLineEdit &categoryEdit);
    void insertData(int amount, const QString &description, const QString &tag);
    void displayStatement();
    void notifyLendBorrow();
    void updateData(int id, const QString &amount, const QString &description, const QString &category);
    void showEditDialog(int id);
    void deleteItem();


protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
};


#endif // HOME_H
