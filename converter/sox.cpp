/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Flacon - audio File Encoder
 * https://github.com/flacon/flacon
 *
 * Copyright: 2018
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "sox.h"
#include "settings.h"

#include <QDir>

using namespace Conv;

/************************************************
 *
 ************************************************/
Sox::Sox()
{
}

/************************************************
 *
 ************************************************/
QStringList Sox::resamplerArgs(int bitsPerSample, int sampleRate, const QString &outFile)
{
    QString     prog = Settings::i()->programName(programName());
    QStringList args;
    args << QDir::toNativeSeparators(prog);

    args << "--type"
         << "wav";

    args << "-"; // Read from STDIN
    if (bitsPerSample) {
        args << "-b" << QString("%1").arg(bitsPerSample);
    }

    args << "--type"
         << "wav";
    args << outFile;

    if (sampleRate) {
        args << "rate";
        args << "-v"; // very high quality
        args << QString("%1").arg(sampleRate);
    }

    return args;
}

/************************************************
 *
 ************************************************/
QStringList Sox::deemphasisArgs(const QString &outFile)
{
    QString     prog = Settings::i()->programName(programName());
    QStringList args;
    args << QDir::toNativeSeparators(prog);

    // clang-format off
    args << "--type" << "wav" << "-"; // Read from STDIN
    args << "--type" << "wav" << outFile;
    args << "deemph";
    // clang-format on

    return args;
}
