#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "pq.h"

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

    struct Shop {
        std::string Database, CompanyName, ShopName;
        bool FolderExists, CreateFolder;
    };

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnLoad_clicked();
    void on_btnCreate_clicked();
    void on_btnBrowse_clicked();
    void on_lEditPostgresIP_textChanged(const QString&);
    void on_lEditUsername_textChanged(const QString&);

private:
    void populateShopsTable();
    void addShopToTable(int index, const std::string &dbname,
                        const std::string &companyName, const std::string &shopName,
                        bool folderExists);

    std::vector<std::string> getDatabases(std::shared_ptr<taopq::connection> conn);
    std::vector<Shop> getAllShops(const std::vector<std::string> &dbs);
    std::vector<Shop> getShopsInDatabase(const std::string &dbname);
    std::shared_ptr<taopq::connection> makeConnection(const std::string &dbname);
    std::string makeConnectionString(const std::string &dbname);

    std::string connectionUrl(const std::string &dbname);
    std::string host();
    std::string port();
    std::string username();
    std::string password();

    void setStatus(const QString &text, StatusType type = Info);

    void saveSettings();
    void loadSettings();

    Ui::MainWindow *ui;

    QString m_settingsFile;

    const uint64_t CipherKey = Q_UINT64_C(0x5c33e800);
};

#endif // MAINWINDOW_H
