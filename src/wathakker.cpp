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


#include "wathakker.h"

WathakkerForm::WathakkerForm(QWidget *parent) :
        QWidget(parent)
{
    m_wathakkerUi.setupUi(this);
}

int WathakkerForm::thikrLangage()
{
    return m_wathakkerUi.langComboBox->currentIndex();
}

bool WathakkerForm::useDouaa()
{
    return m_wathakkerUi.douaaCheckBox->isChecked();
}

bool WathakkerForm::useHadith()
{
    return m_wathakkerUi.hadithCheckBox->isChecked();
}

bool WathakkerForm::useHikma()
{
    return m_wathakkerUi.hikmaCheckBox->isChecked();
}

void WathakkerForm::setThikrLangage(int index)
{
    m_wathakkerUi.langComboBox->setCurrentIndex(index);
}

void WathakkerForm::changeDouaa(bool enable)
{
    m_wathakkerUi.douaaCheckBox->setChecked(enable);
}

void WathakkerForm::changeHadith(bool enable)
{
    m_wathakkerUi.hadithCheckBox->setChecked(enable);
}

void WathakkerForm::changeHikma(bool enable)
{
    m_wathakkerUi.hikmaCheckBox->setChecked(enable);
}

void WathakkerForm::downloadNewThikr(int lang, bool douaa, bool hadith, bool hikma)
{
    m_useDouaa = douaa;
    m_useHadith = hadith;
    m_useHikma = hikma;
    m_wathakkerLang = lang;

    if(lang == 0) // Arabic
        get(QString("http://www.wathakker.net/outdoor_strip_vertical.html"));
    else if(lang == 1) // English
        get(QString("http://www.en.wathakker.net/outdoor_strip_vertical.html"));
}

int WathakkerForm::get( const KUrl &url )
{
	m_job = 0;
	m_data.clear();
	
	if ( !url.isValid() )
    {
        qDebug() << "Error: Invalid URL" ;
        return false;
    }
    if ( url.scheme() != "http" )
    {
        qDebug() << "Error: URL must start with 'http:'";
        return false;
    }
    if ( url.path().isEmpty() )
    {
        qDebug() << "Error: URL has no path";
        return false;
    }
	
	m_job = KIO::get(url, KIO::NoReload, KIO::HideProgressInfo);
    m_job->addMetaData("Host", url.host());
	m_job->addMetaData( "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
    m_job->addMetaData( "Accept-Language", "" );
    m_job->addMetaData( "Accept-Charset", "UTF-8,*" );
    m_job->addMetaData( "Keep-Alive", "300" );
    m_job->addMetaData( "Connection", "keep-alive" );
	
	connect(m_job, SIGNAL(data(KIO::Job*, QByteArray)), this, SLOT(httpDataReceived(KIO::Job*, QByteArray)));
    connect(m_job, SIGNAL(result(KJob*)), this, SLOT(done()));
	
	return 1;
}

void WathakkerForm::httpDataReceived(KIO::Job *job, QByteArray data)
{
    if (job != m_job) // Not the good job
        return;
	
	if(data.isEmpty())
		return;

    m_data.append(data);
}

void WathakkerForm::done()
{
    m_job = 0;
    
	/*//QTextCodec *codec = QTextCodec::codecForName("windows-1256");
    QTextCodec *codec = QTextCodec::codecForUtfText("uft-8");
    QTextDecoder *decoder = codec->makeDecoder();
    QString out = decoder->toUnicode(m_data);*/
	
    QString out(m_data);
    
	if(out.isEmpty())
	{
		emit newThikr("");
		return;
	}
	
	QString text,searchstring, finalText;
    QString bout, bout2, bout3;
    bout = bout2 = bout3 = out;
    int pos1,pos2;

    searchstring = "class=\"vertical\">",
                   pos1 = bout.indexOf(searchstring,0);
    pos2 = bout.indexOf("</td>",pos1);
    text = bout.remove(pos2,out.size());
    text = text.remove(0,pos1+searchstring.size());

    if (m_useHadith)
    {
        if(m_wathakkerLang == 0)
            finalText.append("حديث :<br />");
        else if(m_wathakkerLang == 1)
            finalText.append("Hadith :<br />");
        
        finalText.append(text);
    }

    pos1 = bout2.indexOf(searchstring,pos2);
    pos2 = bout2.indexOf("</td>",pos1);
    text = bout2.remove(pos2,out.size());
    text = text.remove(0,pos1+searchstring.size());

    if (m_useDouaa)
    {
        if (m_useHadith)
            finalText.append("<br /><br />");

        if(m_wathakkerLang == 0)
            finalText.append("دعاء :<br />");
        else if(m_wathakkerLang == 1)
            finalText.append("Proverb prayer :<br />");
        
        finalText.append(text);
    }

    pos1 = bout3.indexOf(searchstring,pos2);
    pos2 = bout3.indexOf("</td>",pos1);
    text = bout3.remove(pos2,out.size());
    text = text.remove(0,pos1+searchstring.size());

    if (m_useHikma)
    {
        if (m_useDouaa || m_useHadith)
            finalText.append("<br /><br />");

        if(m_wathakkerLang == 0)
            finalText.append("حكمة :<br />");
        else if(m_wathakkerLang == 1)
            finalText.append("Wisdom :<br />");
        
        finalText.append(text);
    }

    emit newThikr(finalText);
}
