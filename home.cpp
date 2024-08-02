#include "home.h"
#include "ui_home.h"


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

    ui->category->addItem("Fooding");
    ui->category->addItem("Rent");
    ui->category->addItem("Transportation");
    ui->category->addItem("Education");
    ui->category->addItem("Clothing");
    ui->category->addItem("Health");
    ui->category->addItem("Miscallaneous");
    ui->comboBox_2->addItem("Self");
    ui->comboBox_2->addItem("Borrowed");

    // Connecting to the SQLite database
    db = QSqlDatabase::addDatabase("QSQLITE");

    // Connecting to the SQLite database
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/sthaarekh/Documents/       /lekhaEx/shubham/database/mydb.db"); // SQLite database location
    db.open();

    // Error message if the connection to the database fails
    if (!db.open()) {
        qDebug() << "Failed to connect to database: " << db.lastError();
        return;
    }

    // Creates table lendBorrow if it is missing
    QSqlQuery query;
    if (query.exec("CREATE TABLE IF NOT EXISTS lendBorrow (id INTEGER PRIMARY KEY AUTOINCREMENT, amount TEXT, description TEXT, tag TEXT)")) {
        qDebug() << "Table created successfully";
    } else {
        qDebug() << "Failed to create table: " << query.lastError();
    }
    notifyLendBorrow();

    // Displays the data of userrec to scrollArea
    displayStatement();
}

Home::~Home()
{
    delete ui;
}


// Function to validate the input
bool Home::validateInput(QLineEdit &amountEdit, QLineEdit &descriptionEdit, QLineEdit &categoryEdit) {
    bool ok;
    // Checking whether the amount is a number or not
    int amount = amountEdit.text().toInt(&ok);
    if (!ok) {
        QMessageBox::critical(this, "Invalid Input", "Invalid amount. Please enter a numeric value.");
        return false;
    }
    QString description = descriptionEdit.text();
    QString category = categoryEdit.text();
    if (description.length() < 3 || category.length() < 3) {
        // Show an error message or handle the condition appropriately
        QMessageBox::warning(this, "Input Error", "Description and category must be at least 3 letters long.");
        return false;
    }
    return true;
}

void Home::displayStatement() {
    QSqlQuery query("SELECT id, description, amount, category FROM userrec");

    QWidget *container = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(container);
    QHBoxLayout* rowLayout = new QHBoxLayout;
    int frameCount = 0;

    while (query.next()) {
        int id = query.value(0).toInt();
        QString description = query.value(1).toString();
        QVariant amount = query.value(2);
        QString category = query.value(3).toString();

        QFrame* lbFrame = new QFrame;
        lbFrame->setStyleSheet("background-color: #CCD8C7; border-radius: 10px;");
        lbFrame->setFixedWidth(170);
        lbFrame->setFixedHeight(140);

        QVBoxLayout* lbLayout = new QVBoxLayout(lbFrame);

        QLabel* amountLB = new QLabel("Rs. " + amount.toString());
        amountLB->setStyleSheet("color: black; font: 40pt \"Lucida Grande\";");
        QLabel* descriptionLB = new QLabel(description);
        descriptionLB->setStyleSheet("color: black; font: 24pt \"Comic Sans MS\";");
        QLabel* categoryLB = new QLabel(category);
        categoryLB->setStyleSheet("color: black; font: 18pt \"Arial\";");

        QFrame* line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFixedHeight(1);
        line->setStyleSheet("background-color: black;");

        lbLayout->addWidget(amountLB);
        lbLayout->addWidget(line);
        lbLayout->addWidget(descriptionLB);
        lbLayout->addWidget(categoryLB);

        rowLayout->addWidget(lbFrame);
        frameCount++;

        if (frameCount == 3) {
            layout->addLayout(rowLayout);
            rowLayout = new QHBoxLayout;
            frameCount = 0;
        }

        // Connect the lbFrame click event to show the EditDialog
        lbFrame->installEventFilter(this);
        frameToIdMap[lbFrame] = id;
    }

    if (frameCount > 0) {
        rowLayout->addStretch();
        layout->addLayout(rowLayout);
    }

    layout->addStretch();
    container->setLayout(layout);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QWidget *oldWidget = ui->scrollArea1->widget();
    if (oldWidget) {
        oldWidget->setParent(nullptr);
        oldWidget->deleteLater();
    }

    ui->scrollArea1->setWidget(container);
    ui->scrollArea1->setWidgetResizable(true);
}


bool Home::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonRelease) {
        QFrame *frame = qobject_cast<QFrame*>(watched);
        if (frame && frameToIdMap.contains(frame)) {
            int id = frameToIdMap[frame];
            showEditDialog(id);
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

void Home::showEditDialog(int id) {
    QSqlQuery query;
    query.prepare("SELECT amount, description, category FROM userrec WHERE id = :id");
    query.bindValue(":id", id);
    query.exec();
    if (query.next()) {
        QString amount = query.value(0).toString();
        QString description = query.value(1).toString();
        QString category = query.value(2).toString();

        QDialog dialog(this);
        dialog.setWindowTitle("Edit Record");

        QVBoxLayout *layout = new QVBoxLayout(&dialog);

        QLineEdit *amountEdit = new QLineEdit(amount, &dialog);
        QLineEdit *descriptionEdit = new QLineEdit(description, &dialog);
        QLineEdit *categoryEdit = new QLineEdit(category, &dialog);

        QPushButton *changeButton = new QPushButton("Change", &dialog);
        connect(changeButton, &QPushButton::clicked, &dialog, &QDialog::accept);

        layout->addWidget(new QLabel("Amount:"));
        layout->addWidget(amountEdit);
        layout->addWidget(new QLabel("Description:"));
        layout->addWidget(descriptionEdit);
        layout->addWidget(new QLabel("Category:"));
        layout->addWidget(categoryEdit);
        layout->addWidget(changeButton);

        if (dialog.exec() == QDialog::Accepted) {
            if (validateInput(*amountEdit, *descriptionEdit, *categoryEdit)) {
                updateData(id, amountEdit->text(), descriptionEdit->text(), categoryEdit->text());
            }
        }
    }
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
    } else {
        // Handle the error
        qDebug() << "Update failed: " << query.lastError();
    }
}

// Function to deleteitem with the use of id
void Home::deleteItem(int id) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this item?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM lendBorrow WHERE id = :id");
        deleteQuery.bindValue(":id", id);
        if (deleteQuery.exec()) {
            // QMessageBox::information(this, "Deleted", "Item deleted successfully.");
            notifyLendBorrow(); // Refresh the data
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete item.");
        }
    }
}

// Function to validate the input of lend borrow
bool Home::validateInput(QString amount, QString description) {
    bool ok;
    // Checking whether the amount is a number or not
    int num = amount.toInt(&ok);
    if (!ok) {
        // qDebug() << amount;
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
    QSqlQuery query;
    // Prepare the SQL query to insert data
    query.prepare("INSERT INTO lendBorrow(amount, description, tag) VALUES (:amount, :description, :tag)");
    query.bindValue(":amount", amount);
    query.bindValue(":description", description);
    query.bindValue(":tag", tag);

    // Execute the query and display appropriate message boxes
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Add", "Do you want to add"+description+":",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (query.exec()) {
           notifyLendBorrow();  // Update the label after inserting data
            QMessageBox::information(this, "Success", "Data added successfully");

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



