/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Flacon - audio File Encoder
 * https://github.com/flacon/flacon
 *
 * Copyright: 2012-2013
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

#include "out_wv.h"
#include <QDebug>

static const constexpr char *COMPRESSION_KEY = "Compression";
static const constexpr char *REPLAY_GAIN_KEY = "ReplayGain";

/************************************************

 ************************************************/
OutFormat_Wv::OutFormat_Wv()
{
    mId      = "WV";
    mExt     = "wv";
    mName    = "WavPack";
    mOptions = FormatOption::Lossless | FormatOption::SupportGain;
}

/************************************************

 ************************************************/
QHash<QString, QVariant> OutFormat_Wv::defaultParameters() const
{
    QHash<QString, QVariant> res;
    res.insert(COMPRESSION_KEY, 1);
    res.insert(REPLAY_GAIN_KEY, gainTypeToString(GainType::Disable));
    return res;
}

/************************************************

 ************************************************/
EncoderConfigPage *OutFormat_Wv::configPage(QWidget *parent) const
{
    return new ConfigPage_Wv(parent);
}

/************************************************

 ************************************************/
Conv::Encoder *OutFormat_Wv::createEncoder() const
{
    return new Encoder_Wv();
}

/************************************************
 *
 ************************************************/
Conv::Gain *OutFormat_Wv::createGain(const Profile &profile) const
{
    return new Gain_Wv(profile);
}

/************************************************

 ************************************************/
ConfigPage_Wv::ConfigPage_Wv(QWidget *parent) :
    EncoderConfigPage(parent)
{
    setupUi(this);

    setLosslessToolTip(wvCompressionSlider);
    wvCompressionSpin->setToolTip(wvCompressionSlider->toolTip());
}

/************************************************

 ************************************************/
void ConfigPage_Wv::load(const Profile &profile)
{
    loadWidget(profile, COMPRESSION_KEY, wvCompressionSlider);
}

/************************************************

 ************************************************/
void ConfigPage_Wv::save(Profile *profile)
{
    saveWidget(profile, COMPRESSION_KEY, wvCompressionSlider);
}

/************************************************

 ************************************************/
QStringList Encoder_Wv::programArgs() const
{
    QStringList args;

    args << programPath();

    args << "-q"; // Suppress progress indicator

    // Settings .................................................
    int compression = profile().value(COMPRESSION_KEY).toInt();
    switch (compression) {
        case 0:
            args << "-f";
            break;
        case 1:
            args << "-h";
            break;
        case 2:
            args << "-hh";
            break;
    }

    // Tags .....................................................
    if (!track().artist().isEmpty())
        args << "-w" << QString("Artist=%1").arg(track().artist());

    if (!track().album().isEmpty())
        args << "-w" << QString("Album=%1").arg(track().album());

    if (!track().genre().isEmpty())
        args << "-w" << QString("Genre=%1").arg(track().genre());

    if (!track().date().isEmpty())
        args << "-w" << QString("Year=%1").arg(track().date());

    if (!track().title().isEmpty())
        args << "-w" << QString("Title=%1").arg(track().title());

    if (!track().tag(TagId::AlbumArtist).isEmpty())
        args << "-w" << QString("Album Artist=%1").arg(track().tag(TagId::AlbumArtist));

    if (!track().discId().isEmpty())
        args << "-w" << QString("DiscId=%1").arg(track().discId());

    if (!track().comment().isEmpty())
        args << "-w" << QString("Comment=%1").arg(track().comment());

    args << "-w" << QString("Track=%1/%2").arg(track().trackNum()).arg(track().trackCount());
    args << "-w" << QString("Part=%1").arg(track().discNum());

    if (!coverFile().isEmpty()) {
        args << "--write-binary-tag" << QString("Cover Art (Front)=@%1").arg(coverFile());
    }

    args << "-";
    args << "-o" << outFile();

    return args;
}

/************************************************
 *
 ************************************************/
QStringList Gain_Wv::programArgs(const QStringList &files, const GainType gainType) const
{
    QStringList args;
    args << programPath();
    if (gainType == GainType::Album) {
        args << "-a";
    }
    args << files;

    return args;
}
