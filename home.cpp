#include "home.h"
#include "ui_home.h"
#include "capital.h"
#include <QPixmap>
Home::Home(QWidget *parent)
    :QMainWindow(parent)
    , ui(new Ui::Home)
{
    ui->setupUi(this);
    // Hide the title bar and borders
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // Ensure the background is not painted (no background)
    this->setAttribute(Qt::WA_OpaquePaintEvent, true);

    // Disable the window's resizing functionality
    this->setFixedSize(this->width(), this->height());

    ui->category->addItem("Food");
    ui->category->addItem("Housing");
    ui->category->addItem("Education");
    ui->category->addItem("Health");
    ui->category->addItem("Transportation");
    ui->category->addItem("Miscellaneous");
    ui->payMode->addItem("Self");
    ui->payMode->addItem("Borrow");

    // Connecting to the SQLite database
    db = QSqlDatabase::addDatabase("QSQLITE");

    // Connecting to the SQLite database
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/sthaarekh/Documents/       /lekhaEx/shubham/database/mydb-2.db"); // SQLite database location
    db.open();

    // Error message if the connection to the database fails
    if (!db.open()) {
        qDebug() << "Failed to connect to database home: " << db.lastError();
        return;
    }

    createTables();
    notifyLendBorrow();

    // Displays the data of userrec to scrollArea
    displayStatement();
    showAvailableBalance();
}

Home::~Home()
{
    delete ui;
}

void Home::createTables(){
    // Creates table lendBorrow if it is missing
    QSqlQuery query;
    if (query.exec("CREATE TABLE IF NOT EXISTS lendBorrow (id INTEGER PRIMARY KEY AUTOINCREMENT, amount TEXT, description TEXT, tag TEXT)")) {
        qDebug() << "Table created successfully";
    } else {
        qDebug() << "Failed to create lendborrow table: " << query.lastError();
    }

    // Creates table userrec if it is missing
    if (query.exec("CREATE TABLE IF NOT EXISTS userrec (id INTEGER PRIMARY KEY AUTOINCREMENT, category TEXT, description TEXT, amount TEXT, timestamp DEFAULT CURRENT_TIMESTAMP, paymode TEXT)")) {
        qDebug() << "Table created successfully";
    } else {
        qDebug() << "Failed to create userrec table: " << query.lastError();
    }

    // Creates table balance if it is missing
    if (query.exec("CREATE TABLE IF NOT EXISTS balance (id INTEGER PRIMARY KEY AUTOINCREMENT, balance REAL)")) {
        query.prepare("INSERT INTO balance (balance, id) VALUES (0.0, 1)");
        if(!query.exec()){
            qDebug()<<"Query not executed";
        }
        qDebug() << "Table created successfully";
    } else {
        qDebug() << "Failed to create balance table: " << query.lastError();
    }

    // Creates table capital if it is missing
    if (query.exec("CREATE TABLE IF NOT EXISTS capital (id INTEGER PRIMARY KEY AUTOINCREMENT, amount REAL, month INTEGER, year INTEGER)")) {
        qDebug() << "Table created successfully";
    } else {
        qDebug() << "Failed to create capital table: " << query.lastError();
    }
}

void Home::displayStatement() {
    // Query to select id, description, amount, timestamp, and category from the table userrec
    QSqlQuery query("SELECT id, description, amount, timestamp, category FROM userrec ORDER BY timestamp DESC LIMIT 15");

    // Create a new widget to act as the container for the layout
    QWidget *container = new QWidget;

    // Create a new vertical layout for the container
    QVBoxLayout* layout = new QVBoxLayout(container);

    // Create a horizontal layout to hold 3 frames in each row
    QHBoxLayout* rowLayout = new QHBoxLayout;

    int frameCount = 0;

    // Fetch data and create labels dynamically
    while (query.next()) {

        QString description = query.value(1).toString();
        QVariant amount = query.value(2);
        QString timestampUTC = query.value(3).toString();
        QString category = query.value(4).toString(); // Assuming category is in the 4th column

        // Convert the timestamp from UTC to local time
        QDateTime timestamp = QDateTime::fromString(timestampUTC, "yyyy-MM-dd hh:mm:ss");
        timestamp.setTimeSpec(Qt::UTC);
        timestamp = timestamp.toLocalTime();
        // To use 12-hour time format with am/pm marker
        QString timestampLocal = timestamp.toString("MM-dd hh:mm AP");

        // To create a new lbFrame
        QFrame* SFrame = new QFrame;

        // To Set a fixed width and height for the frame
        SFrame->setFixedWidth(1400);
        SFrame->setFixedHeight(100);

        // Create a new vertical layout for lbFrame
        QVBoxLayout* lbLayout = new QVBoxLayout(SFrame);

        // Create labels for amount, description, and category
        QLabel* amountS = new QLabel("Rs. " + amount.toString());
        amountS->setStyleSheet("color: black; "
                               "font: 20pt \"Arial\";");
        QLabel* descriptionS = new QLabel(description );
        descriptionS->setStyleSheet("color: black; "
                                    "font: 18pt \"Arial\";");
        QLabel* timestampS = new QLabel( timestampLocal);
        timestampS->setStyleSheet("color: gray; font: 12pt \"Arial\";");

        // Create a label for the category image
        QLabel* imageLabel = new QLabel;
        imageLabel->setFixedSize(30, 30); // Set the size of the image label

        // Set the image according to the category
        if (category == "food") {
            imageLabel->setPixmap(QPixmap(":/pics/LekhaResources/food.jpg").scaled(30, 30));
        } else if (category == "transportation") {
            imageLabel->setPixmap(QPixmap(":/pics/LekhaResources/transportation.png").scaled(20, 20));
        } else if (category == "entertainment") {
            imageLabel->setPixmap(QPixmap(":/pics/LekhaResources/entertainment.png").scaled(20, 20));
        } else if (category == "health") {
            imageLabel->setPixmap(QPixmap(":/pics/LekhaResources/entertainment.png").scaled(20, 20));
        } else if (category == "education") {
            imageLabel->setPixmap(QPixmap(":/pics/LekhaResources/education.png").scaled(20, 20));
        } else if (category == "rent") {
            imageLabel->setPixmap(QPixmap(":/pics/LekhaResources/rent.png").scaled(20, 20));
        } else if (category == "miscellaneous") {
        imageLabel->setPixmap(QPixmap(":/pics/LekhaResources/miscellaneous.png").scaled(20, 20));
        }

        // Create a horizontal layout for image, description, and amount
        QHBoxLayout* descAmtLayout = new QHBoxLayout;
        descAmtLayout->addWidget(imageLabel);
        descAmtLayout->addWidget(descriptionS);
        descAmtLayout->addStretch(); // Add stretch to push amount to the right
        descAmtLayout->addWidget(amountS);

        // Create a line to separate amount, description, and category
        QFrame* line = new QFrame;
        line->setFrameShape(QFrame::HLine); // Set the frame shape to horizontal line
        line->setFixedHeight(1); // Set the fixed height of the line to 1 pixel
        line->setStyleSheet("background-color: black;");

        // Add labels and line to lbLayout
        lbLayout->addLayout(descAmtLayout);
        lbLayout->addWidget(timestampS);
        lbLayout->addWidget(line);

        // Add the lbFrame to the row layout
        rowLayout->addWidget(SFrame);

        frameCount++;

        if (frameCount == 1) {
            layout->addLayout(rowLayout);
            rowLayout = new QHBoxLayout;
            frameCount = 0;
        }
    }

    if (frameCount > 0) {
        // Add spacer to push items to the left
        rowLayout->addStretch();
        layout->addLayout(rowLayout);
    }

    // Add a vertical spacer to the layout
    layout->addStretch();

    // Set the layout to the container widget
    container->setLayout(layout);

    // Ensure the container resizes with its contents
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Remove the previous widget if it exists
    QWidget *oldWidget = ui->scrollArea1->widget();
    if (oldWidget) {
        oldWidget->setParent(nullptr);
        oldWidget->deleteLater();
    }

    // Set the new container widget to the scroll area
    ui->scrollArea1->setWidget(container);
    ui->scrollArea1->setWidgetResizable(true); // Ensure the scroll area resizes with its contents
}





void Home::notifyLendBorrow() {
    // Query to select description and amount from the table lendBorrow
    QSqlQuery query("SELECT id, description, amount, tag FROM lendBorrow");

    // Create a new widget to act as the container for the layout
    QWidget *container = new QWidget;

    // Create a new grid layout for the container
    QGridLayout* layout = new QGridLayout(container);

    int row = 0;
    int column = 0;

    // Fetch data and create labels dynamically
    while (query.next()) {
        int id = query.value(0).toInt();
        QString description = query.value(1).toString();
        QVariant amount = query.value(2);
        QString tag = query.value(3).toString();
        // Create a new lbFrame
        QFrame* lbFrame = new QFrame;


        // Set a fixed width and height for the frame
        lbFrame->setFixedWidth(170);
        lbFrame->setFixedHeight(190);

        // Create a new vertical layout for lbFrame
        QVBoxLayout* lbLayout = new QVBoxLayout(lbFrame);

        // Create labels for tag, amount and description
        QLabel* tagLB;
        if(tag=="lend"){
            tagLB = new QLabel("LEND");
            lbFrame->setStyleSheet("background-color: rgb(255, 230, 204); "
                                   "border-radius: 10px;");
        }
        else{
            tagLB = new QLabel("BORROW");
            lbFrame->setStyleSheet("background-color: rgb(255, 200, 204); "
                                   "border-radius: 10px;");
        }
        tagLB->setStyleSheet("color: black; "
                                "font: 28pt \"Lucida Grande\";");

        QLabel* amountLB = new QLabel("Rs. " + amount.toString());
        amountLB->setStyleSheet("color: grey; "
                                "font: 28pt \"Lucida Grande\";");
        QLabel* descriptionLB = new QLabel(description);
        descriptionLB->setStyleSheet("color: black; "
                                     "font: 24pt \"Comic Sans MS\";");

        // Create a line to separate amount and description
        QFrame* line = new QFrame;
        line->setFrameShape(QFrame::HLine); // Set the frame shape to horizontal line
        line->setFixedHeight(1); // Set the fixed height of the line to 1 pixel
        line->setStyleSheet("background-color: black;");

        // Create a delete button
        QPushButton* deleteButton = new QPushButton;
        deleteButton->setIcon(QIcon("/Users/sthaarekh/Documents/qtLearn/bin.png")); // Set the path to your image file
        deleteButton->setIconSize(QSize(24, 24)); // Adjust the icon size as needed
        deleteButton->setStyleSheet("color: black; "
                                    "font: 16pt \"Comic Sans MS\";");
        deleteButton->setCursor(Qt::PointingHandCursor);
        connect(deleteButton, &QPushButton::clicked, this, [=]() { deleteItem(id); });

        // Create a horizontal layout for the delete button
        QHBoxLayout* buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch(); // Add a stretchable space before the delete button
        buttonLayout->addWidget(deleteButton);

        // Add labels, line, and delete button to lbLayout
        lbLayout->addWidget(tagLB);
        lbLayout->addWidget(line);
        lbLayout->addWidget(amountLB);
        lbLayout->addWidget(descriptionLB);
        lbLayout->addLayout(buttonLayout); // Add the button layout instead of the delete button directly

        // Add the lbFrame to the grid layout
        layout->addWidget(lbFrame, row, column);

        column++;
        if (column == 3) {
            column = 0;
            row++;
        }
    }

    // Add a vertical spacer to the layout
    layout->setRowStretch(row + 1, 1);

    // Set the layout to the container widget
    container->setLayout(layout);

    // Ensure the container resizes with its contents
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Remove the previous widget if it exists
    QWidget *oldWidget = ui->scrollArea->widget();
    if (oldWidget) {
        oldWidget->setParent(nullptr);
        oldWidget->deleteLater();
    }

    // Set the new container widget to the scroll area
    ui->scrollArea->setWidget(container);
    ui->scrollArea->setWidgetResizable(true); // Ensure the scroll area resizes with its contents
}

void Home::updateData(int id, const QString &amount, const QString &description, const QString &category) {
    QSqlQuery query;
    query.prepare("UPDATE userrec SET amount = :amount, description = :description, category = :category WHERE id = :id");
    query.bindValue(":amount", amount);
    query.bindValue(":description", description);
    query.bindValue(":category", category);
    query.bindValue(":id", id);
    if (query.exec()) {
        // Refresh the display
        displayStatement();
        showAvailableBalance();
    } else {
        // Handle the error
        qDebug() << "Update data failed in home: " << query.lastError();
    }
}

// Function to deleteitem with the use of id
void Home::deleteItem(int id) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this item?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("SELECT amount, tag FROM lendBorrow WHERE id = :id");
        query.bindValue(":id", id);
        if (query.exec() && query.next()) {
            double amount = query.value(0).toDouble();
            QString tag = query.value(1).toString() + "R";
            QSqlQuery deleteQuery;
            deleteQuery.prepare("DELETE FROM lendBorrow WHERE id = :id");
            deleteQuery.bindValue(":id", id);
            if (deleteQuery.exec()) {
                QMessageBox::information(this, "Deleted", "Item deleted successfully.");
                notifyLendBorrow(); // Refresh the data
                Capital::editAvailableBalance(amount, tag);
                showAvailableBalance();
            } else {
                QMessageBox::warning(this, "Error", "Failed to delete item.");
            }
        } else {
            QMessageBox::warning(this, "Error", "Failed to retrieve amount.");
        }
    }
}

// Function to validate the input of lend borrow
bool Home::validateInput(QString amount, QString description) {
    bool ok;
    // Checking whether the amount is a number or not
    int num = amount.toInt(&ok);
    if (!ok) {
        QMessageBox::critical(this, "Invalid Input", "Invalid amount. Please enter a numeric value.");
        return false;
    }

    if (description.length() < 3) {
        // Show an error message or handle the condition appropriately
        QMessageBox::warning(this, "Input Error", "Description must be at least 3 letters long.");
        return false;
    }
    return true;
}

// Function to insert data into the lendBorrow table
void Home::insertData(int amount, const QString &description, const QString &tag)
{
    double availableBalance = Capital::getAvailableBalance();

    if (tag == "lend" && amount > availableBalance) {
        QMessageBox::critical(this, "Insufficient Balance", "You do not have sufficient balance to lend this amount.");
        return;
    }

    QSqlQuery query;
    // Prepare the SQL query to insert data
    query.prepare("INSERT INTO lendBorrow(amount, description, tag) VALUES (:amount, :description, :tag)");
    query.bindValue(":amount", amount);
    query.bindValue(":description", description);
    query.bindValue(":tag", tag);

    // Execute the query and display appropriate message boxes
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Add", "Do you want to add "+description+":"+QString::number(amount),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (query.exec()) {
            notifyLendBorrow();  // Update the label after inserting data
            Capital::editAvailableBalance(amount, tag);
            showAvailableBalance();
        } else {
            QMessageBox::critical(this, "Database Error", "Failed to save data: " + query.lastError().text());
        }
    }
}


void Home::on_lendButton_clicked()
{
    QString amount = ui->amountL->text();
    QString desc = ui->descriptionL->text();

    // Validate the input
    if (!validateInput(amount, desc)) {
        // If validation fails, exit the function
        return;
    }

    QString tag = "lend";
    insertData(amount.toInt(), desc, tag);
}

void Home::on_borrowButton_clicked()
{
    QString amount = ui->amountB->text();
    QString desc = ui->descriptionB->text();

    // Validate the input
    if (!validateInput(amount, desc)) {
        // If validation fails, exit the function
        return;
    }

    QString tag = "borrow";
    insertData(amount.toInt(), desc, tag);
}

void Home::on_analyticsButton_clicked()
{
        this->hide();
        analyticsWindow = new Analytics;
        analyticsWindow->show();
        delete (this);

}


bool Home::validateInput(int &amount, QString &description, QString &category, QString &payMode) {
    bool ok;
    // Checking whether the amount is a number or not
    amount = ui->amountAI->text().toInt(&ok);
    if (!ok) {
        QMessageBox::critical(this, "Invalid Input", "Invalid amount. Please enter a numeric value.");
        return false;
    }
    description = ui->descriptionAI->text();
    category = ui->category->currentText();
    payMode = ui-> payMode->currentText();
    if (description.length() < 3 ) {
        // Show an error message or handle the condition appropriately
        QMessageBox::warning(this, "Input Error", "Description  must be at least 3 letters long.");
        return false;
    }
    return true;
}


// Function to insert data into the userrec table
void Home::insertData(int amount, const QString &description, const QString &category, const QString &payMode) {

    QSqlQuery query;
    // Prepare the SQL query to insert data
    query.prepare("INSERT INTO userrec(amount, description, category, paymode) VALUES (:amount, :description, :category, :payMode)");
    query.bindValue(":amount", amount);
    query.bindValue(":description", description);
    query.bindValue(":category", category.toLower());
    query.bindValue (":payMode", payMode.toLower());

    // Execute the query and display appropriate message boxes
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Add", "You spent: Rs."+QString::number(amount)+" on "+description,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // Execute the query and display appropriate message boxes
        if (query.exec()) {
            displayStatement();  // Update the label after inserting data
            Capital::editAvailableBalance(amount, "expense");
            showAvailableBalance();
        } else {
            QMessageBox::critical(this, "Database Error", "Failed to save data: " + query.lastError().text());
        }
    }
}

void Home::callAvailableBalance(){
    Home *home = new Home(nullptr); // or pass a valid QWidget pointer as parent
   home->showAvailableBalance();
}
void Home::showAvailableBalance(){
    double amount = Capital::getAvailableBalance();
    ui->avBalance->setText(QString::number(amount));
}

void Home::on_addItemButton_clicked()
{
    // Checking whether the amount is a number or not
    int amountD= ui->amountAI->text().toInt();
    QString descD = ui->descriptionAI->text();
    QString catecD = ui->category->currentText();
    QString modeD = ui->payMode->currentText();
    // Validate the input
    if (!validateInput(amountD, descD, catecD, modeD)) {
        // If validation fails, exit the function
        return;
    }

    insertData(amountD, descD, catecD, modeD);
}

