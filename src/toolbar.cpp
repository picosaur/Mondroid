#include "toolbar.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QSpacerItem>
#include <QSpinBox>

Toolbar::Toolbar(QWidget *parent)
    : QToolBar(parent)
{
    m_runInp = new QCheckBox("Discover");
    m_rowsInp = new QSpinBox();
    m_colsInp = new QSpinBox();
    m_hostInp = new QLineEdit();
    m_portInp = new QSpinBox();
    m_scaleInp = new QSpinBox();
    m_rateInp = new QSpinBox();
    m_fastInp = new QCheckBox("Fast");

    m_rowsInp->setFixedSize(60, 30);
    m_rowsInp->setMinimum(1);
    m_rowsInp->setMaximum(9);
    m_rowsInp->setValue(2);

    m_colsInp->setFixedSize(60, 30);
    m_colsInp->setMinimum(1);
    m_colsInp->setMaximum(9);
    m_colsInp->setValue(4);

    m_hostInp->setText("127.0.0.1");
    m_hostInp->setFixedSize(150, 30);

    m_portInp->setMinimum(1);
    m_portInp->setMaximum(65535);
    m_portInp->setValue(5037);
    m_portInp->setFixedSize(60, 30);

    m_scaleInp->setMinimum(1);
    m_scaleInp->setMaximum(999);
    m_scaleInp->setSuffix("%");
    m_scaleInp->setSingleStep(10);
    m_scaleInp->setValue(100);
    m_scaleInp->setFixedSize(60, 30);

    m_rateInp->setMinimum(1);
    m_rateInp->setMaximum(999);

    // Host Port
    addWidget(new QLabel("Host"));
    addWidget(m_hostInp);
    addWidget(new QLabel("Port"));
    addWidget(m_portInp);
    addWidget(m_runInp);
    // Rows Cols
    addSeparator();
    addWidget(new QLabel("Rows"));
    addWidget(m_rowsInp);
    addWidget(new QLabel("Cols"));
    addWidget(m_colsInp);
    // Scale Rate Fast
    addSeparator();
    addWidget(new QLabel("Scale"));
    addWidget(m_scaleInp);
    addWidget(new QLabel("Rate"));
    addWidget(m_rateInp);
    addWidget(m_fastInp);

    connect(m_runInp, SIGNAL(stateChanged(int)), this, SIGNAL(confChanged()));
    connect(m_rowsInp, SIGNAL(valueChanged(int)), this, SIGNAL(confChanged()));
    connect(m_colsInp, SIGNAL(valueChanged(int)), this, SIGNAL(confChanged()));
    //connect(m_hostInp, SIGNAL(valueChanged(int)), this, SIGNAL(confChanged()));
    connect(m_portInp, SIGNAL(valueChanged(int)), this, SIGNAL(confChanged()));
    connect(m_scaleInp, SIGNAL(valueChanged(int)), this, SIGNAL(confChanged()));
    connect(m_rateInp, SIGNAL(valueChanged(int)), this, SIGNAL(confChanged()));
    connect(m_fastInp, SIGNAL(stateChanged(int)), this, SIGNAL(confChanged()));
}

Toolbar::~Toolbar() {}

bool Toolbar::discover() const
{
    return int(m_runInp->isChecked());
}

QString Toolbar::host() const
{
    return m_hostInp->text();
}

int Toolbar::port() const
{
    return m_portInp->value();
}

int Toolbar::rows() const
{
    return m_rowsInp->value();
}

int Toolbar::cols() const
{
    return m_colsInp->value();
}

int Toolbar::scale() const
{
    return m_scaleInp->value();
}

int Toolbar::rate() const
{
    return m_rateInp->value();
}

bool Toolbar::fast() const
{
    return m_fastInp->isChecked();
}

GridConf Toolbar::gridConf() const
{
    GridConf conf;
    conf.discover = discover();
    conf.host = host();
    conf.port = port();
    conf.rows = rows();
    conf.cols = cols();
    conf.scale = scale();
    conf.rate = rate();
    conf.fast = fast();
    return conf;
}

void Toolbar::saveState(QSettings &settings) const
{
    settings.setValue("toolbar/host", host());
    settings.setValue("toolbar/port", port());
    settings.setValue("toolbar/rows", rows());
    settings.setValue("toolbar/cols", cols());
    settings.setValue("toolbar/scale", scale());
    settings.setValue("toolbar/rate", rate());
    settings.setValue("toolbar/fast", fast());
}

void Toolbar::loadState(const QSettings &settings)
{
    m_hostInp->setText(settings.value("toolbar/host", "127.0.0.1").toString());
    m_portInp->setValue(settings.value("toolbar/port", 5037).toInt());
    m_rowsInp->setValue(settings.value("toolbar/rows", 2).toInt());
    m_colsInp->setValue(settings.value("toolbar/cols", 4).toInt());
    m_scaleInp->setValue(settings.value("toolbar/scale", 1).toInt());
    m_rateInp->setValue(settings.value("toolbar/rate", 1).toInt());
    m_fastInp->setChecked(settings.value("toolbar/fast", false).toBool());
}
