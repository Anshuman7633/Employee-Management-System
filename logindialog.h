#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    bool isAuthenticated() const;

private slots:
    void on_loginButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::LoginDialog *ui;
    bool authenticated;
};

#endif // LOGINDIALOG_H
