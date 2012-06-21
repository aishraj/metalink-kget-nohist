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
                         const KGetMetalink::metalinkHttpParser *httpParser,
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
    DataSourceFactory* factory = m_dataSourceFactory;
    if (m_currentFiles < MetalinkSettings::simultanousFiles())
    {
        const int status = factory->status();
        //only start factories that should be downloaded
        if (factory->doDownload() &&
            (status != Job::Finished) &&
            (status != Job::FinishedKeepAlive) &&
            (status != Job::Running))
        {
            factory->start();
        }
    }

}

void MetalinkHttp::start()
{
    kdDebug(5001) << "metalinkhttp::start";

    if (!m_ready)
    {
        setLinks();
        setDigests();
        metalinkHttpInit();
    }
}

void MetalinkHttp::stop()
{
    DataSourceFactory* factory;
    kDebug(5001) << "metalinkhtt::Stop";
    if (m_ready && status() != Stopped)
    {
        factory->stop();
    }
}

bool MetalinkHttp::metalinkHttpInit()
{
    qSort(m_linkheaderList.begin(), m_linkheaderList.end(), qGreater<KGetMetalink::httpLinkHeader());
    KUrl dest = KUrl(m_dest.directory());
    DataSourceFactory *dataFactory = new DataSourceFactory(this,dest);
    dataFactory->setMaxMirrorsUsed(MetalinkSettings::mirrorsPerFile());

    connect(dataFactory, SIGNAL(capabilitiesChanged()), this, SLOT(slotUpdateCapabilities()));
    connect(dataFactory, SIGNAL(dataSourceFactoryChange(Transfer::ChangesFlags)), this, SLOT(slotDataSourceFactoryChange(Transfer::ChangesFlags)));
    connect(dataFactory->verifier(), SIGNAL(verified(bool)), this, SLOT(slotVerified(bool)));
    connect(dataFactory->signature(), SIGNAL(verified(int)), this, SLOT(slotSignatureVerified()));
    connect(dataFactory, SIGNAL(log(QString,Transfer::LogLevel)), this, SLOT(setLog(QString,Transfer::LogLevel)));

    for(int i = 0; i < m_linkheaderList.size(); ++i)
    {
        const KUrl url = m_linkheaderList[i].url;
        if (url.isValid())
        {
            dataFactory->addMirror(url, MetalinkSettings::connectionsPerUrl());
        }
    }

    if (dataFactory->mirrors().isEmpty())
    {
        delete dataFactory;
    }
    else
    {
        dataFactory->verifier()->addChecksums(m_DigestList);
        //TODO Extend Support to signatures also
    }

    m_dataSourceFactory = dataFactory;

    if (!m_dataSourceFactory.size()) {
        //TODO make this via log in the future + do not display the KMessageBox
        kWarning(5001) << "Download of" << m_source << "failed, no working URLs were found.";
        KMessageBox::error(0, i18n("Download failed, no working URLs were found."), i18n("Error"));
        setStatus(Job::Aborted);
        setTransferChange(Tc_Status, true);
        return false;
    }

    if (m_dataSourceFactory) {
        m_ready = true;
    }
    else {
        kDebug() << "Setting ready to false" ; //Should not happen. Check this
        m_ready = false;
    }

    return true;

}

void MetalinkHttp::setLinks()
{
    QMultiMap<QString, QString>* headerInf = m_httpparser->getHeaderInfo();
    QList<QString> linkVals = headerInf->values("link");
    foreach ( QString link, linkVals) {
        KGetMetalink::httpLinkHeader linkheader(link);
        m_linkheaderList.insert(linkheader);
    }
}

void MetalinkHttp::deinit(Transfer::DeleteOptions options)
{
    DataSourceFactory* factory() = m_dataSourceFactory;
    if (options & Transfer::DeleteFiles) {
        factory->deinit();
    }
}

}

void MetalinkHttp::setDigests()
{
    QMultiMap<QString, QString>* digestInfo = m_httpparser->getHeaderInfo();
    QList<QString> digestList = digestInfo->values("digest");
    foreach( QString digest, digestList) {
        int eqDelimiter = digest.indexOf('=');
        QString digestType = digest.left(eqDelimiter).trimmed();
        QString digestValue = digest.mid(eqDelimiter + 1).trimmed();
        m_DigestList.insertMulti(digestType,digestValue);
    }
}
