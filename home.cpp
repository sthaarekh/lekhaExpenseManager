#include "home.h"
#include "ui_home.h"

// Constructor for the home class
Home::Home(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Home)
{
    ui->setupUi(this);
    //combobox code ie for dropdown
    ui->comboBox->addItem("Food");
    ui->comboBox->addItem("Clothes");
    ui->comboBox->addItem("Clothes");

    // Connecting to the SQLite database
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/sthaarekh/Documents/qtLearn/database/lendBorrow.db"); // SQLite database location
    db.open();
    // Error message if the connection to the database fails
    if (!db.open()) {
        qDebug() << "Failed to connect to database: " << db.lastError();
        return;
    }

    // Displays the data of lendBorrow to lbFrame
    displayData();

    // Creates table lendBorrow if it is missing
    QSqlQuery query;
    if (query.exec("CREATE TABLE IF NOT EXISTS lendBorrow (id INTEGER PRIMARY KEY AUTOINCREMENT, amount TEXT, description TEXT, tag TEXT)")) {
        qDebug() << "Table created successfully";
    } else {
        qDebug() << "Failed to create table: " << query.lastError();
    }
}


// Destructor for the home class
Home::~Home()
{
    // Close the database connection when the application closes
    db.close();
    delete ui;
}


void Home::on_pushButton_clicked()
{
    this->hide();
    analytics = new Analytics(this);
    analytics->show();
}


// Slot to handle the deletion of an item
void home::deleteItem() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        int id = button->property("id").toInt();

        // Show a confirmation dialog
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Item", "Are you sure you want to delete this item?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // Execute the deletion query
            QSqlQuery query;
            query.prepare("DELETE FROM lendBorrow WHERE id = :id");
            query.bindValue(":id", id);
            if (query.exec()) {
                // If deletion was successful, refresh the display
                displayData();
            } else {
                // Handle query execution error
                QMessageBox::warning(this, "Error", "Failed to delete the item.");
            }
        }
    }
}


// Function to display data from the lendBorrow table
void home::displayData() {
    // Query to select description and amount from the table lendBorrow
    QSqlQuery query("SELECT id, description, amount FROM lendBorrow");

    // Create a new vertical layout for the frame
    QVBoxLayout* layout = new QVBoxLayout;

    // Fetch data and create labels dynamically
    while (query.next()) {
        int id = query.value(0).toInt();
        QString description = query.value(1).toString();
        QVariant amount = query.value(2);

        // Concatenating amount and description
        QString lbData = description + " : Rs. " + amount.toString();
        QLabel* lbLabel = new QLabel(lbData, this);

        // Create a horizontal layout to hold the label and button
        QHBoxLayout* itemLayout = new QHBoxLayout;
        itemLayout->addWidget(lbLabel);

        // Create the cross button
        QPushButton* deleteButton = new QPushButton("X", this);
        deleteButton->setProperty("id", id); // Store the id in the button property

        // Connect the button's clicked signal to the deletion slot
        connect(deleteButton, &QPushButton::clicked, this, &home::deleteItem);

        // Add the button to the item layout
        itemLayout->addWidget(deleteButton);

        // Add the item layout to the main layout
        layout->addLayout(itemLayout);
    }

    // Add a vertical spacer to the layout
    layout->addStretch();

    // Remove the previous layout if it exists
    if (ui->lbFrame->layout() != nullptr) {
        QLayoutItem* item;
        while ((item = ui->lbFrame->layout()->takeAt(0)) != nullptr) {
            delete item->widget(); // Delete the widget
            delete item;           // Delete the layout item
            qDebug() << "Deleted widget and layout item";
        }
        delete ui->lbFrame->layout(); // Delete the old layout
        qDebug() << "Deleted old layout";
    }

    // Set the new layout to the lend borrow frame
    ui->lbFrame->setLayout(layout);
    qDebug() << "Set new layout";
}


// Function to insert data into the lendBorrow table
void home::insertData(int amount, const QString &description, const QString &tag)
{
    QSqlQuery query;
    // Prepare the SQL query to insert data
    query.prepare("INSERT INTO lendBorrow(amount, description, tag) VALUES (:amount, :description, :tag)");
    query.bindValue(":amount", amount);
    query.bindValue(":description", description);
    query.bindValue(":tag", tag);

    // Execute the query and display appropriate message boxes
    if (query.exec()) {
        home::displayData();  // Update the label after inserting data
        QMessageBox::information(this, "Success", "Data added successfully");

    } else {
        QMessageBox::critical(this, "Database Error", "Failed to save data: " + query.lastError().text());
    }
}

// Slot for handling the "Lend" button click event
void home::on_lendButton_clicked() {
    bool ok;
    // Checking whether the amount is a number or not
    int amountD = ui->amount->text().toInt(&ok);
    if (!ok) {
        QMessageBox::critical(this, "Invalid Input", "Invalid amount. Please enter a numeric value.");
        return;
    }
    QString descD = ui->description->text();
    if (descD.length() < 3) {
        // Show an error message or handle the condition appropriately
        QMessageBox::warning(this, "Input Error", "Description must be at least 3 letters long.");
        return ;
    }
    QString tagD = "lend";
    insertData(amountD, descD, tagD);
}


// Slot for handling the "Borrow" button click event
void home::on_borrowButton_clicked() {
    bool ok;
    // Checking whether the amount is a number or not
    int amountD = ui->amount->text().toInt(&ok);
    if (!ok) {
        QMessageBox::critical(this, "Invalid Input", "Invalid amount. Please enter a numeric value.");
        return;
    }
    QString descD = ui->description->text();
    if (descD.length() < 3) {
        // Show an error message or handle the condition appropriately
        QMessageBox::warning(this, "Input Error", "Description must be at least 3 letters long.");
        return ;
    }
    QString tagD = "borrow";
    insertData(amountD, descD, tagD);
}



