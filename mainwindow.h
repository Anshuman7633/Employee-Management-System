#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override; // To handle saving data on close

private slots:
    void on_addButton_clicked();
    void on_updateButton_clicked();
    void on_deleteButton_clicked();
    void on_searchButton_clicked();

private:
    Ui::MainWindow *ui;
    QSet<QString> employeeIds; // To track unique Employee IDs

    void setupTable();
    void saveDataToFile();
    void loadDataFromFile();
};

#endif // MAINWINDOW_H
