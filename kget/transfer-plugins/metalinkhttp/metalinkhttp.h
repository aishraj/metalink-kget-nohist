/* This file is part of the KDE project

   Copyright (C) 2012 Aish Raj Dahal <dahalaishraj@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/


#ifndef METALINKHTTP_H
#define METALINKHTTP_H

#include <KIO/Job>

#include "core/datasourcefactory.h"
#include "core/transfer.h"

#include "ui/metalinkcreator/metalinker.h"
#include "transfer-plugins/metalink/metalink.h"

class Metalink;

class MetalinkHttp : public Metalink
{
    public:
        MetalinkHttp(TransferGroup * parent, TransferFactory * factory,
                    Scheduler * scheduler, const KUrl & src, const KUrl & dest,
                    const KGetMetalink::metalinkHttpParser* headerParser,
                    const QDomElement * e = 0);

        ~MetalinkHttp();

        void start();
        void stop();

    private:
        const KGetMetalink::metalinkHttpParser* m_httpparser;
        KGetMetalink::httpLinkHeader  m_linkHeaderInfo;
        };

#endif
