/* This file is part of the KDE project

   Copyright (C) 2006 Manolo Valdes <nolis71cu@gmail.com>
   Copyright (C) 2012 Aish Raj Dahal <dahalaishraj@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "dlgmetalinkhttp.h"

#include "metalinkhttpsettings.h"

#include "kget_export.h"

KGET_EXPORT_PLUGIN_CONFIG(DlgSettingsWidget)

DlgSettingsWidget::DlgSettingsWidget(QWidget *parent, const QVariantList &args)
    : KCModule(KGetFactory::componentData(), parent, args)
{
    ui.setupUi(this);

    connect(ui.numSimultanousFiles, SIGNAL(valueChanged(int)), SLOT(changed()));
    connect(ui.kcfg_MirrorsPerFile, SIGNAL(valueChanged(int)), SLOT(changed()));
    connect(ui.kcfg_ConnectionsPerUrl, SIGNAL(valueChanged(int)), SLOT(changed()));
}

DlgSettingsWidget::~DlgSettingsWidget()
{
}

void DlgSettingsWidget::load()
{
    ui.numSimultanousFiles->setValue(MetalinkHttpSettings::simultanousFiles());
    ui.kcfg_MirrorsPerFile->setValue(MetalinkHttpSettings::mirrorsPerFile());
    ui.kcfg_ConnectionsPerUrl->setValue(MetalinkHttpSettings::connectionsPerUrl());
}

void DlgSettingsWidget::save()
{
    MetalinkHttpSettings::setSimultanousFiles(ui.numSimultanousFiles->value());
    MetalinkHttpSettings::setMirrorsPerFile(ui.kcfg_MirrorsPerFile->value());
    MetalinkHttpSettings::setConnectionsPerUrl(ui.kcfg_ConnectionsPerUrl->value());

    MetalinkHttpSettings::self()->writeConfig();
}

#include "dlgmetalinkhttp.moc"
