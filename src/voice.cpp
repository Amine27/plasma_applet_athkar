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

#include "voice.h"
#include <KGlobal>
#include <KStandardDirs>

VoiceForm::VoiceForm(QWidget *parent) :
        QWidget(parent)
{
    m_ui.setupUi(this);
	m_ui.labelIcon->setPixmap(KIcon("dialog-information").pixmap(64, 64));
	
    checkProgram();
}

QString VoiceForm::commandLine() 
{
	QString command;
	
	if (m_ui.radioFestival->isChecked())
		command = "echo '$TEXT' | text2wave | aplay";

    else if (m_ui.radioEspeak->isChecked())
        command = "espeak '$TEXT'";

    else
        command = "";
	
    return command;
}

void VoiceForm::checkProgram()
{
    if (!KStandardDirs::findExe("festival").isEmpty())
	{
        m_ui.radioFestival->setEnabled(true);
        m_ui.labelFestival->setText(i18n("found"));
    }
    else 
	{
        m_ui.labelFestival->setText(i18n("not found... please install it"));
        m_ui.radioFestival->setEnabled(false);
    }
	
    if (!KStandardDirs::findExe("espeak").isEmpty())
	{
        m_ui.labelEspeak->setText(i18n("found"));
        m_ui.radioFestival->setEnabled(true);
    }
    else
	{
        m_ui.labelEspeak->setText(i18n("not found... please install it"));
        m_ui.radioEspeak->setEnabled(false);
    }
}

void VoiceForm::changeCmdProg(bool enable)
{
	m_ui.radioFestival->setChecked(enable);
	m_ui.radioEspeak->setChecked(!enable);
}

bool VoiceForm::useFestival()
{
	return m_ui.radioFestival->isChecked();
}

bool VoiceForm::useEspeak()
{
	return m_ui.radioEspeak->isChecked();
}