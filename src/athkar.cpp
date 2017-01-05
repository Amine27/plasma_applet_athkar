/***************************************************************************
 *   Copyright (C) 2009-2011 by Amine Roukh     <amineroukh@gmail.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QLabel>
#include <QTextCodec>
#include <QLocale>
#include <QTextOption>
#include <QToolButton>

#include <KTextBrowser>
#include <KConfigDialog>
#include <Plasma/Theme>
#include <KProcess>

#include "athkar.h"

Athkar::Athkar(QObject *parent, const QVariantList &args)
        : Plasma::PopupApplet(parent, args)
{
    m_text = new Plasma::TextBrowser(this);
    m_nextButton = new Plasma::ToolButton(this);
    m_prevButton = new Plasma::ToolButton(this);
    m_copyButton = new Plasma::ToolButton(this);
	m_voiceButton = new Plasma::ToolButton(this);
    db = new Database;
    HMdb = new HisnMuslim;

    connect(m_nextButton, SIGNAL(clicked()), this, SLOT(nextClicked()));
    connect(m_prevButton, SIGNAL(clicked()), this, SLOT(prevClicked()));
    connect(m_copyButton, SIGNAL(clicked()), this, SLOT(copyClicked()));
	connect(m_voiceButton, SIGNAL(clicked()), this, SLOT(voiceClicked()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(randomThkir()));

    //connect(Solid::Networking::notifier(), SIGNAL(statusChanged(Solid::Networking::Status)),
            //this, SLOT(solidStatusChanged(Solid::Networking::Status)));

    setBackgroundHints(DefaultBackground);
    setLayoutDirection(Qt::RightToLeft);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
	setAttribute(Qt::WA_DeleteOnClose);
}


Athkar::~Athkar()
{
	db->closeConnection();
    writeConfig();
}

void Athkar::init()
{
    m_timer.stop();
    m_connectionErrTry = 0;

    constructForm();
    configChanged();

    if (!db->checkDatabase())
    {
        qDebug() << "Unable to Read Database File";
        return;
    }

    if (!db->makeConnection())
    {
        qDebug() << "Unable to establish a database connection, this Application needs SQLITE support.";
        return;
    }

    QStringList str = db->getLang();
    
    if(m_langIndex >= 0 && m_langIndex < str.size())
        m_langText = str[m_langIndex]; // FIXME: plasma crach - Done
    else
        m_langText = str[0];

    randomThkir();
}

void Athkar::constructForm()
{
    m_text->nativeWidget()->setOpenExternalLinks(true);

    m_nextButton->setIcon(KIcon("arrow-left.png"));
    m_nextButton->setMaximumSize(QSize(20,18));
    m_nextButton->nativeWidget()->setToolTip(i18n("Next Thikr"));

    m_prevButton->setIcon(KIcon("arrow-right.png"));
    m_prevButton->nativeWidget()->setToolTip(i18n("Previous Thikr"));
    m_prevButton->setMaximumSize(QSize(20,18));

    m_copyButton->setIcon(KIcon("edit-copy"));
    m_copyButton->nativeWidget()->setToolTip(i18n("Copy Thikr"));
    m_copyButton->setMaximumSize(QSize(20,18));
	
	m_voiceButton->setIcon(KIcon("text-speak.png"));
	m_voiceButton->nativeWidget()->setToolTip(i18n("Listen to Thikr"));
	m_voiceButton->setMaximumSize(QSize(20, 20));

    QGraphicsLinearLayout*  layoutControl = new QGraphicsLinearLayout(Qt::Horizontal);
    layoutControl->addItem(m_copyButton);
	layoutControl->addItem(m_voiceButton);
    layoutControl->addStretch();
    layoutControl->addItem(m_prevButton);
    layoutControl->addItem(m_nextButton);

    layoutControl->setMaximumHeight(20);

    QGraphicsLinearLayout* mainLayout = new QGraphicsLinearLayout(Qt::Vertical);

    mainLayout->addItem(m_text);
    mainLayout->addItem(layoutControl);

    m_layout = new QGraphicsLinearLayout(this);
    m_layout->addItem(mainLayout);
    m_layout->setSpacing(0);
    setMinimumSize(QSize(100, 125));
    resize(300,175);
}

void Athkar::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *configForm = new QWidget;
    m_configUi.setupUi(configForm);

    QWidget *databaseForm = new QWidget;
    m_databaseUi.setupUi(databaseForm);
	
	m_voiceForm = new VoiceForm;

    m_wathakkerForm = new WathakkerForm;

    parent->addPage(configForm, i18n("General"), "preferences-system");
    parent->addPage(databaseForm, i18n("Database"), "office-chart-pie");
    parent->addPage(m_wathakkerForm, i18n("Wathakker.net"), "download");
	parent->addPage(m_voiceForm, i18n("Voice"), "preferences-desktop-text-to-speech");

    m_configUi.m_sourceBox->setCurrentIndex(m_thikrSource);
	
    m_configUi.fontStyleComboBox->setCurrentFont(m_font);
    m_configUi.fontBoldCheckBox->setChecked(m_font.bold());
    m_configUi.fontItalicCheckBox->setChecked(m_font.italic());
    m_configUi.customFont->setChecked(m_customFont);
    m_configUi.customFontSizeSpinBox->setEnabled(m_customFont);
    m_configUi.customFontSizeSpinBox->setValue(m_customFontSize);

    m_configUi.textColorButton->setColor(m_textColor);
    m_configUi.useThemeColor->setChecked(m_useThemeColor);
    m_configUi.useCustomColor->setChecked(!m_useThemeColor);

    QTime time(0, 0, 0);
    time = time.addSecs(m_delay);
    m_configUi.m_timeDelay->setTime(time);
    m_configUi.m_timeDelay->setMinimumTime(QTime(0, 1, 0));
    connect(m_configUi.m_timeDelay, SIGNAL(timeChanged(const QTime&)), this, SLOT(timeChanged(const QTime&)));
    connect(m_configUi.m_timeDelay, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(modified()));

    m_databaseUi.m_langList->addItems(db->getLang());
    m_databaseUi.m_langList->setCurrentIndex(m_langIndex);
	m_databaseUi.m_showMeritsBox->setChecked(m_showMerits);

    m_wathakkerForm->setThikrLangage(m_wathakkerLang);
    m_wathakkerForm->changeDouaa(m_useDouaa);
    m_wathakkerForm->changeHadith(m_useHadith);
    m_wathakkerForm->changeHikma(m_useHikma);
	
	m_voiceForm->changeCmdProg(m_cmdFestival);
	//m_voiceForm->changeEspeak(m_cmdEspeak);
	
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
}

void Athkar::modified()
{
    emit settingsChanged(true);
}

void Athkar::configAccepted()
{
    m_thikrSource = m_configUi.m_sourceBox->currentIndex();
	
    m_langIndex = m_databaseUi.m_langList->currentIndex();
    m_langText = m_databaseUi.m_langList->itemText(m_langIndex);
	m_showMerits = m_databaseUi.m_showMeritsBox->isChecked();

    m_font = m_configUi.fontStyleComboBox->currentFont();
    m_font.setBold(m_configUi.fontBoldCheckBox->isChecked());
    m_font.setItalic(m_configUi.fontItalicCheckBox->isChecked());

    m_customFont = m_configUi.customFont->isChecked();
    m_customFontSize = m_configUi.customFontSizeSpinBox->value();

    m_useThemeColor = m_configUi.useThemeColor->isChecked();

    if (!m_customFont)
        m_customFontSize = 9;

    if (m_useThemeColor)
        m_textColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
    else
    {
        m_textColor = m_configUi.textColorButton->color();
    }

    m_font.setPointSize(m_customFontSize);
    m_text->nativeWidget()->setFont(m_font);
    m_text->setStyleSheet("color:" +m_textColor.name());

    m_wathakkerLang = m_wathakkerForm->thikrLangage();
    m_useDouaa = m_wathakkerForm->useDouaa();
    m_useHadith = m_wathakkerForm->useHadith();
    m_useHikma = m_wathakkerForm->useHikma();
	
	m_command = m_voiceForm->commandLine();
	m_voiceForm->checkProgram();
	m_cmdFestival = m_voiceForm->useFestival();
	m_cmdEspeak =m_voiceForm->useEspeak();

    writeConfig();
    randomThkir();
}

void Athkar::configChanged()
{
    KConfigGroup cg = config();

    m_thikrSource = cg.readEntry("thikrSource", 2);
    m_delay = cg.readEntry("thikrTimer", 60);
    m_font = cg.readEntry("font", KGlobalSettings::generalFont());
    m_customFont = cg.readEntry("customFont", true);
    m_customFontSize = cg.readEntry("customFontSize", m_font.pointSize());
    m_useThemeColor = cg.readEntry("useThemeColor", true);

    if (m_useThemeColor)
        m_textColor = Plasma::Theme::defaultTheme()->color(Plasma::Theme::TextColor);
    else
        m_textColor = cg.readEntry("textColor", m_textColor);

    m_font.setPointSize(m_customFontSize);
    m_text->nativeWidget()->setFont(m_font);
    m_text->setStyleSheet("color:"+m_textColor.name());

    m_langIndex = cg.readEntry("lang", 0);
	m_showMerits = cg.readEntry("showMerits", true);

    m_wathakkerLang = cg.readEntry("wathakkerLang", 0);
    m_useDouaa = cg.readEntry("douaa", true);
    m_useHadith = cg.readEntry("hadith", true);
    m_useHikma = cg.readEntry("hikma", true);
	
	m_command = cg.readEntry("cmd");
	m_cmdFestival = cg.readEntry("cmdFestival", false);
	m_cmdEspeak = cg.readEntry("cmdEspeak", false);
}

void Athkar::writeConfig()
{
    KConfigGroup cg = config();

    cg.writeEntry("thikrSource", m_thikrSource);
    cg.writeEntry("thikrTimer", m_delay);
    cg.writeEntry("font", m_font);
    cg.writeEntry("customFont", m_customFont);
    cg.writeEntry("customFontSize", m_customFontSize);
    cg.writeEntry("useThemeColor", m_useThemeColor);
    cg.writeEntry("textColor", m_textColor);

    cg.writeEntry("lang", m_langIndex);
	cg.writeEntry("showMerits", m_showMerits);

    cg.writeEntry("wathakkerLang", m_wathakkerLang);
    cg.writeEntry("douaa", m_useDouaa);
    cg.writeEntry("hadith", m_useHadith);
    cg.writeEntry("hikma", m_useHikma);
	
	cg.writeEntry("cmd", m_command);
	cg.writeEntry("cmdFestival", m_cmdFestival);
	cg.writeEntry("cmdEspeak", m_cmdEspeak);
}

void Athkar::timeChanged(const QTime& time)
{
    m_delay = QTime(0, 0, 0).secsTo(time);
    m_timer.start(m_delay * 1000);
}

void Athkar::solidStatusChanged(Solid::Networking::Status status)
{
    if (status != Solid::Networking::Connected)
    {
        m_connectionErrTry++;
        
        qDebug() << "Networking is disabled. Cannot connect to wathakkir.net. Switch to local database";
        m_thikrSource = LOCAL_SOURCE;
        randomThkir();
        
        if(m_connectionErrTry < 3)
        {
            qDebug() << "Switch to local database temporary" << m_connectionErrTry;
            m_thikrSource = WATHAKKER_SOURCE;
        }
    }
}

void Athkar::randomThkir()
{
    if (m_thikrSource == LOCAL_SOURCE) // Local database
    {
        if (!db->checkDatabase())
        {
            qDebug() << "Unable to Read Database File";
            return;
        }

        if (!db->makeConnection())
        {
            qDebug() << "Unable to establish a database connection, this Application needs SQLITE support.";
            return;
        }
        setBusy(true);
        displayThikr(db->getThikr(m_langText, m_showMerits));
    }

    else if (m_thikrSource == WATHAKKER_SOURCE) // wathakker.net
        getFromWathakkir();
    
    else if(m_thikrSource == HISNMUSLIM_SOURCE)
    {
        if (!HMdb->checkHisnMuslimDB())
        {
            qDebug() << "Unable to Read Database File";
            return;
        }

        if (!HMdb->makeConnection())
        {
            qDebug() << "Unable to establish a database connection, this Application needs SQLITE support.";
            return;
        }
        setBusy(true);
        displayThikr(HMdb->getThikr(true));
    }

    m_timer.start(m_delay * 1000);
}

void Athkar::getFromWathakkir()
{
    setBusy(true);
    m_wathakkerForm = new WathakkerForm;
    m_wathakkerForm->downloadNewThikr(m_wathakkerLang, m_useDouaa, m_useHadith, m_useHikma);

    connect( m_wathakkerForm, SIGNAL( newThikr( QString ) ), this, SLOT( showNewThir( QString ) ) );
}

void Athkar::showNewThir(QString thikr)
{
	//if(thikr.isEmpty())
		//solidStatusChanged(Solid::Networking::Unconnected);
	
	//else
    //{
        m_connectionErrTry = 0;
	    displayThikr(thikr);
    //}
}

void Athkar::prevClicked()
{
    if (m_thikrSource == LOCAL_SOURCE)
    {
        setBusy(true);
        displayThikr(db->getPrevThikr());
    }

    else if (m_thikrSource == WATHAKKER_SOURCE)
        getFromWathakkir();
    
    else if(m_thikrSource == HISNMUSLIM_SOURCE)
    {
        setBusy(true);
        displayThikr(HMdb->getPrevThikr());
    }
}

void Athkar::nextClicked()
{
    qDebug() << m_thikrSource;
    
    if (m_thikrSource == LOCAL_SOURCE)
    {
        setBusy(true);
        displayThikr(db->getNextThikr());
    }

    else if (m_thikrSource == WATHAKKER_SOURCE)
        getFromWathakkir();
    
    else if(m_thikrSource == HISNMUSLIM_SOURCE)
    {
        setBusy(true);
        displayThikr(HMdb->getNextThikr());
    }
}

void Athkar::copyClicked()
{
    m_text->nativeWidget()->selectAll();
    m_text->nativeWidget()->copy();
}

void Athkar::displayThikr(QString thikr)
{
    setBusy(false);

    if ((m_thikrSource == LOCAL_SOURCE && m_langText == "ar") || (m_thikrSource == WATHAKKER_SOURCE && m_wathakkerLang == 0)
        || (m_thikrSource == HISNMUSLIM_SOURCE))
        m_text->nativeWidget()->setLayoutDirection(Qt::RightToLeft);
    else
        m_text->nativeWidget()->setLayoutDirection(Qt::LeftToRight);

    m_text->nativeWidget()->setHtml(thikr);
}

void Athkar::voiceClicked()
{
    // No arabic support
    if ((m_thikrSource == LOCAL_SOURCE && m_langText == "ar") || (m_thikrSource == WATHAKKER_SOURCE && m_wathakkerLang == 0)
        || (m_thikrSource == HISNMUSLIM_SOURCE))
        return;
		
    QString qcommand = m_command;
    qcommand.replace("$TEXT", m_text->nativeWidget()->toPlainText().remove("'"));

	KProcess *proc = new KProcess;
	proc->setShellCommand(qcommand);
	proc->start();
	
	m_voiceButton->setEnabled(false); // to avoid multiple voice in the same time
	connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(enableVoiceBtn(int, QProcess::ExitStatus)));
}

void Athkar::enableVoiceBtn(int exitCode, QProcess::ExitStatus exitStatus)
{
	Q_UNUSED(exitCode);
	Q_UNUSED(exitStatus);
	
	m_voiceButton->setEnabled(true);
}

#include "athkar.moc"
