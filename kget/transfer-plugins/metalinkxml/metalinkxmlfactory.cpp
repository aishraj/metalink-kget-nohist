/* This file is part of the KDE project

   Copyright (C) 2004 Dario Massarin <nekkar@libero.it>
   Copyright (C) 2007 Manolo Valdes <nolis71cu@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "metalinkxmlfactory.h"

#include "core/scheduler.h"
#include "core/transfergroup.h"
#include "metalinkxml.h"

#include <kdebug.h>

KGET_EXPORT_PLUGIN( metalinkXmlFactory )

metalinkXmlFactory::metalinkXmlFactory(QObject *parent, const QVariantList &args)
  : TransferFactory(parent, args)
{
}

metalinkXmlFactory::~metalinkXmlFactory()
{
}

Transfer * metalinkXmlFactory::createTransfer( const KUrl &srcUrl, const KUrl &destUrl,
                                               TransferGroup * parent,
                                               Scheduler * scheduler,
                                               const QDomElement * e )
{
    kDebug(5001) << "metalinkFactory::createTransfer";

    if (isSupported(srcUrl))
    {
        return new MetalinkXml(parent, this, scheduler, srcUrl, destUrl, e);
    }
    return 0;
}

bool metalinkXmlFactory::isSupported(const KUrl &url) const
{
    return (url.fileName().endsWith(QLatin1String(".metalink")) || url.fileName().endsWith(QLatin1String(".meta4")));
}
