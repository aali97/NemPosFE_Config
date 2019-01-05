#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegularExpression ipRegex("^" + ipRange
                               + "(\\." + ipRange + ")"
                               + "(\\." + ipRange + ")"
                               + "(\\." + ipRange + ")$");
    QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(ipRegex, this);
    ui->lEditPostgresIP->setValidator(ipValidator);
    ui->lEditServerIP->setValidator(ipValidator);

    connect(ui->lEditPostgresIP, QLineEdit::textChanged, this,
            MainWindow::on_lineEditIP_textChanged);
    connect(ui->lEditServerIP, QLineEdit::textChanged, this,
            MainWindow::on_lineEditIP_textChanged);
    connect(ui->lEditUsername, QLineEdit::textChanged, this,
            MainWindow::on_lEditCredential_textChanged);
    connect(ui->lEditPassword, QLineEdit::textChanged, this,
            MainWindow::on_lEditCredential_textChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setStatus(const QString &text, StatusType type)
{
    ui->statusBar->showMessage(text);
    QString color = "";
    switch (type) {
        case Error:
            color = "red";
            break;
        case Success:
            color = "green";
            break;
        default:
            color = "#007AFF";
            break;
    }
    ui->statusBar->setStyleSheet("color: " + color);
}

void MainWindow::on_btnLoad_clicked()
{
    setStatus("");
    bool haveInfo = true;
    if (ui->lEditPostgresIP->text().isEmpty()) {
        ui->lEditPostgresIP->setStyleSheet("border: 1px solid red");
        haveInfo = false;
    }
    if (!ui->lEditPostgresIP->hasAcceptableInput())
        haveInfo = false;
    if (ui->lEditUsername->text().isEmpty()) {
        ui->lEditUsername->setStyleSheet("border: 1px solid red");
        haveInfo = false;
    }
    if (ui->lEditPassword->text().isEmpty()) {
        ui->lEditPassword->setStyleSheet("border: 1px solid red");
        haveInfo = false;
    }
    if (haveInfo) {
        ui->btnLoad->setEnabled(false);
        //Do work
        ui->btnLoad->setEnabled(true);
        ui->btnCreate->setEnabled(ui->listDBs->count());
    }
    else
        setStatus("Please fill in the missing fields", Error);
}

void MainWindow::on_btnCreate_clicked()
{
    setStatus("");
    bool canCreate = true;
    if (ui->listDBs->count() == 0 || ui->listDBs->selectedItems().count() == 0) {
        ui->listDBs->setStyleSheet("border: 1px solid red");
        canCreate = false;
        setStatus("Select at least one shop to continue", Error);
    }
    else if (ui->lEditNemposFePath->text().isEmpty()) {
        ui->lEditNemposFePath->setStyleSheet("border: 1px solid red");
        canCreate = false;
        setStatus("Select NemPosFE path to continue", Error);
    }
    if (canCreate) {
        ui->btnLoad->setEnabled(false);
        ui->btnCreate->setEnabled(false);
        //Do work
        ui->btnLoad->setEnabled(true);
        ui->btnCreate->setEnabled(true);
    }
}

void MainWindow::on_btnBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    if (dir != "") {
        ui->lEditNemposFePath->setText(dir);
        ui->lEditNemposFePath->setStyleSheet("");
    }
}

void MainWindow::on_lineEditIP_textChanged(const QString&)
{
    QObject *source = sender();
    QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(source);
    if (lineEdit) {
        if (lineEdit->text().length() == 0)
            lineEdit->setStyleSheet("");
        else if (lineEdit->hasAcceptableInput())
            lineEdit->setStyleSheet("border: 1px solid green");
        else
            lineEdit->setStyleSheet("border: 1px solid red");
    }
}

void MainWindow::on_lEditCredential_textChanged(const QString&)
{
    QObject *source = sender();
    QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(source);
    if (lineEdit)
        lineEdit->setStyleSheet("");
}
