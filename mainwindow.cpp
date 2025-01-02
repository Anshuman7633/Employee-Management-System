#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup the table
    setupTable();

    // Load data from file on startup
    loadDataFromFile();

    // Connect buttons to slots
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::on_addButton_clicked);
    connect(ui->updateButton, &QPushButton::clicked, this, &MainWindow::on_updateButton_clicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::on_deleteButton_clicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::on_searchButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTable()
{
    ui->tableWidget->setColumnCount(6); // Six columns for the fields
    QStringList headers = {"Sno.", "Employee Name", "Employee ID", "Designation", "Experience", "Age"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::on_addButton_clicked()
{
    QString id = ui->lineEditID->text();
    QString name = ui->lineEditName->text();
    QString designation = ui->lineEditDesignation->text();
    QString experience = ui->lineEditExperience->text();
    QString age = ui->lineEditAge->text();

    // Validation
    if (id.isEmpty() || name.isEmpty() || designation.isEmpty() || age.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields except Experience must be filled!");
        return;
    }
    if (!age.toInt()) {
        QMessageBox::warning(this, "Input Error", "Age must be a valid integer!");
        return;
    }
    if (employeeIds.contains(id)) {
        QMessageBox::warning(this, "Duplicate ID", "Employee ID already exists!");
        return;
    }

    // Add new employee data to the table
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowCount);

    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(rowCount + 1)));
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(name));
    ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(id));
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(designation));
    ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(experience));
    ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(age));

    employeeIds.insert(id); // Track the new ID

    // Clear input fields after adding the employee
    ui->lineEditID->clear();
    ui->lineEditName->clear();
    ui->lineEditDesignation->clear();
    ui->lineEditExperience->clear();
    ui->lineEditAge->clear();
}

void MainWindow::on_updateButton_clicked()
{
    QList<QTableWidgetItem *> selected = ui->tableWidget->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Update Error", "Please select a row to update!");
        return;
    }

    int row = selected.first()->row();
    QString currentId = ui->tableWidget->item(row, 2)->text(); // Current Employee ID

    QString name = QInputDialog::getText(this, "Update Employee", "Enter New Employee Name:", QLineEdit::Normal, selected[1]->text());
    QString id = QInputDialog::getText(this, "Update Employee", "Enter New Employee ID:", QLineEdit::Normal, selected[2]->text());
    QString designation = QInputDialog::getText(this, "Update Employee", "Enter New Designation:", QLineEdit::Normal, selected[3]->text());
    QString experience = QInputDialog::getText(this, "Update Employee", "Enter New Experience:", QLineEdit::Normal, selected[4]->text());
    QString age = QInputDialog::getText(this, "Update Employee", "Enter New Age:", QLineEdit::Normal, selected[5]->text());

    if (name.isEmpty() || id.isEmpty() || designation.isEmpty() || age.isEmpty()) {
        QMessageBox::warning(this, "Update Error", "All fields except Experience must be filled!");
        return;
    }

    if (!age.toInt()) { // Check if age is numeric
        QMessageBox::warning(this, "Input Error", "Age must be a valid integer!");
        return;
    }

    // Check for duplicate Employee ID
    if (id != currentId && employeeIds.contains(id)) {
        QMessageBox::warning(this, "Update Error", "Employee ID already exists!");
        return;
    }

    // Update the employee data
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(name));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(id));
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(designation));
    ui->tableWidget->setItem(row, 4, new QTableWidgetItem(experience));
    ui->tableWidget->setItem(row, 5, new QTableWidgetItem(age));

    // Update the employeeIds set
    employeeIds.remove(currentId); // Remove the old ID
    employeeIds.insert(id); // Add the new ID
}

void MainWindow::on_deleteButton_clicked()
{
    QList<QTableWidgetItem *> selected = ui->tableWidget->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Delete Error", "Please select a row to delete!");
        return;
    }

    int row = selected.first()->row();
    QString id = ui->tableWidget->item(row, 2)->text(); // Get Employee ID
    employeeIds.remove(id); // Remove ID from set
    ui->tableWidget->removeRow(row);

    // Renumber rows
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
    }
}

void MainWindow::on_searchButton_clicked()
{
    QString searchType = QInputDialog::getItem(this, "Search Employee",
                                               "Search by:", {"Employee Name", "Employee ID"}, 0, false);

    if (searchType.isEmpty())
        return;

    QString searchValue = QInputDialog::getText(this, "Search Employee",
                                                QString("Enter %1:").arg(searchType));

    if (searchValue.isEmpty())
        return;

    bool found = false;
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QString currentValue;
        if (searchType == "Employee Name") {
            currentValue = ui->tableWidget->item(row, 1)->text();
        } else if (searchType == "Employee ID") {
            currentValue = ui->tableWidget->item(row, 2)->text();
        }

        if (currentValue == searchValue) {
            ui->tableWidget->selectRow(row);
            found = true;
            break;
        }
    }

    if (!found) {
        QMessageBox::information(this, "Search Result", "No matching employee found.");
    }
}

void MainWindow::saveDataToFile()
{
    QJsonArray dataArray;

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QJsonObject employeeObject;
        employeeObject["Sno"] = ui->tableWidget->item(row, 0)->text();
        employeeObject["Employee Name"] = ui->tableWidget->item(row, 1)->text();
        employeeObject["Employee ID"] = ui->tableWidget->item(row, 2)->text();
        employeeObject["Designation"] = ui->tableWidget->item(row, 3)->text();
        employeeObject["Experience"] = ui->tableWidget->item(row, 4)->text();
        employeeObject["Age"] = ui->tableWidget->item(row, 5)->text();
        dataArray.append(employeeObject);
    }

    QJsonDocument doc(dataArray);
    QFile file("employees.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void MainWindow::loadDataFromFile()
{
    QFile file("employees.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return; // File doesn't exist; nothing to load
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull() || !doc.isArray()) {
        return; // Invalid or empty file
    }

    QJsonArray dataArray = doc.array();
    for (const QJsonValue &value : dataArray) {
        QJsonObject employeeObject = value.toObject();
        int rowCount = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rowCount);

        ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(employeeObject["Sno"].toString()));
        ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(employeeObject["Employee Name"].toString()));
        ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(employeeObject["Employee ID"].toString()));
        ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(employeeObject["Designation"].toString()));
        ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(employeeObject["Experience"].toString()));
        ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(employeeObject["Age"].toString()));

        employeeIds.insert(employeeObject["Employee ID"].toString());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveDataToFile();
    QMainWindow::closeEvent(event); // Ensure the base class handles the event
}
