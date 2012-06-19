/* This file is part of the KDE project

   Copyright (C) 2004 Dario Massarin <nekkar@libero.it>
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
#include "transfer-plugins/metalink/abstractmetalink.h"

#include "ui/metalinkcreator/metalinker.h"


class MetalinkHttp : public AbstractMetalink
{
    Q_OBJECT

    public:
    MetalinkHttp(TransferGroup * parent, TransferFactory * factory,
                Scheduler * scheduler, const KUrl & src, const KUrl & dest,
                const KGetMetalink::metalinkHttpParser*  httpParser,const QDomElement * e = 0 );
    ~MetalinkHttp();

    public Q_SLOTS:
    // --- Job virtual functions ---
        void start();
        void stop();

        void deinit(Transfer::DeleteOptions options);

    private Q_SLOTS:
        /**
        * @return true if initialising worked
        * @note false does not mean that an error happened, it could mean, that the user
        * decided to update the metalink
        */
        bool metalinkHttpInit();

    private:
        void startMetalinkHTTP();
        const KGetMetalink::metalinkHttpParser *m_httpparser;
        KGetMetalink::httpLinkHeader m_linkheader;


};

#endif //METALINKHTTP_H
