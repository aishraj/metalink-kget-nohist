/* This file is part of the KDE project

   Copyright (C) 2004 Dario Massarin <nekkar@libero.it>
   Copyright (C) 2007 Manolo Valdes <nolis71cu@gmail.com>
   Copyright (C) 2009 Matthias Fuchs <mat69@gmx.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "metalinkxml.h"
#include "fileselectiondlg.h"
#include "metalinkxmlsettings.h"

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

MetalinkXml::MetalinkXml(TransferGroup * parent, TransferFactory * factory,
                         Scheduler * scheduler, const KUrl & source, const KUrl & dest,
                         const QDomElement * e)
    : AbstractMetalink(parent, factory, scheduler, source, dest, e)

{
}

MetalinkXml::~MetalinkXml()
{
}

void MetalinkXml::start()
{
    kDebug(5001) << "metalink::start";

    if (!m_ready)
    {
        if (m_localMetalinkLocation.isValid() && metalinkInit()) {
            startMetalink();
        } else {
            downloadMetalink();
        }
    }
    else
    {
        startMetalink();
    }
}

void MetalinkXml::downloadMetalink()
{
    m_metalinkJustDownloaded = true;

    setStatus(Job::Stopped, i18n("Downloading Metalink File...."), SmallIcon("document-save"));
    setTransferChange(Tc_Status, true);
    Download *download = new Download(m_source, QString(KStandardDirs::locateLocal("appdata", "metalinks/") + m_source.fileName()));
    connect(download, SIGNAL(finishedSuccessfully(KUrl,QByteArray)), SLOT(metalinkInit(KUrl,QByteArray)));
}

bool MetalinkXml::metalinkInit(const KUrl &src, const QByteArray &data)
{
    kDebug(5001);

    if (!src.isEmpty()) {
        m_localMetalinkLocation = src;
    }

    //use the downloaded metalink-file data directly if possible
    if (!data.isEmpty()) {
        KGetMetalink::HandleMetalink::load(data, &m_metalink);
    }

    //try to parse the locally stored metalink-file
    if (!m_metalink.isValid() && m_localMetalinkLocation.isValid()) {
        KGetMetalink::HandleMetalink::load(m_localMetalinkLocation.toLocalFile(), &m_metalink);
    }

    if (!m_metalink.isValid()) {
        kError(5001) << "Unknown error when trying to load the .metalink-file. Metalink is not valid.";
        setStatus(Job::Aborted);
        setTransferChange(Tc_Status, true);
        return false;
    }

    //offers a dialog to download the newest version of a dynamic metalink
     if ((m_source.isLocalFile() || !m_metalinkJustDownloaded) &&
         m_metalink.dynamic && (UrlChecker::checkSource(m_metalink.origin) == UrlChecker::NoError)) {
        if (KMessageBox::questionYesNo(0, i18n("A newer version of this Metalink might exist, do you want to download it?"),
                                       i18n("Redownload Metalink")) == KMessageBox::Yes) {
            m_localMetalinkLocation.clear();
            m_source = m_metalink.origin;
            downloadMetalink();
            return false;
        }
    }

    QList<KGetMetalink::File>::const_iterator it;
    QList<KGetMetalink::File>::const_iterator itEnd = m_metalink.files.files.constEnd();
    m_totalSize = 0;
    KIO::fileoffset_t segSize = 500 * 1024;//TODO use config here!
    const KUrl tempDest = KUrl(m_dest.directory());
    KUrl dest;
    for (it = m_metalink.files.files.constBegin(); it != itEnd ; ++it)
    {
        dest = tempDest;
        dest.addPath((*it).name);

        QList<KGetMetalink::Url> urlList = (*it).resources.urls;
        //sort the urls according to their priority (highest first)
        qSort(urlList.begin(), urlList.end(), qGreater<KGetMetalink::Url>());

        KIO::filesize_t fileSize = (*it).size;
        m_totalSize += fileSize;

        //create a DataSourceFactory for each separate file
        DataSourceFactory *dataFactory = new DataSourceFactory(this, dest, fileSize, segSize);
        dataFactory->setMaxMirrorsUsed(MetalinkXmlSettings::mirrorsPerFile());

#ifdef HAVE_NEPOMUK
        nepomukHandler()->setProperties((*it).properties(), QList<KUrl>() << dest);
#endif //HAVE_NEPOMUK

//TODO compare available file size (<size>) with the sizes of the server while downloading?

        connect(dataFactory, SIGNAL(capabilitiesChanged()), this, SLOT(slotUpdateCapabilities()));
        connect(dataFactory, SIGNAL(dataSourceFactoryChange(Transfer::ChangesFlags)), this, SLOT(slotDataSourceFactoryChange(Transfer::ChangesFlags)));
        connect(dataFactory->verifier(), SIGNAL(verified(bool)), this, SLOT(slotVerified(bool)));
        connect(dataFactory->signature(), SIGNAL(verified(int)), this, SLOT(slotSignatureVerified()));
        connect(dataFactory, SIGNAL(log(QString,Transfer::LogLevel)), this, SLOT(setLog(QString,Transfer::LogLevel)));

        //add the DataSources
        for (int i = 0; i < urlList.size(); ++i)
        {
            const KUrl url = urlList[i].url;
            if (url.isValid())
            {
                dataFactory->addMirror(url, MetalinkXmlSettings::connectionsPerUrl());
            }
        }
        //no datasource has been created, so remove the datasource factory
        if (dataFactory->mirrors().isEmpty())
        {
            delete dataFactory;
        }
        else
        {
            dataFactory->verifier()->addChecksums((*it).verification.hashes);
            foreach (const KGetMetalink::Pieces &pieces, (*it).verification.pieces) {
                dataFactory->verifier()->addPartialChecksums(pieces.type, pieces.length, pieces.hashes);
            }

            const QHash <QString, QString> signatures = (*it).verification.signatures;
            QHash<QString, QString>::const_iterator it;
            QHash<QString, QString>::const_iterator itEnd = signatures.constEnd();
            for (it = signatures.constBegin(); it != itEnd; ++it) {
                if (it.key().toLower() == "pgp") {
                    dataFactory->signature()->setAsciiDetatchedSignature(*it);
                }
            }

            m_dataSourceFactory[dataFactory->dest()] = dataFactory;
        }
    }

    if ((m_metalink.files.files.size() == 1) && m_dataSourceFactory.size())
    {
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

    //the metalink-file has just been downloaded, so ask the user to choose the files that
    // should be downloaded
    if (m_metalinkJustDownloaded) {
        KDialog *dialog = new FileSelectionDlg(fileModel());
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, SIGNAL(finished(int)), this, SLOT(fileDlgFinished(int)));

        dialog->show();
    }

    return true;
}

void MetalinkXml::untickAllFiles()
{
    for (int row = 0; row < fileModel()->rowCount(); ++row) {
        QModelIndex index = fileModel()->index(row, FileItem::File);
        if (index.isValid()) {
            fileModel()->setData(index, Qt::Unchecked, Qt::CheckStateRole);
        }
    }
}

void MetalinkXml::fileDlgFinished(int result)
{
    //the dialog was not accepted untick every file, this ensures that the user does not
    //press start by accident without first selecting the desired files
    if (result != QDialog::Accepted) {
        untickAllFiles();
    }

    filesSelected();

    //no files selected to download or dialog rejected, stop the download
    if (!m_numFilesSelected  || (result != QDialog::Accepted)) {
        setStatus(Job::Stopped);
        setTransferChange(Tc_Status, true);
        return;
    }

    startMetalink();
}

void MetalinkXml::startMetalink()
{
    if (m_ready)
    {
        foreach (DataSourceFactory *factory, m_dataSourceFactory)
        {
            //specified number of files is downloaded simultanously
            if (m_currentFiles < MetalinkXmlSettings::simultanousFiles())
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

void MetalinkXml::deinit(Transfer::DeleteOptions options)
{
    foreach (DataSourceFactory *factory, m_dataSourceFactory) {
        if (options & Transfer::DeleteFiles) {
            factory->deinit();
        }
    }//TODO: Ask the user if he/she wants to delete the *.part-file? To discuss (boom1992)

    //FIXME does that mean, that the metalink file is always removed, even if
    //downloaded by the user?
    if ((options & Transfer::DeleteTemporaryFiles) && m_localMetalinkLocation.isLocalFile())
    {
        KIO::Job *del = KIO::del(m_localMetalinkLocation, KIO::HideProgressInfo);
        KIO::NetAccess::synchronousRun(del, 0);
    }

#ifdef HAVE_NEPOMUK
    nepomukHandler()->deinit();
#endif //HAVE_NEPOMUK
}

void MetalinkXml::stop()
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

void MetalinkXml::load(const QDomElement *element)
{
    Transfer::load(element);

    if (!element)
    {
        return;
    }

    const QDomElement e = *element;
    m_localMetalinkLocation = KUrl(e.attribute("LocalMetalinkLocation"));
    QDomNodeList factories = e.firstChildElement("factories").elementsByTagName("factory");

    //no stored information found, stop right here
    if (!factories.count())
    {
        return;
    }

    while (factories.count())
    {
        QDomDocument doc;
        QDomElement factory = doc.createElement("factories");
        factory.appendChild(factories.item(0).toElement());
        doc.appendChild(factory);

        DataSourceFactory *file = new DataSourceFactory(this);
        file->load(&factory);
        connect(file, SIGNAL(capabilitiesChanged()), this, SLOT(slotUpdateCapabilities()));
        connect(file, SIGNAL(dataSourceFactoryChange(Transfer::ChangesFlags)), this, SLOT(slotDataSourceFactoryChange(Transfer::ChangesFlags)));
        m_dataSourceFactory[file->dest()] = file;
        connect(file->verifier(), SIGNAL(verified(bool)), this, SLOT(slotVerified(bool)));
        connect(file->signature(), SIGNAL(verified(int)), this, SLOT(slotSignatureVerified()));
        connect(file, SIGNAL(log(QString,Transfer::LogLevel)), this, SLOT(setLog(QString,Transfer::LogLevel)));

        //start the DataSourceFactories that were Started when KGet was closed
        if (file->status() == Job::Running) {
            if (m_currentFiles < MetalinkXmlSettings::simultanousFiles()) {
                ++m_currentFiles;
                file->start();
            } else {
                //enough simultanous files already, so increase the number and set file to stop --> that will decrease the number again
                file->stop();
            }
        }
    }
    m_ready = !m_dataSourceFactory.isEmpty();
    slotUpdateCapabilities();
}

void MetalinkXml::save(const QDomElement &element)
{
    Transfer::save(element);

    QDomElement e = element;
    e.setAttribute("LocalMetalinkLocation", m_localMetalinkLocation.url());

    foreach (DataSourceFactory *factory, m_dataSourceFactory)
    {
        factory->save(e);
    }
}

void MetalinkXml::filesSelected()
{
    bool overwriteAll = false;
    bool autoSkip = false;
    bool cancel = false;
    QModelIndexList files = fileModel()->fileIndexes(FileItem::File);
    m_numFilesSelected = 0;

    //sets the CheckState of the fileModel to the according DataSourceFactories
    //and asks the user if there are existing files already
    foreach (const QModelIndex &index, files)
    {
        const KUrl dest = fileModel()->getUrl(index);
        bool doDownload = index.data(Qt::CheckStateRole).toBool();
        if (m_dataSourceFactory.contains(dest))
        {
            DataSourceFactory *factory = m_dataSourceFactory[dest];
            //ignore finished transfers
            if ((factory->status() == Job::Finished) || (factory->status() == Job::FinishedKeepAlive)) {
                continue;
            }

            //check if the file at dest exists already and ask the user what to do in this case, ignore already running transfers
            if (doDownload && (factory->status() != Job::Running) && QFile::exists(dest.toLocalFile())) {
                //usere has chosen to skip all files that exist already before
                if (autoSkip) {
                    fileModel()->setData(index, Qt::Unchecked, Qt::CheckStateRole);
                    doDownload = false;
                //ask the user, unless he has choosen overwriteAll before
                } else if (!overwriteAll) {
                    KIO::RenameDialog dlg(0, i18n("File already exists"), index.data().toString(), dest, KIO::RenameDialog_Mode(KIO::M_MULTI | KIO::M_OVERWRITE | KIO::M_SKIP));
                    const int result = dlg.exec();

                    if (result == KIO::R_RENAME) {
                        //no reason to use FileModel::rename() since the file does not exist yet, so simply skip it
                        //avoids having to deal with signals
                        const KUrl newDest = dlg.newDestUrl();
                        factory->setDoDownload(doDownload);
                        factory->setNewDestination(newDest);
                        fileModel()->setData(index, newDest.fileName(), FileItem::File);
                        ++m_numFilesSelected;

                        m_dataSourceFactory.remove(dest);
                        m_dataSourceFactory[newDest] = factory;
                        continue;
                    } else if (result == KIO::R_SKIP) {
                        fileModel()->setData(index, Qt::Unchecked, Qt::CheckStateRole);
                        doDownload = false;
                    } else if (result == KIO::R_CANCEL) {
                        cancel = true;
                        break;
                    } else if (result == KIO::R_AUTO_SKIP) {
                        autoSkip = true;
                        fileModel()->setData(index, Qt::Unchecked, Qt::CheckStateRole);
                        doDownload = false;
                    } else if (result == KIO::R_OVERWRITE_ALL) {
                        overwriteAll = true;
                    }
                }
            }

            factory->setDoDownload(doDownload);
            if (doDownload && (factory->status() != Finished) && (factory->status() != FinishedKeepAlive)) {
                ++m_numFilesSelected;
            }
        }
    }

    //the user decided to cancel, so untick all files
    if (cancel) {
        m_numFilesSelected = 0;
        untickAllFiles();
        foreach (DataSourceFactory *factory, m_dataSourceFactory) {
            factory->setDoDownload(false);
        }
    }

    Transfer::ChangesFlags change = (Tc_TotalSize | Tc_DownloadSpeed);
    //some files have been selected that are not finished yet, set them to stop if the transfer is not running (checked in slotStatus)
    if (m_numFilesSelected) {
        change |= Tc_Status;
    }
    slotDataSourceFactoryChange(change);
}
#include "metalinkxml.moc"
