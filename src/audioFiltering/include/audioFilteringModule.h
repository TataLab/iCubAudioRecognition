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
 * @file  audioFilteringModule.h
 * @brief Header file of the processing module
 */

#ifndef _AUDIO_FILTERING_MODULE_H_
#define _AUDIO_FILTERING_MODULE_H_

#include <yarp/os/BufferedPort.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/Vector.h>

#include <audioFilteringRateThread.h>

#include <stdio.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/api.h>
#include <iostream>

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

class AudioFilteringModule: public yarp::os::RFModule {

 private:
	std::string moduleName;                  // name of the module
	std::string robotPortName;               // name of robot port
	std::string inputPortName;               // name of the input port for events
	std::string handlerPortName;             // name of handler port
	std::string robotName;                   // name of the robot
	std::string configFile;                  // name of the configFile that the resource Finder will seek
	yarp::os::Port handlerPort;              // a port to handle messages

	AudioFilteringRateThread* apr;        // ratethread handling the processing in the module


 public:
	bool configure(yarp::os::ResourceFinder &rf);
	bool interruptModule();
	bool close();
	double getPeriod();
	bool updateModule();
};

#endif //_AUDIO_FILTERING_MODULE_H_

//----- end-of-file --- ( next line intentionally left blank ) ------------------
