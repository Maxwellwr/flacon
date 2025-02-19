/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Flacon - audio File Encoder
 * https://github.com/flacon/flacon
 *
 * Copyright: 2017
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

#ifndef DISCPIPLINE_H
#define DISCPIPLINE_H

#include <QObject>
#include <QTemporaryDir>
#include "track.h"
#include "converter.h"
#include "convertertypes.h"
#include "profiles.h"
#include "coverimage.h"

class Project;

namespace Conv {

class WorkerThread;

struct DiscPipelineJob
{
    DiscPipelineJob(ConvTracks tracks,
                    QString    workDir) :
        mTracks(tracks),
        mWorkDir(workDir)
    {
    }

    const ConvTracks &tracks() const { return mTracks; }
    QString           workDir() const { return mWorkDir; }

public:
    ConvTracks mTracks;
    QString    mWorkDir;
};

class DiscPipeline : public QObject
{
    Q_OBJECT
public:
    explicit DiscPipeline(const Profile &profile, Disc *disc, ConvTracks tracks, const QString &workDir, QObject *parent = nullptr) noexcept(false);
    virtual ~DiscPipeline();

    void startWorker(int *splitterCount, int *count);
    void stop();
    bool isRunning() const;
    int  runningThreadCount() const;

signals:
    void readyStart();
    void threadFinished();
    void finished();
    void stopAllThreads();
    void trackProgressChanged(const Conv::ConvTrack &track, TrackState status, Percent percent);

private slots:
    void trackProgress(const Conv::ConvTrack &track, TrackState state, int percent);
    void trackError(const Conv::ConvTrack &track, const QString &message);

    void trackDone(const Conv::ConvTrack &track, const QString &outFileName);

private:
    Profile                  mProfile;
    Disc                    *mDisc = nullptr;
    QString                  mWorkDir;
    QMap<TrackId, ConvTrack> mTracks;
    QTemporaryDir           *mTmpDir = nullptr;
    QString                  mEmbedCoverFile;
    CoverImage               mCoverImage;
    QString                  mEmbeddedCue;

    struct SplitterRequest
    {
        ConvTracks tracks;
        QString    inFile;
        QString    outDir;
    };

    struct Request
    {
        ConvTrack track;
        QString   inputFile;
    };

    QVector<WorkerThread *>      mThreads;
    bool                         mInterrupted = false;
    QList<SplitterRequest>       mSplitterRequests;
    QList<DiscPipeline::Request> mEncoderRequests;
    QList<DiscPipeline::Request> mTrackGainRequests;
    QList<DiscPipeline::Request> mAlbumGainRequests;

    void addSpliterRequest(const InputAudioFile &audio);
    void startSplitter(const ConvTracks &tracks, const QString &inFile, const QString &outDir);

    void addEncoderRequest(const Conv::ConvTrack &track, const QString &inputFile);
    void startEncoder(const ConvTrack &track, const QString &inputFile);

    void addGainRequest(const Conv::ConvTrack &track, const QString &fileName);
    void startGain(const Request &request);
    void startGain(const QList<Request> &requests);

    void interrupt(TrackState state);

    void createDir(const QString &dirName) const;

    void copyCoverImage() const;
    void createEmbedImage();

    void writeOutCueFile();
    void loadEmbeddedCue();
};

} // Namespace

#endif // DISCPIPLINE_H
