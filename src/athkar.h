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


#ifndef ATHKAR_HEADER
#define ATHKAR_HEADER

#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QTimer>

#include <KIcon>
#include <KFontComboBox>
#include <Plasma/PopupApplet>
#include <plasma/widgets/textbrowser.h>
#include <plasma/widgets/toolbutton.h>
#include <Solid/Networking>

#include "database.h"
#include "wathakker.h"
#include "hisnmuslim.h"
#include "voice.h"
#include "ui_configform.h"
#include "ui_dbform.h"

#define LOCAL_SOURCE 0
#define WATHAKKER_SOURCE 1
#define HISNMUSLIM_SOURCE 2

class Athkar : public Plasma::PopupApplet
{
    Q_OBJECT
public:
    Athkar(QObject *parent, const QVariantList &args);
    ~Athkar();

    void init();
    void constructForm();
    //void readConfig();
    void writeConfig();
    void displayThikr(QString thikr);
    void getFromWathakkir();

signals:
    void settingsChanged(bool);

public slots:
    void randomThkir();
    void nextClicked();
    void prevClicked();
    void copyClicked();
	void voiceClicked();
    void configAccepted();
    void configChanged();
    void timeChanged(const QTime& time);
    void modified();
    void showNewThir(QString);
    void solidStatusChanged(Solid::Networking::Status status);
	void enableVoiceBtn(int, QProcess::ExitStatus);

protected:
    void createConfigurationInterface(KConfigDialog *parent);

private:
    int m_delay;
    int m_langIndex;
    int m_wathakkerLang;
    QString m_langText;
	bool m_showMerits;
    QFont m_font;
    QColor m_textColor;
    bool m_customFont;
    int m_customFontSize;
    bool m_useThemeColor;
    bool m_useNoColor;
    int m_thikrSource;
    bool m_useDouaa;
    bool m_useHadith;
    bool m_useHikma;
	QString m_command;
	bool m_cmdFestival;
	bool m_cmdEspeak;
    int m_connectionErrTry;
	
    Plasma::TextBrowser *m_text;
    Plasma::ToolButton *m_nextButton;
    Plasma::ToolButton *m_prevButton;
    Plasma::ToolButton *m_copyButton;
	Plasma::ToolButton *m_voiceButton;
    KIcon m_icon;

    QGraphicsLinearLayout *m_layout;
    QTimer m_timer;

    Ui::ConfigForm m_configUi;
    Ui::DatabaseForm m_databaseUi;
    WathakkerForm *m_wathakkerForm;
	VoiceForm *m_voiceForm;

    Database *db;
    HisnMuslim *HMdb;
};

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(athkar, Athkar)

#endif
