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


#ifndef VOICEFORM_H
#define VOICEFORM_H

#include <QWidget>
#include <KIcon>
#include "ui_voiceform.h"


class VoiceForm : public QWidget
{
    Q_OBJECT
public:
    VoiceForm(QWidget *parent = 0);
    QString commandLine();
	void changeCmdProg(bool cmdProg);
	bool useFestival();
	bool useEspeak();

public slots:
    void checkProgram();

private:
    Ui::VoiceForm m_ui;
};

#endif
