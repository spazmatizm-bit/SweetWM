#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QStyle>
#include <QScreen>
#include <QTimer>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QKeyEvent>
#include <cstdlib>

struct App {
    QString name;
    QString exec;
};

class MenuWindow : public QMainWindow {
private:
    QLineEdit *searchEdit;
    QListWidget *appList;
    QPushButton *shutdownBtn;
    QPushButton *rebootBtn;
    QPushButton *logoutBtn;
    QList<App> apps;
    QList<App> filteredApps;
    bool menuVisible;

    void loadApps() {
        apps.clear();
        QDir dir("/usr/share/applications");
        QStringList filters;
        filters << "*.desktop";
        dir.setNameFilters(filters);

        for (const QString &file : dir.entryList()) {
            QString path = dir.absolutePath() + "/" + file;
            QFile f(path);
            if (!f.open(QIODevice::ReadOnly)) continue;

            QTextStream stream(&f);
            QString name, exec;
            bool inEntry = false;

            while (!stream.atEnd()) {
                QString line = stream.readLine().trimmed();
                
                if (line.startsWith("[Desktop Entry]")) {
                    inEntry = true;
                    continue;
                }
                if (!inEntry) continue;
                if (line.startsWith("[")) break;
                if (line.startsWith("#")) continue;

                if (line.startsWith("Name=")) {
                    name = line.mid(5).trimmed();
                    if (name.contains('[')) name.clear();
                }
                if (line.startsWith("Exec=")) {
                    exec = line.mid(5).trimmed();
                    exec.remove(QRegularExpression(" %[UuFf]"));
                    exec.remove(QRegularExpression(" %."));
                }
            }
            f.close();

            if (!name.isEmpty() && !exec.isEmpty()) {
                App app;
                app.name = name;
                app.exec = exec;
                apps.append(app);
            }
        }

        std::sort(apps.begin(), apps.end(), [](const App &a, const App &b) {
            return a.name.toLower() < b.name.toLower();
        });
        printf("Loaded %d apps\n", apps.size());
    }

    void updateList() {
        appList->clear();
        int count = 0;
        for (const App &app : filteredApps) {
            if (count >= 100) break;
            appList->addItem(app.name);
            count++;
        }
        if (filteredApps.isEmpty()) {
            appList->addItem("No apps found");
        }
    }

    void runApp(const QString &exec) {
        hide();
        QProcess::startDetached("/bin/sh", QStringList() << "-c" << exec);
    }

    void setupUI() {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Popup | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_ShowWithoutActivating);
        
        QColor bgColor(45, 27, 61);
        QColor accentColor(155, 77, 202);
        QColor textColor(225, 190, 231);
        QColor hoverColor(74, 43, 93);
        QColor closeColor(123, 47, 190);
        QColor minColor(106, 27, 154);
        QColor maxColor(155, 77, 202);

        QWidget *central = new QWidget(this);
        central->setStyleSheet(QString(
            "QWidget { background: #%1; border-radius: 8px; }"
            "QLineEdit { background: #1A0F24; color: #E1BEE7; border: 2px solid #9B4DCA; border-radius: 4px; padding: 6px; font-size: 13px; }"
            "QLineEdit:focus { border: 2px solid #B39DDB; }"
            "QListWidget { background: #2D1B3D; color: #E1BEE7; border: none; outline: none; font-size: 12px; }"
            "QListWidget::item { padding: 6px 10px; border-radius: 4px; }"
            "QListWidget::item:selected { background: #9B4DCA; color: #FFFFFF; }"
            "QListWidget::item:hover { background: #4A2B5D; }"
            "QPushButton { border: none; border-radius: 4px; padding: 8px; font-size: 12px; font-weight: bold; color: #E1BEE7; }"
            "QPushButton#shutdown { background: #7B2FBE; }"
            "QPushButton#shutdown:hover { background: #9B4DCA; }"
            "QPushButton#reboot { background: #6A1B9A; }"
            "QPushButton#reboot:hover { background: #9B4DCA; }"
            "QPushButton#logout { background: #9B4DCA; }"
            "QPushButton#logout:hover { background: #B39DDB; }"
        ).arg(bgColor.rgb(), 0, 16));

        QVBoxLayout *layout = new QVBoxLayout(central);
        layout->setSpacing(8);
        layout->setContentsMargins(12, 12, 12, 12);

        searchEdit = new QLineEdit();
        searchEdit->setPlaceholderText("Search apps...");
        searchEdit->setFocus();
        layout->addWidget(searchEdit);

        appList = new QListWidget();
        appList->setVerticalScrollMode(QListWidget::ScrollPerPixel);
        appList->setMinimumHeight(300);
        layout->addWidget(appList);

        QHBoxLayout *powerLayout = new QHBoxLayout();
        powerLayout->setSpacing(6);

        shutdownBtn = new QPushButton("Shutdown");
        shutdownBtn->setObjectName("shutdown");
        powerLayout->addWidget(shutdownBtn);

        rebootBtn = new QPushButton("Reboot");
        rebootBtn->setObjectName("reboot");
        powerLayout->addWidget(rebootBtn);

        logoutBtn = new QPushButton("Logout");
        logoutBtn->setObjectName("logout");
        powerLayout->addWidget(logoutBtn);

        layout->addLayout(powerLayout);

        setCentralWidget(central);
        setFixedSize(380, 520);

        // Используем лямбды для сигналов
        connect(searchEdit, &QLineEdit::textChanged, [this](const QString &text) {
            filteredApps.clear();
            if (text.isEmpty()) {
                filteredApps = apps;
            } else {
                for (const App &app : apps) {
                    if (app.name.contains(text, Qt::CaseInsensitive)) {
                        filteredApps.append(app);
                    }
                }
            }
            updateList();
        });

        connect(appList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem*) {
            int idx = appList->currentRow();
            if (idx >= 0 && idx < filteredApps.size()) {
                runApp(filteredApps[idx].exec);
            }
        });

        connect(appList, &QListWidget::itemClicked, [this](QListWidgetItem*) {
            int idx = appList->currentRow();
            if (idx >= 0 && idx < filteredApps.size()) {
                runApp(filteredApps[idx].exec);
            }
        });

        connect(shutdownBtn, &QPushButton::clicked, [this]() {
            hide();
            QProcess::startDetached("systemctl", QStringList() << "poweroff");
        });

        connect(rebootBtn, &QPushButton::clicked, [this]() {
            hide();
            QProcess::startDetached("systemctl", QStringList() << "reboot");
        });

        connect(logoutBtn, &QPushButton::clicked, [this]() {
            hide();
            QProcess::startDetached("/bin/sh", QStringList() << "-c" << "pkill -KILL -u $USER");
        });

        searchEdit->installEventFilter(this);
        appList->installEventFilter(this);

        loadApps();
        filteredApps = apps;
        updateList();
        menuVisible = false;
    }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            
            if (keyEvent->key() == Qt::Key_Escape) {
                hide();
                return true;
            }
            
            if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
                QApplication::sendEvent(appList, keyEvent);
                return true;
            }
            
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                int idx = appList->currentRow();
                if (idx >= 0 && idx < filteredApps.size()) {
                    runApp(filteredApps[idx].exec);
                }
                return true;
            }

            if (keyEvent->key() == Qt::Key_Tab) {
                if (appList->hasFocus()) {
                    searchEdit->setFocus();
                } else {
                    appList->setFocus();
                }
                return true;
            }
        }
        return QMainWindow::eventFilter(obj, event);
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Escape) {
            hide();
            return;
        }
        QMainWindow::keyPressEvent(event);
    }

    void showEvent(QShowEvent *event) override {
        QMainWindow::showEvent(event);
        searchEdit->setFocus();
        searchEdit->selectAll();
        menuVisible = true;
        
        QRect screen = QApplication::primaryScreen()->geometry();
        int x = 5;
        int y = 30;
        
        if (x + width() > screen.width()) {
            x = screen.width() - width() - 5;
        }
        if (y + height() > screen.height()) {
            y = screen.height() - height() - 5;
        }
        if (x < 0) x = 5;
        if (y < 0) y = 5;
        
        move(x, y);
    }

    void hideEvent(QHideEvent *event) override {
        menuVisible = false;
        QMainWindow::hideEvent(event);
    }

    void focusOutEvent(QFocusEvent *event) override {
        QTimer::singleShot(50, this, [this]() {
            if (!isActiveWindow()) {
                hide();
            }
        });
        QMainWindow::focusOutEvent(event);
    }

public:
    MenuWindow(QWidget *parent = nullptr) : QMainWindow(parent), menuVisible(false) {
        setupUI();
    }

    ~MenuWindow() {}

    void showMenu() {
        if (isVisible()) {
            hide();
            return;
        }
        show();
        raise();
        activateWindow();
        setFocus();
        searchEdit->setFocus();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setQuitOnLastWindowClosed(true);

    MenuWindow menu;
    
    QTimer::singleShot(10, &menu, &MenuWindow::showMenu);
    
    return app.exec();
}
