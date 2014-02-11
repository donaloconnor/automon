/*
    ==================================================================================================
    |  automonapp.cpp                                                                                |
    |  Part of the Automon application.                                                              |
    |                                                                                                |
    |  Final Year Project - "An Embedded Automotive Monitoring Device"                               |
    |                                                                                                |
    |  By Donal O' Connor for completion of B.Sc (Hons) Software Development and Computer Networking |
    |  Email: donaloconnor@gmail.com                                                                 |
    |  Website/Blog: http://automon.killarneyonline.eu                                               |
    |                                                                                                |
    | Cork Institute of Technology, Cork, Ireland - http://www.cit.ie/                               |
    |                                                                                                |
    | Copyright © 2009 Donal O'Connor <donaloconnor@gmail.com>                                       |
    ==================================================================================================

    Implementation file of the AutomonApp class. It's purpose is to paint a GUI to the screen and construct
    all the neccessary widgets

*/

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplashScreen>
#include <QStackedWidget>
#include <QMessageBox>
#include "automonapp.h"

AutomonApp::AutomonApp(QSplashScreen * splashScreen)
{

    m_errorHandler = new ErrorHandler(); /* Create an error handler dialog. Simply display error  */

    /* Set up all the layouts */
    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignTop);

    m_centerLayout = new QVBoxLayout();
    m_centerLayout->setMargin(10);
    m_centerLayout->setSpacing(0);
    m_centerLayout->setAlignment(Qt::AlignTop);

    m_bottomLayout = new QHBoxLayout();
    m_bottomLayout->setMargin(0);
    m_bottomLayout->setSpacing(0);
    m_bottomLayout->setAlignment(Qt::AlignTop);

    m_topLayout = new QHBoxLayout();

    m_mainLayout->addLayout(m_topLayout);
    m_mainLayout->addLayout(m_centerLayout);
    m_mainLayout->addLayout(m_bottomLayout);

    /* Create a MenuWidget object. This is the menu you see on top of the screen */
    m_menu = new MenuWidget();

    /* Assign the splashscreen pointer to a member variable for reference */
    m_splashScreen = splashScreen;

    /* Display initialization message on Splash Screen */
    m_splashScreen->showMessage("Begining Initialisation Stage...", Qt::AlignCenter, Qt::white);

    /* Set the layout of this main widget to the main layout */
    setLayout(m_mainLayout);

    /* Start initialization stage. This is where the Automon kernel is created and all ECU ->ELM327 connectivity set up */
    init();
    
    /* Set the status to be a label, initializing to a message doing nothing */
    m_status = new QLabel("<strong>Status:</strong> Doing Nothing");
    m_status->setStyleSheet("background:transparent; color:#000000; padding-bottom:30px;");
    m_status->setFixedWidth(550);

    /* Create all the widgets/views passing on the kernel pointer and making this the parent widget */
    m_acellerationTestWidget = new AccelerationTestWidget(m_automonKernel,this);
    m_diagnosticsWidget = new DiagnosticsWidget(m_automonKernel,this);
    m_monitoringWidget = new MonitoringWidget(m_automonKernel,this);
    m_dashboardWidget = new DashboardWidget(m_automonKernel,this);
//    m_carDetailsWidget = new CarDetailsWidget(m_automonKernel,this); [LA]
    m_ruleEditorWidget = new RuleEditorWidget(m_automonKernel,this);

    /* Create the stacked widget. This will hold all our widgets stacked, and only one can be seen at a time */
    m_panes = new QStackedWidget();
    m_panes->setFixedHeight(350);

    /* Add all our widgets to the stack */
    m_panes->addWidget(m_monitoringWidget);
    m_panes->addWidget(m_dashboardWidget);
    m_panes->addWidget(m_diagnosticsWidget);
    m_panes->addWidget(m_acellerationTestWidget);
//    m_panes->addWidget(m_carDetailsWidget);
    m_panes->addWidget(m_ruleEditorWidget);

    /* Connect the appropiate buttons to the appropiate widgets so when we click them, we get correct widget shown */
    m_menu->connectButtonToPanel("MonitoringButton", 0, this);
    m_menu->connectButtonToPanel("DigitalDashboardButton", 1, this);
    m_menu->connectButtonToPanel("DiagnosticsButton", 2, this);
    m_menu->connectButtonToPanel("AccelerationTestButton", 3, this);
    m_menu->connectButtonToPanel("CarDetailsButton", 4, this);


    /* Connect all widget's status update signals to the status update slot of this widget. */
    connect(m_monitoringWidget, SIGNAL(changeStatus(QString)), this, SLOT(setStatus(QString)));
    connect(m_dashboardWidget, SIGNAL(changeStatus(QString)), this, SLOT(setStatus(QString)));
    connect(m_diagnosticsWidget, SIGNAL(changeStatus(QString)), this, SLOT(setStatus(QString)));
    connect(m_acellerationTestWidget, SIGNAL(changeStatus(QString)), this, SLOT(setStatus(QString)));
//    connect(m_carDetailsWidget, SIGNAL(changeStatus(QString)), this, SLOT(setStatus(QString))); [LA]
    connect(m_ruleEditorWidget, SIGNAL(changeStatus(QString)), this, SLOT(setStatus(QString)));

    /* Needed for rule editor: */
    connect(m_monitoringWidget, SIGNAL(changePanel(int)), this, SLOT(changePanel(int)));
    connect(m_ruleEditorWidget, SIGNAL(refreshRules()), m_monitoringWidget, SLOT(refreshRules()));

    /* Add widggets to layouts */
    m_topLayout->addWidget(m_menu);
    m_centerLayout->addWidget(m_panes);
    m_bottomLayout->addWidget(m_status);


    /* Set this object to the MainWindow for stylesheet purposes */
    setObjectName(QString("MainWindow"));

    /* Show in full screen */
    // [LA]
#if defined(Q_OS_ANDROID)
    showFullScreen();
#else
    setFixedSize(800, 480); // Don't maximize the screen on a PC
#endif
}


void AutomonApp::setStatus(const QString & statusText)
{
    /* This slot is responsible for updating the status text so user knows what is happening */
    m_status->setText("<strong>Status:</strong> " + statusText);
}

void AutomonApp::changePanel(int panelID)
{
    /*
        This slot is responsible for changing the panel/widget that is viewed on the widget stack.
        Menu button's signals are connected to this
    */
    m_panes->setCurrentIndex(panelID);
}


void AutomonApp::init()
{
    /*
        This is responsible for attempting to create an AutomonKernel session and set up ECU/ELM327.
        Any exceptions thrown will be caught and the exception handler will display them to user if they occur
    */

    try
    {
        /* Create the AutomonKernel */
        m_automonKernel = new Automon("/dev/ttyUSB0");

        /* Connect the update status of the Kernel to the Splash screen so know what is happening in the kernel */
        connect(m_automonKernel, SIGNAL(updateStatus(QString,int,QColor)), m_splashScreen, SLOT(showMessage(QString,int,QColor)));

        /* Initialize the kernel, connecting to ECU/ preparing ELM327 etc */
        m_automonKernel->init();

    }
    catch (exception & e)
    {
        /* An exception occured. Print it out to the screen to user */
        qCritical() << "Exception: " << e.what();
        m_splashScreen->showMessage(QString(QString(e.what()) + QString("\n Exiting...")), Qt::AlignCenter, Qt::red);
        m_errorHandler->errorSlot(e.what());

        /* After the errorSlot called, the whole application will restart */
    }
}

void AutomonApp::closeEvent(QCloseEvent *event)
{

}
