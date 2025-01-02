#include "loginDialog.h"
#include "ui_loginDialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    authenticated(false)
{
    ui->setupUi(this);
    setWindowTitle("Login");

    // Mask the password input
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

bool LoginDialog::isAuthenticated() const
{
    return authenticated;
}

void LoginDialog::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    // Simple authentication logic
    if (username == "Anshuman7633" && password == "1234") {
        authenticated = true;
        accept(); // Close the dialog with success
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password!");
    }
}

void LoginDialog::on_cancelButton_clicked()
{
    reject(); // Close the dialog without logging in
}
