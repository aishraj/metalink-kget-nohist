/* This file is part of the KDE project

   Copyright (C) 2004 Dario Massarin <nekkar@libero.it>
   Copyright (C) 2007 Manolo Valdes <nolis71cu@gmail.com>
   Copyright (C) 2009 Matthias Fuchs <mat69@gmx.net>
   Copyright (C) 2012 Aish Raj Dahal <dahalaishraj@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "metalinkhttp.h"
#include "metalinksettings.h"

#include "core/kget.h"
#include "core/transfergroup.h"
#include "core/download.h"
#include "core/transferdatasource.h"
#include "core/filemodel.h"
#include "core/urlchecker.h"
#include "core/verifier.h"
#include "core/signature.h"
#ifdef HAVE_NEPOMUK
#include "core/nepomukhandler.h"
#include <Nepomuk/Variant>
#endif //HAVE_NEPOMUK

#include <KIconLoader>
#include <KIO/DeleteJob>
#include <KIO/NetAccess>
#include <KIO/RenameDialog>
#include <KLocale>
#include <KMessageBox>
#include <KDebug>
#include <KDialog>
#include <KStandardDirs>

#include <QtCore/QFile>
#include <QtXml/QDomElement>

MetalinkHttp::MetalinkHttp(TransferGroup * parent, TransferFactory * factory,
                         Scheduler * scheduler, const KUrl & source, const KUrl & dest,
                         KGetMetalink::metalinkHttpParser *httpParser,
                         const QDomElement * e)
    : AbstractMetalink(parent, factory,scheduler,source, dest, e) ,
      m_httpparser(httpParser)
{

}

MetalinkHttp::~MetalinkHttp()
{

}

void MetalinkHttp::startMetalink()
{
    if (m_ready)
    {
        foreach (DataSourceFactory *factory, m_dataSourceFactory)
        {
            //specified number of files is downloaded simultanously
            if (m_currentFiles < MetalinkSettings::simultanousFiles())
            {
                const int status = factory->status();
                //only start factories that should be downloaded
                if (factory->doDownload() &&
                    (status != Job::Finished) &&
                    (status != Job::FinishedKeepAlive) &&
                    (status != Job::Running))
                {
                    ++m_currentFiles;
                    factory->start();
                }
            }
            else
            {
                break;
            }
        }
    }
}

void MetalinkHttp::start()
{
    kDebug() << "metalinkhttp::start";

    if (!m_ready) {
        setLinks();
        setDigests();
        if (metalinkHttpInit()) {
            startMetalink();
        }
    }
    else {
        startMetalink();
    }
}

void MetalinkHttp::stop()
{
    kDebug(5001) << "metalink::Stop";
    if (m_ready && status() != Stopped)
    {
        m_currentFiles = 0;
        foreach (DataSourceFactory *factory, m_dataSourceFactory)
        {
            factory->stop();
        }
    }
}

bool MetalinkHttp::metalinkHttpInit()
{
    kDebug() << "metalinkHttp::metalinkHttpInit";

    const KUrl tempDest=  KUrl(m_dest.directory());
    KUrl dest;
    dest = tempDest;
    dest.addPath(m_dest.fileName());

    //sort the urls according to their priority (highest first)
    qStableSort(m_linkheaderList);

    DataSourceFactory *dataFactory = new DataSourceFactory(this,dest); //TODO: check the filesize and add a param here
    dataFactory->setMaxMirrorsUsed(MetalinkSettings::mirrorsPerFile());

    connect(dataFactory, SIGNAL(capabilitiesChanged()), this, SLOT(slotUpdateCapabilities()));
    connect(dataFactory, SIGNAL(dataSourceFactoryChange(Transfer::ChangesFlags)), this, SLOT(slotDataSourceFactoryChange(Transfer::ChangesFlags)));
    connect(dataFactory->verifier(), SIGNAL(verified(bool)), this, SLOT(slotVerified(bool)));
    connect(dataFactory->signature(), SIGNAL(verified(int)), this, SLOT(slotSignatureVerified()));
    connect(dataFactory, SIGNAL(log(QString,Transfer::LogLevel)), this, SLOT(setLog(QString,Transfer::LogLevel)));

    //add the Mirrors Sources

    for(int i = 0; i < m_linkheaderList.size(); ++i)
    {
        const KUrl url = m_linkheaderList[i].url;
        if (url.isValid())
        {
            dataFactory->addMirror(url, MetalinkSettings::connectionsPerUrl());
        }
    }

    //no datasource has been created, so remove the datasource factory
    if (dataFactory->mirrors().isEmpty())
    {
        kDebug() << "data source factory being deleted" ;
        delete dataFactory;
    }
    else
    {
        //BUG: Cannot add checksum
        //TODO: Remove the bug
        /* Checking if the values are in uppper or lower case */
        QHashIterator<QString, QString> itr(m_DigestList);
        while(itr.hasNext()) {
            itr.next();
            kDebug() << itr.key() << ":" << itr.value() ;
        }
       // dataFactory->verifier()->addChecksums(m_DigestList);
        //TODO Extend Support to signatures also

        m_dataSourceFactory[dataFactory->dest()] = dataFactory;
    }

    if ((m_dataSourceFactory).size()) {
        m_dest = dest;
    }

    if (!m_dataSourceFactory.size()) {
        //TODO make this via log in the future + do not display the KMessageBox
        kWarning(5001) << "Download of" << m_source << "failed, no working URLs were found.";
        KMessageBox::error(0, i18n("Download failed, no working URLs were found."), i18n("Error"));
        setStatus(Job::Aborted);
        setTransferChange(Tc_Status, true);
        return false;
    }

    m_ready = !m_dataSourceFactory.isEmpty();
    slotUpdateCapabilities();

    return true;

}

void MetalinkHttp::setLinks()
{
    kDebug() << "metalinkHttp::setLinks";
    QMultiMap<QString, QString>* headerInf = m_httpparser->getHeaderInfo();
    QList<QString> linkVals = headerInf->values("link"); //TODO need to check the describedby field also.
    foreach ( QString link, linkVals) {
        KGetMetalink::httpLinkHeader linkheader;
        linkheader.headerBuilder(link);
        if (linkheader.m_reltype == "duplicate") {
            m_linkheaderList.append(linkheader);
        }
    }
}

void MetalinkHttp::deinit(Transfer::DeleteOptions options)
{
    foreach (DataSourceFactory *factory, m_dataSourceFactory) {
        if (options & Transfer::DeleteFiles) {
            factory->deinit();
        }
    }//TODO: Ask the user if he/she wants to delete the *.part-file? To discuss (boom1992)
}

void MetalinkHttp::setDigests()
{
    kDebug() << "metalinkHttp::setDigests " ;
    QMultiMap<QString, QString>* digestInfo = m_httpparser->getHeaderInfo();
    QList<QString> digestList = digestInfo->values("digest");
    foreach( QString digest, digestList) {
        int eqDelimiter = digest.indexOf('=');
        QString digestType = digest.left(eqDelimiter).trimmed();
        QString digestValue = digest.mid(eqDelimiter + 1).trimmed();
        m_DigestList.insertMulti(digestType,digestValue);
    }
}

#include "metalinkhttp.moc"
