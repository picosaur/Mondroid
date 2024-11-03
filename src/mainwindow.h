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

private slots:
    void onRunStateChanged(int state);
    void onApplyConfRequested();

private:
    Ui::MainWindow *ui{};
    Toolbar *m_toolbar{};
    GridWidget *m_gridWidget{};

    void startStop();
};

#endif // MAINWINDOW_H
