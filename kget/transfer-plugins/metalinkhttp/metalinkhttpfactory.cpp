/* This file is part of the KDE project

   Copyright (C) 2004 Dario Massarin <nekkar@libero.it>
   Copyright (C) 2007 Manolo Valdes <nolis71cu@gmail.com>
   Copyright (C) 2012 Aish Raj Dahal <ardahal.public@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "metalinkhttpfactory.h"

#include "core/scheduler.h"
#include "core/transfergroup.h"
#include "metalinkhttp.h"

#include <kdebug.h>

KGET_EXPORT_PLUGIN( metalinkHttpFactory )

metalinkHttpFactory::metalinkHttpFactory(QObject *parent, const QVariantList &args)
  : TransferFactory(parent, args)
{
}

metalinkHttpFactory::~metalinkHttpFactory()
{
}

Transfer * metalinkHttpFactory::createTransfer( const KUrl &srcUrl, const KUrl &destUrl,
                                               TransferGroup * parent,
                                               Scheduler * scheduler,
                                               const QDomElement * e )
{
    if (isSupported(srcUrl)) {
        kDebug() << " this is metalink http" ;
        qDebug() << " this is metalink http" ;
    }
    else {
        kDebug() << " this is not metalink http" ;
        qDebug() << " this is not metalink http" ;
    }
    return 0;
}

bool metalinkHttpFactory::isSupported(const KUrl &url)
{
    m_metalinkHttpChecker = new KGetMetalink::metalinkHttpParser(url);
    if (m_metalinkHttpChecker->isMetalinkHttp()) {
        kDebug() << "This is metalinkhttp";
        return true;
    }
    return false;
}
