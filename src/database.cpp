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


#include <QtSql>

#include <KGlobal>
#include <KStandardDirs>
#include <KRandom>

#include "database.h"

Database::Database()
{
    
}

bool Database::checkDatabase()
{
    dbFilePath.setFileName(KGlobal::dirs()->findResourceDir("data", "athkar/") + "athkar/data.db");
    
    if ( dbFilePath.exists() )
        return true;

    return false;
}


bool Database::makeConnection()
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

void Database::closeConnection()
{
	db.removeDatabase(db.connectionName());
}

QString Database::getThikr(QString lang, bool showMerits)
{
	m_lang = lang;
    m_min = getLangStart();
    m_max = getLangEnd();

    m_currentThikrId = (KRandom::random() % (m_max - m_min + 1)) + m_min;
	
	m_showMerits = showMerits;

    return readThikr(m_currentThikrId);
}

QString Database::getNextThikr()
{
    if (m_currentThikrId < m_max)
        m_currentThikrId++;

    return readThikr(m_currentThikrId);
}

QString Database::getPrevThikr()
{
    if (m_currentThikrId > m_min)
        m_currentThikrId--;

    return readThikr(m_currentThikrId);
}

QString Database::readThikr(int thikrNo)
{
    m_finalText.clear();
    m_finalLink.clear();

    QSqlQuery query;
    query.prepare("SELECT rowid,* FROM monajat WHERE lang=? AND rowid=? LIMIT 1");
    query.addBindValue(m_lang);
    query.addBindValue(thikrNo);
    query.exec();

    int links = query.record().indexOf("links");
    int text = query.record().indexOf("text");
	int merits = query.record().indexOf("merits");

    while ( query.next() )
    {
        m_finalText.append(query.value(text).toString());

        QStringList linksList = query.value(links).toString().split("\n");
		
		if(m_showMerits && !query.value(merits).toString().isEmpty())
		{
			QString meritsLang;
			
			if (m_lang == "ar")
				meritsLang = "فضائله";
			else if (m_lang == "en")
				meritsLang = "Its Merits";
				 
			m_finalText.append("<br /><br />" + meritsLang + ":<br />" + query.value(merits).toString());
		}

        foreach (QString result, linksList)
        {
            QStringList str = result.split("\t");
            QString url = str[0];
            QString t;

            if (str.size() > 1)
                t = str[1];
            else
                t = url;
			
            m_finalLink.append("<a href=\"" + url + "\">"+ t + "</a><br />");
        }

        m_finalText.append("<br /><br /><em>" + m_finalLink + "</em>");
    }

    return m_finalText;
}

int Database::getLangStart()
{
    QSqlQuery query;
    query.prepare("SELECT rowid FROM monajat WHERE lang=? LIMIT 1");
    query.addBindValue(m_lang);
    query.exec();

    query.next();
    int strat = query.value(0).toInt();

    return strat;
}

int Database::getLangEnd()
{
    QSqlQuery query;
    query.prepare("SELECT rowid FROM monajat WHERE lang=? ORDER BY rowid DESC LIMIT 1");
    query.addBindValue(m_lang);
    query.exec();

    query.next();
    int end = query.value(0).toInt();

    return end;
}

QStringList Database::getLang()
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
}
