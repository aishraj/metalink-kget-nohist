/* This file is part of the KDE project

   Copyright (C) 2006 Manolo Valdes <nolis71cu@gmail.com>
   Copyright (C) 2012 Aish Raj Dahal <dahalaishraj@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef DLGMETALINKHTTP_H
#define DLGMETALINKHTTP_H

#include "ui_dlgmetalinkhttp.h"

#include <KCModule>

class DlgSettingsWidget : public KCModule
{
    Q_OBJECT
public:
    explicit DlgSettingsWidget(QWidget *parent = 0, const QVariantList &args = QVariantList());
    ~DlgSettingsWidget();

public slots:
    virtual void save();
    virtual void load();

private:
    Ui::DlgMetalinkHttp ui;
};

#endif // DLGMETALINKHTTP_H
