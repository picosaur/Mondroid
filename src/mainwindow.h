#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class GridWidget;
class Toolbar;
class Statusbar;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
    void onGridConfChanged();
    void onCellConfChanged();

private:
    Ui::MainWindow *ui{};
    Toolbar *m_toolbar{};
    Statusbar *m_statusbar{};
    GridWidget *m_gridWidget{};
};

#endif // MAINWINDOW_H
