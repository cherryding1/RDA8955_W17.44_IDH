//////////////////////////////////////////////////////////////////////////////
///
/// SoundTouch - main class for tempo/pitch/rate adjusting routines. 
///
/// Notes:
/// - Initialize the SoundTouch object instance by setting up the sound stream 
///   parameters with functions 'setSampleRate' and 'setChannels', then set 
///   desired tempo/pitch/rate settings with the corresponding functions.
///
/// - The SoundTouch class behaves like a first-in-first-out pipeline: The 
///   samples that are to be processed are fed into one of the pipe by calling
///   function 'putSamples', while the ready processed samples can be read 
///   from the other end of the pipeline with function 'receiveSamples'.
/// 
/// - The SoundTouch processing classes require certain sized 'batches' of 
///   samples in order to process the sound. For this reason the classes buffer 
///   incoming samples until there are enough of samples available for 
///   processing, then they carry out the processing step and consequently
///   make the processed samples available for outputting.
/// 
/// - For the above reason, the processing routines introduce a certain 
///   'latency' between the input and output, so that the samples input to
///   SoundTouch may not be immediately available in the output, and neither 
///   the amount of outputtable samples may not immediately be in direct 
///   relationship with the amount of previously input samples.
///
/// - The tempo/pitch/rate control parameters can be altered during processing.
///   Please notice though that they aren't currently protected by semaphores,
///   so in multi-thread application external semaphore protection may be
///   required.
///
/// - This class utilizes classes 'TDStretch' for tempo change (without modifying
///   pitch) and 'RateTransposer' for changing the playback rate (that is, both 
///   tempo and pitch in the same ratio) of the sound. The third available control 
///   'pitch' (change pitch but maintain tempo) is produced by a combination of
///   combining the two other controls.
///
/// Author        : Copyright (c) Olli Parviainen
/// Author e-mail : oparviai 'at' iki.fi
/// SoundTouch WWW: http://www.surina.net/soundtouch
///
////////////////////////////////////////////////////////////////////////////////
//
// Last changed  : $Date: 2015-09-20 10:38:32 +0300 (Sun, 20 Sep 2015) $
// File revision : $Revision: 4 $
//
// $Id: SoundTouch.h 230 2015-09-20 07:38:32Z oparviai $
//
////////////////////////////////////////////////////////////////////////////////
//
// License :
//
//  SoundTouch audio processing library
//  Copyright (c) Olli Parviainen
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SoundTouch_H
#define SoundTouch_H

#include "STTypes.h"
#include "morphvoice.h"

#include "hal_debug.h"
/// @todo include the profile codes here
#include "sxs_io.h"
#include "vois_profile_codes.h"


#define BUFF_SIZE           160




    void  SoundTouch_SettingChange(uint nChange);
	/// Calculates effective rate & tempo valuescfrom 'virtualRate', 'virtualTempo' and 
    /// 'virtualPitch' parameters.
    void SoundTouch_calcEffectiveRateAndTempo();
	void SoundTouch_setParameters(int aSampleRate, int aSequenceMS, 
                              int aSeekWindowMS, int aOverlapMS, bool bquickseek);


    /// Sets new rate control value as a difference in percents compared
    /// to the original rate (-50 .. +100 %)
    void SoundTouch_setRateChange(double newRate);

    /// Sets new tempo control value as a difference in percents compared
    /// to the original tempo (-50 .. +100 %)
    void SoundTouch_setTempoChange(double newTempo);

    /// Sets new pitch control value. Original pitch = 1.0, smaller values
    /// represent lower pitches, larger values higher pitch.
   /// Sets pitch change in semi-tones compared to the original pitch
    /// (-12 .. +12)
    void SoundTouch_setPitchSemiTones(double newPitch);

    /// Sets the number of channels, 1 = mono, 2 = stereo
    void SoundTouch_setChannels(uint numChannels);

    /// Sets sample rate.
   // void setSampleRate(uint srate);


    /// Flushes the last samples from the processing pipeline to the output.
    /// Clears also the internal processing buffers.
    //
    /// Note: This function is meant for extracting the last samples of a sound
    /// stream. This function may introduce additional blank samples in the end
    /// of the sound stream, and thus it's not recommended to call this function
    /// in the middle of a sound stream.
    bool SoundTouch_flush(SAMPLETYPE *buff, int nframe);

    /// Adds 'numSamples' pcs of samples from the 'samples' memory position into
    /// the input of the object. Notice that sample rate _has_to_ be set before
    /// calling this function, otherwise throws a runtime_error exception.
    void SoundTouch_putSamples(
            const SAMPLETYPE *samples,  ///< Pointer to sample buffer.
            uint numSamples                         ///< Number of samples in buffer. Notice
                                                    ///< that in case of stereo-sound a single sample
                                                    ///< contains data for both channels.
            );

    /// Output samples from beginning of the sample buffer. Copies requested samples to 
    /// output buffer and removes them from the sample buffer. If there are less than 
    /// 'numsample' samples in the buffer, returns all that available.
    ///
    /// \return Number of samples returned.
    uint SoundTouch_receiveSamples(SAMPLETYPE *output, ///< Buffer where to copy output samples.
        uint maxSamples                 ///< How many samples to receive at max.
        );

    /// Adjusts book-keeping so that given number of samples are removed from beginning of the 
    /// sample buffer without copying them anywhere. 
    ///
    /// Used to reduce the number of samples in the buffer when accessing the sample buffer directly
    /// with 'ptrBegin' function.
     uint SoundTouch_Pre_receiveSamples(uint maxSamples   ///< Remove this many samples from the beginning of pipe.
        );

    /// Clears all the samples in the object's output and internal processing
    /// buffers.
    void SoundTouch_clear();

    
    /// Returns number of samples currently unprocessed.
     uint SoundTouch_numUnprocessedSamples() ;





#endif
