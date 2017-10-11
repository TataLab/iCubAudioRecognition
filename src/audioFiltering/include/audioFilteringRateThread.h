// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
  * Copyright (C)2017  Department of Neuroscience - University of Lethbridge
  * Author:Matt Tata, Marko Ilievski
  * email: m.ilievski@uleth.ca, matthew.tata@uleth.ca, francesco.rea@iit.it
  * Permission is granted to copy, distribute, and/or modify this program
  * under the terms of the GNU General Public License, version 2 or any
  * later version published by the Free Software Foundation.
  *
  * A copy of the license can be found at
  * http://www.robotcub.org/icub/license/gpl.txt
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
  * Public License for more details
*/

/**
 * @file  audioFilteringRatethread.h
 * @brief Header file of the processing ratethread.
 *        This is where the processing happens.
 */

#ifndef _AUDIO_FILTERING_RATETHREAD_H_
#define _AUDIO_FILTERING_RATETHREAD_H_

#include <yarp/dev/all.h>

#include <yarp/math/Math.h>

#include <yarp/os/all.h>
#include <yarp/os/Log.h>
#include <yarp/os/RateThread.h>

#include <yarp/sig/all.h>

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/NetInt32.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>

#include "webrtc/modules/audio_processing/include/audio_processing.h"
#include "webrtc/modules/interface/module_common_types.h"
#include "webrtc/common_audio/channel_buffer.h"
#include "webrtc/modules/audio_processing/beamformer/nonlinear_beamformer.h"
#include "webrtc/common_audio/include/audio_util.h"

using namespace std;
using namespace webrtc;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


const float normDivid = pow(2,15);

//const float normDivid = pow(2,23);  // Number that is used to convert the integer number received
                                    // as the audio signal and convert it to a double audio signal

struct knotValues {
    double k0, k1, k2;
};

class AudioFilteringRateThread : public yarp::os::RateThread {

 private:
	//
	// name strings
	//
	std::string configFile;         // name of the configFile where the parameter of the camera are set
	std::string inputPortName;      // name of input port for incoming events, typically from aexGrabber
	std::string name;               // rootname of all the ports opened by this thread
	std::string robot;              // name of the robot


	//
	// Incoming Audio Data from the iCub and remoteInterface
	//
	yarp::os::BufferedPort<yarp::sig::Sound> *inPort;
	yarp::os::Port *outPort;

	yarp::os::Stamp ts;
	yarp::sig::Sound* s;

	float *rawAudio;


	//
	// processing objects
	//
  NonlinearBeamformer* beamformer;
  AudioProcessing* apm;

  double PI;
  int SAMPLE_RATE;
  int NUM_CHANNELS;


  int analog_level;
  int delay_ms;
  int voiceDetected;
  int webrtcErr;

    //
    // derived variables
    //
    int lastframe;
    int totalBeams;
    int longBufferSize;
    int nBeamsPerHemi;

    double startTime;
    double stopTime;


    //
    // variables from resource finder
    //
    int C;
    int frameSamples;
    int nMics;
    int samplingRate;

    double micDistance;

    void initializeAudioProcessing(SphericalPointf direction);
    void processFrame(Sound& input);

 public:
    /**
     *  constructor
     */
    AudioFilteringRateThread();


    /**
     *  constructor
     *
     *  set robotname and configFile, the calls loadFile
     *  on the passed in resource finder
     *
     *  @param   _robotname : name of the robot
     *  @param  _configFile : configuration file
     *  @param           rf : resource finder object for setting constants
     */
    AudioFilteringRateThread(std::string _robotname, std::string _configFile, yarp::os::ResourceFinder &rf);


    /**
     *  destructor
     */
    ~AudioFilteringRateThread();


    /**
     *  threadInit
     *
     *  initialises the thread
     *
     *  @return whether or not initialization executed correctly
     */
    bool threadInit();


    /**
     *  threadRelease
     *
     *  correctly releases the thread
     */
    void threadRelease();


    /**
     *  run
     *
     *  active part of the thread
     */
    void run();


    /**
     *  setName
     *
     *  function that sets the rootname of all the ports that are going to be created by the thread
     *
     *  @param str : the rootname used for all ports opened by this thread
     */
    void setName(std::string str);

    /**
     *  getName
     *
     *  function that returns the original root name and appends another string iff passed as parameter
     *
     *  @param p : pointer to the string that has to be added
     *
     *  @return rootname
     */
    std::string getName(const char* p);

    /**
     *  setInputPortName
     *
     *  function that sets the inputPort name
     *
     *  @param inPrtName : the name to set input ports to
     */
    void setInputPortName(std::string inpPrtName);

    /**
     *  processing
     *
     *  method for the processing in the ratethread
     *
     *  @return whether processing was successful
     */
    bool processing();

    /**
     *  loadFile
     *
     *  Accesses the loadFile.xml that is found in the root directory of this
     *  module and load all required parameters for the beam former.
     *
     *  @param rf : resource finder object containing the values of presets
     */
    void loadFile(yarp::os::ResourceFinder &rf);


};

#endif  //_AUDIO_FILTERING_THREAD_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
