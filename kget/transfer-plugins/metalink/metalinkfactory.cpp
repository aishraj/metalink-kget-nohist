/* This file is part of the KDE project

   Copyright (C) 2004 Dario Massarin <nekkar@libero.it>
   Copyright (C) 2007 Manolo Valdes <nolis71cu@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "metalinkfactory.h"

#include "core/scheduler.h"
#include "core/transfergroup.h"
#include "metalinkhttp.h"
#include "metalinkxml.h"

#include <kdebug.h>

KGET_EXPORT_PLUGIN( metalinkFactory )

metalinkFactory::metalinkFactory(QObject *parent, const QVariantList &args)
  : TransferFactory(parent, args)
{
}

metalinkFactory::~metalinkFactory()
{
}

Transfer * metalinkFactory::createTransfer( const KUrl &srcUrl, const KUrl &destUrl,
                                               TransferGroup * parent,
                                               Scheduler * scheduler,
                                               const QDomElement * e )
{
    kDebug(5001) << "metalinkFactory::createTransfer";

    m_metalinkHttpChecker = new KGetMetalink::metalinkHttpParser(srcUrl);

    if (m_metalinkHttpChecker->isMetalinkHttp())
    {
            kDebug(5001) << "This is metalinkhttp";
            return new MetalinkHttp(parent,this,scheduler,srcUrl,destUrl,m_metalinkHttpChecker,e);
    }
    else if (isSupported(srcUrl))
    {
        kDebug(5001) << "This is not metalinkhttp";
        return new MetalinkXml(parent, this, scheduler, srcUrl, destUrl, e);
    }
    return 0;
}

bool metalinkFactory::isSupported(const KUrl &url) const
{
    return (url.fileName().endsWith(QLatin1String(".metalink")) || url.fileName().endsWith(QLatin1String(".meta4")));
}
