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


#include <QtSql>

#include <KGlobal>
#include <KStandardDirs>
#include <KRandom>

#include "hisnmuslim.h"

HisnMuslim::HisnMuslim()
{
    
}

bool HisnMuslim::checkHisnMuslimDB()
{
    dbFilePath.setFileName(KGlobal::dirs()->findResourceDir("data", "athkar/") + "athkar/hisnmuslim.db");
    
    if ( dbFilePath.exists() )
        return true;

    return false;
}

bool HisnMuslim::makeConnection()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName(dbFilePath.fileName());
    db.setUserName("");
    db.setPassword("");

    if ( db.isOpen() || !db.open())
        return false;
    
    return true;
}

void HisnMuslim::closeConnection()
{
	db.removeDatabase(db.connectionName());
}

QString HisnMuslim::getThikr(bool showSources)
{
	//m_lang = lang;
    m_min = 1;
    m_max = getAthkarCount();

    m_currentThikrId = (KRandom::random() % (m_max - m_min + 1)) + m_min;
	
	m_showSources = showSources;

    return readThikr(m_currentThikrId);
}

QString HisnMuslim::getNextThikr()
{
    if (m_currentThikrId < m_max)
        m_currentThikrId++;
    
    qDebug() << m_currentThikrId << m_max;

    return readThikr(m_currentThikrId);
}

QString HisnMuslim::getPrevThikr()
{
    if (m_currentThikrId > m_min)
        m_currentThikrId--;
    
    qDebug() << m_currentThikrId << m_min;

    return readThikr(m_currentThikrId);
}

QString HisnMuslim::readThikr(int thikrNo)
{
    m_finalText.clear();
    m_finalLink.clear();

    QSqlQuery query;
    query.prepare("SELECT dkr, fnt, bab FROM thikr, bab WHERE thikr.dnkr = ? AND bab.babn = thikr.babn");
    //query.addBindValue(m_lang);
    query.addBindValue(QString::number(thikrNo));
    query.exec();

    int dkr = query.record().indexOf("dkr");
    int fnt = query.record().indexOf("fnt");
	int bab = query.record().indexOf("bab");

    while ( query.next() )
    {
        m_finalText.append(query.value(bab).toString() +" :<br /><br />");

        m_finalText.append("\""+query.value(dkr).toString() +"\"<br /><br />");
		
		if(m_showSources && !query.value(fnt).toString().isEmpty())
		{		 
			m_finalText.append("<em>"+query.value(fnt).toString()+"</em>");
		}
    }

    return m_finalText;
}

int HisnMuslim::getAthkarCount()
{
    QSqlQuery query;
    query.exec("SELECT COUNT(dnkr) FROM thikr WHERE dnkr <> '0'");

    query.next();
    int count = query.value(0).toInt();

    return count;
}

/*int HisnMuslim::getLangEnd()
{
    QSqlQuery query;
    query.prepare("SELECT rowid FROM monajat WHERE lang=? ORDER BY rowid DESC LIMIT 1");
    query.addBindValue(m_lang);
    query.exec();

    query.next();
    int end = query.value(0).toInt();

    return end;
}*/

/*QStringList HisnMuslim::getLang()
{
    QSqlQuery query;
    query.exec("SELECT DISTINCT lang FROM monajat");

    QString lang;

    while (query.next())
    {
        lang.append(query.value(0).toString());
        lang.append("\n");
    }

    QStringList langList = lang.split("\n");
    langList.removeLast();

    return langList;
}*/
