#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class GridWidget;
class Toolbar;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
    Ui::MainWindow *ui{};
    Toolbar *m_toolbar{};
    GridWidget *m_gridWidget{};
};

#endif // MAINWINDOW_H
