/***************************************************************************
 *   Copyright (C) 2009-2013 by Amine Roukh     <amineroukh@gmail.com>     *
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


#ifndef HISNMUSLIM_H
#define HISNMUSLIM_H

#include <QSqlDatabase>
#include <QDir>
#include <QString>

class HisnMuslim
{
public:
    HisnMuslim();

    bool checkHisnMuslimDB();
    bool makeConnection();
	void closeConnection();
    QString getThikr(bool showSources);
    QString readThikr(int thikrNo);
    QString getNextThikr();
    QString getPrevThikr();
    int getAthkarCount();
    //int getLangStart();
    //int getLangEnd();
    //QStringList getLang();

private:
    QSqlDatabase db;
    QFile dbFilePath;
    QString m_finalText;
    QString m_finalLink;
	QString m_lang;
    int m_currentThikrId;
    int m_min;
    int m_max;
	bool m_showSources;
};

#endif
