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
#include "transfer-plugins/metalink/metalink.h"

#include "ui/metalinkcreator/metalinker.h"


class MetalinkHttp : public Metalink
{
    bool m_dummy; //Just to check compilation. Ignore me.
};

#endif //METALINKHTTP_H
