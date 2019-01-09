#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <sstream>

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "simplecrypt.h"

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

    connect(ui->lEditServerIP, QLineEdit::textChanged, this,
            MainWindow::on_lEditPostgresIP_textChanged);
    connect(ui->lEditPassword, QLineEdit::textChanged, this,
            MainWindow::on_lEditUsername_textChanged);

    QStringList labels;
    labels << "Database" << "Company" << "Shop" << "Exists" << "Create";
    ui->tableShops->setColumnCount(labels.size());
    ui->tableShops->setHorizontalHeaderLabels(labels);
    ui->tableShops->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_settingsFile = QDir::cleanPath(QApplication::applicationDirPath()
                                     + QDir::separator() + "NemPosFE_Config.ini");
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateShopsTable()
{
    try {
        std::shared_ptr< taopq::connection > conn = makeConnection("postgres");
        if (conn->is_open()) {
            setStatus("Connected to " +
                      QString::fromStdString(connectionUrl("postgres")), Success);

            std::vector<std::string> dbs = getDatabases(conn);
            if (!dbs.size())
                setStatus("No dev database found");
            else {
                setStatus(QString::number(dbs.size()) + " dev databases found", Info);
                std::vector<MainWindow::Shop> shops = getAllShops(dbs);
                saveSettings();

                ui->tableShops->setRowCount(0);
                ui->tableShops->setRowCount(shops.size());

                Shop *shop;
                for (size_t i = 0; i < shops.size(); i++) {
                    shop = &(shops[i]);
                    addShopToTable(i, shop->Database, shop->CompanyName,
                                   shop->ShopName, shop->FolderExists);
                }
                setStatus("Shops loaded successfully", Success);
            }

            conn->close();
            if (conn->is_open())
                setStatus("Failed to close database connection", Error);
        }
        else
            setStatus("Failed to connect to " +
                      QString::fromStdString(connectionUrl("postgres")), Error);
    } catch (std::runtime_error &e) {
        setStatus("Failed to connect to " +
                  QString::fromStdString(connectionUrl("postgres")), Error);
        QMessageBox msgBox(this);
        msgBox.setText(QString::fromUtf8(e.what()));
        msgBox.exec();
    }
}

void MainWindow::addShopToTable(int index,
                                const std::string &dbname,
                                const std::string &companyName,
                                const std::string &shopName,
                                bool folderExists)
{
    QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(dbname));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->tableShops->setItem(index, 0, item);

    item = new QTableWidgetItem(QString::fromStdString(companyName));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->tableShops->setItem(index, 1, item);

    item = new QTableWidgetItem(QString::fromStdString(shopName));
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    ui->tableShops->setItem(index, 2, item);

    //FolderExists
    QWidget *checkBoxWidget = new QWidget();
    QCheckBox *checkBox = new QCheckBox();
    checkBox->setChecked(folderExists);
    checkBox->setEnabled(false);
    QHBoxLayout *layoutCheckBox = new QHBoxLayout(checkBoxWidget);
    layoutCheckBox->addWidget(checkBox);
    layoutCheckBox->setAlignment(Qt::AlignCenter);
    layoutCheckBox->setContentsMargins(0,0,0,0);
    ui->tableShops->setCellWidget(index, 3, checkBoxWidget);

    //CreateFolder
    checkBoxWidget = new QWidget();
    checkBox = new QCheckBox();
    checkBox->setChecked(false);
    layoutCheckBox = new QHBoxLayout(checkBoxWidget);
    layoutCheckBox->addWidget(checkBox);
    layoutCheckBox->setAlignment(Qt::AlignCenter);
    layoutCheckBox->setContentsMargins(0,0,0,0);
    ui->tableShops->setCellWidget(index, 4, checkBoxWidget);
}

std::vector<std::string> MainWindow::getDatabases(
        std::shared_ptr<taopq::connection> conn)
{
    std::vector<std::string> dbs;

    if (conn->is_open()) {
        conn->prepare("GetDatabases", "SELECT datname FROM pg_database"
                                      " WHERE datistemplate = false"
                                      " AND datname LIKE '%_dev';");

        taopq::result res = conn->execute("GetDatabases");

        if (!res.empty()) {
            for (size_t i = 0; i < res.size(); i++)
                dbs.push_back(res.get(i, i));
        }
    }

    return dbs;
}

std::vector<MainWindow::Shop> MainWindow::getAllShops(const std::vector<std::string> &dbs)
{
    std::vector<MainWindow::Shop> shops;

    for (size_t i = 0; i < dbs.size(); i++) {
        setStatus("Retrieving " + QString::fromStdString(dbs[i]) + " shops");
        std::vector<MainWindow::Shop> curr_shops = getShopsInDatabase(dbs[i]);
        if (curr_shops.size())
            shops.insert(shops.end(), curr_shops.begin(), curr_shops.end());
    }

    return shops;
}

std::vector<MainWindow::Shop> MainWindow::getShopsInDatabase(const std::string &dbname)
{
    std::vector<MainWindow::Shop> shops;

    try {
        std::shared_ptr<taopq::connection> conn = makeConnection(dbname);
        if (conn->is_open()) {
            setStatus("Connected to " +
                      QString::fromStdString(connectionUrl(dbname)), Success);

            conn->prepare("GetShops", "SELECT res_company.name as companyname,"
                                      " scm_shops.name as shopname"
                                      " FROM scm_shops"
                                      " INNER JOIN res_company"
                                      " ON scm_shops.company_id = res_company.id");
            taopq::result res = conn->execute("GetShops");
            if (!res.empty()) {
                for (size_t i = 0; i < res.size(); i++) {
                    Shop shop;
                    shop.Database = dbname;
                    shop.CompanyName = res.get(i, 0);
                    shop.ShopName = res.get(i, 1);
                    shops.push_back(shop);
                }
            }
            conn->close();
            if (conn->is_open())
                setStatus("Failed to close database connection", Error);
        }
        else
            setStatus("Failed to connect to " +
                      QString::fromStdString(connectionUrl(dbname)), Error);
    } catch (std::runtime_error &e) {
        setStatus("Failed to connect to " +
                  QString::fromStdString(connectionUrl(dbname)), Error);
        QMessageBox msgBox(this);
        msgBox.setText(QString::fromUtf8(e.what()));
        msgBox.exec();
    }

    return shops;
}

std::shared_ptr<taopq::connection> MainWindow::makeConnection(const std::string &dbname)
{
    std::string conn_string = makeConnectionString(dbname);
    return taopq::connection::create(conn_string);
}

std::string MainWindow::makeConnectionString(const std::string &dbname)
{
    //hostaddr='' port='' user='' password='' dbname=''
    std::ostringstream ss;
    ss << "hostaddr=" << host() << " port=" << port();
    ss << " user=" << username() << " password=" << password();
    ss << " dbname=" << dbname;
    return ss.str();
}

std::string MainWindow::connectionUrl(const std::string &dbname)
{
    //user@host:port/dbname
    std::ostringstream ss;
    ss << username() << "@" << host() << ":" << port() << "/" << dbname;
    return ss.str();
}

std::string MainWindow::host()
{
    return ui->lEditPostgresIP->text().toStdString();
}

std::string MainWindow::port()
{
    return QString::number(ui->spinPostgresPort->value()).toStdString();
}

std::string MainWindow::username()
{
    return ui->lEditUsername->text().toStdString();
}

std::string MainWindow::password()
{
    return ui->lEditPassword->text().toStdString();
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

void MainWindow::saveSettings()
{
    QSettings settings(m_settingsFile);

    settings.setValue("postgres_ip", ui->lEditPostgresIP->text());
    settings.setValue("postgres_port", ui->spinPostgresPort->value());

    QString user = ui->lEditUsername->text();
    QString pass = ui->lEditPassword->text();

    if (!user.isEmpty() || !pass.isEmpty()) {
        SimpleCrypt crypto(CipherKey);
        if (!user.isEmpty())
            user = crypto.encryptToString(user);
        if (!pass.isEmpty())
            pass = crypto.encryptToString(pass);
    }

    settings.setValue("postgres_user", user);
    settings.setValue("postgres_pass", pass);

    settings.setValue("nemposfe_path", ui->lEditNemposFePath->text());
    settings.setValue("server_ip", ui->lEditServerIP->text());
    settings.setValue("server_port", ui->spinServerPort->value());
    settings.setValue("https", ui->cbHttps->isChecked());
}

void MainWindow::loadSettings()
{
    QSettings settings(m_settingsFile);

    ui->lEditPostgresIP->setText(settings.value("postgres_ip", "").toString());
    ui->spinPostgresPort->setValue(settings.value("postgres_port", 5432).toInt());

    QString user = settings.value("postgres_user", "").toString();
    QString pass = settings.value("postgres_pass", "").toString();

    if (!user.isEmpty() || !pass.isEmpty()) {
        SimpleCrypt crypto(CipherKey);
        if (!user.isEmpty())
            user = crypto.decryptToString(user);
        if (!pass.isEmpty())
            pass = crypto.decryptToString(pass);
    }

    ui->lEditUsername->setText(user);
    ui->lEditPassword->setText(pass);

    ui->lEditNemposFePath->setText(settings.value("nemposfe_path", "").toString());
    ui->lEditServerIP->setText(settings.value("server_ip", "").toString());
    ui->spinServerPort->setValue(settings.value("server_port", 443).toInt());
    ui->cbHttps->setChecked(settings.value("https", true).toBool());
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
        populateShopsTable();
        ui->btnLoad->setEnabled(true);
        ui->btnCreate->setEnabled(ui->tableShops->rowCount());
    }
    else
        setStatus("Please fill in the missing fields", Error);
}

void MainWindow::on_btnCreate_clicked()
{
    setStatus("");
    bool canCreate = true;
    if (ui->tableShops->rowCount() == 0 /*|| ui->listDBs->selectedItems().count() == 0*/) {
        ui->tableShops->setStyleSheet("border: 1px solid red");
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

void MainWindow::on_lEditPostgresIP_textChanged(const QString&)
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

void MainWindow::on_lEditUsername_textChanged(const QString&)
{
    QObject *source = sender();
    QLineEdit *lineEdit = dynamic_cast<QLineEdit*>(source);
    if (lineEdit)
        lineEdit->setStyleSheet("");
}
