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


#ifndef WATHAKKERFORM_H
#define WATHAKKERFORM_H

#include <QWidget>
#include <QTextCodec>

#include <KDebug>
#include <KIO/Job>

#include "ui_wathakkerform.h"

namespace KIO
{
    class Job;
}

class WathakkerForm : public QWidget
{
    Q_OBJECT
public:
    WathakkerForm(QWidget *parent = 0);
    void downloadNewThikr(int lang, bool douaa, bool hadith, bool hikma);
    int get( const KUrl &url );
    int thikrLangage();
    bool useDouaa();
    bool useHadith();
    bool useHikma();
    void setThikrLangage(int index);
    void changeDouaa(bool enable);
    void changeHadith(bool enable);
    void changeHikma(bool enable);

signals:
    void newThikr(QString);

private slots:
    void httpDataReceived(KIO::Job *job, QByteArray data);
    void done();

private:
    KIO::Job *m_job;
    QByteArray m_data;

    int m_wathakkerLang;
    bool m_useDouaa;
    bool m_useHadith;
    bool m_useHikma;

    Ui::WathakkerForm m_wathakkerUi;
};

#endif

