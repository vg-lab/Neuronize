/*
 * Copyright (c) 2018 CCS/UPM - GMRV/URJC.
 *
 * Authors: Juan Pedro Brito Méndez <juanpedro.brito@upm.es>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef BATCHBUILDER_H
#define BATCHBUILDER_H

#include <QWidget>
#include <QFileDialog>

#include "ui_batchbuilder.h"

class  BatchBuilder: public QWidget
{
  Q_OBJECT

    QString mInputDir;
    QString mOutputDir;

  public:
    BatchBuilder ( QWidget *parent = 0 );
    ~BatchBuilder ( );

    QString getInputDir ( ) { return mInputDir; }
    QString getOutputDir ( ) { return mOutputDir; }

    unsigned int getNumberOfSmooths ( ) { return ui.spinBox_Subdivisions->value ( ); }
    QString getBaseName ( ) { return ui.lineEdit_baseName->text ( ); }

  signals:

    void directoriesReadies ( );

  private:

    Ui::BatchBuilder ui;

  public slots:

    void selectInputDirectory ( );
    void selectOutputDirectory ( );

    void generateNeurons ( );
    void cancel ( );
};

#endif // BATCHBUILDER_H
