#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum StatusType {
        Error,
        Info,
        Success
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnLoad_clicked();
    void on_btnCreate_clicked();
    void on_btnBrowse_clicked();
    void on_lineEditIP_textChanged(const QString&);
    void on_lEditCredential_textChanged(const QString&);

private:
    void retrieveDatabases();
    void setStatus(const QString &text, StatusType type = Info);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H