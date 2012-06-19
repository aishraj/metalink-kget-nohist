/* This file is part of the KDE project

   Copyright (C) 2006 Manolo Valdes <nolis71cu@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "dlgmetalink.h"

#include "metalinkxmlsettings.h"

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
    ui.numSimultanousFiles->setValue(MetalinkXmlSettings::simultanousFiles());
    ui.kcfg_MirrorsPerFile->setValue(MetalinkXmlSettings::mirrorsPerFile());
    ui.kcfg_ConnectionsPerUrl->setValue(MetalinkXmlSettings::connectionsPerUrl());
}

void DlgSettingsWidget::save()
{
    MetalinkXmlSettings::setSimultanousFiles(ui.numSimultanousFiles->value());
    MetalinkXmlSettings::setMirrorsPerFile(ui.kcfg_MirrorsPerFile->value());
    MetalinkXmlSettings::setConnectionsPerUrl(ui.kcfg_ConnectionsPerUrl->value());

    MetalinkXmlSettings::self()->writeConfig();
}

#include "dlgmetalink.moc"
