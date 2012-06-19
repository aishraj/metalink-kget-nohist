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
#include "metalinkhttpsettings.h"

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

void MetalinkHttp::start()
{
    kdDebug(5001) << "metalinkhttp::start";

    if (!m_ready)
    {
        //metalinkHttpInit();
    }
}

void MetalinkHttp::stop()
{

}
