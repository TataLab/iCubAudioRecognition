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
 * @file  audioFilteringRateThread.cpp
 * @brief Implementation of the processing ratethread.
 *        This is where the processing happens.
 */

#include "audioFilteringRateThread.h"

using namespace yarp::os;

#define THRATE 80 //ms


AudioFilteringRateThread::AudioFilteringRateThread() : RateThread(THRATE) {
	robot = "icub";
}


AudioFilteringRateThread::AudioFilteringRateThread(std::string _robot, std::string _configFile, yarp::os::ResourceFinder &rf) : RateThread(THRATE) {
	robot = _robot;
	configFile = _configFile;
	loadFile(rf);
}


AudioFilteringRateThread::~AudioFilteringRateThread() {
	delete inPort;
	delete outPort;
  delete beamformer;
  delete apm;
}


bool AudioFilteringRateThread::threadInit() {

	// input port for receiving raw audio
	inPort = new yarp::os::BufferedPort<yarp::sig::Sound>();
	if (!inPort->open("/iCubAudioRecognition/AudioFiltering:i")) {
		yError("unable to open port to receive input");
		return false;
	}


	// output port for sending GammaTone Filtered Audio
	outPort = new yarp::os::Port();
	if (!outPort->open("/iCubAudioRecognition/FilteredAudio:o")) {
		yError("unable to open port to send Gammatone Filtered Audio");
		return false;
	}




	// error checking
	if (yarp::os::Network::exists("/iCubAudioRecognition/AudioFiltering:i")) {
		if (yarp::os::Network::connect("/sender", "/iCubAudioRecognition/AudioFiltering:i") == false) {
			yError("Could not make connection to /sender. \nExiting. \n");
			return false;
		}
	}

	// inPort failed to open. quit.
	else {
		return false;
	}

  PI = acos(-1.0);
  SAMPLE_RATE = 48000;
  NUM_CHANNELS = 2;

  delay_ms = 20;
  voiceDetected = 0;
  webrtcErr = 0;

  SphericalPointf direction(PI/2.0f, 0.0f, 10.0f);

  initializeAudioProcessing(direction);




	// initializing completed successfully
	yInfo("Initialization of the processing thread correctly ended");

	return true;
}


void AudioFilteringRateThread::setName(std::string str) {
	this->name=str;
}


std::string AudioFilteringRateThread::getName(const char* p) {
	std::string str(name);
	str.append(p);
	return str;
}


void AudioFilteringRateThread::setInputPortName(std::string InpPort) {

}


void AudioFilteringRateThread::run() {

	// read in raw audio
	s = inPort->read(true);

  if (s!=NULL) {
      //Sound filtered = processFrame(*s);
      //cout << s->getSamples() << endl;
      //Sound filtered(*s);

      // int frameSize = s->getSamples();
      // //number of 10ms frames
      // int numFrames = frameSize/(SAMPLE_RATE/100);
      //
      // //remaining data at end
      // int remainingData = frameSize%(SAMPLE_RATE/100);
      //
      // //filter sound 10ms at a time
      // for(int i=0; i<numFrames; i++) {
      //   Sound section = s->subSound((frameSize/numFrames)*i, (frameSize/numFrames)*(i+1));
      //   processFrame(section);
      //
      //   for(int j=0; j<section.getSamples(); j++) {
      //     filtered.set(section.get(j, 0), (frameSize/numFrames)*i+j, 0);
      //     filtered.set(section.get(j, 1), (frameSize/numFrames)*i+j, 1);
      //   }
      // }
      //
      // //if sound length isn't multiple of sampling rate
      // if(remainingData > 0) {
      //   Sound endChunk = s->subSound(0, (SAMPLE_RATE/100));
      //
      //   //set chunk to zero
      //   for(int i=0; i< endChunk.getSamples(); i++) {
      //     endChunk.set(0, i, 0);
      //     endChunk.set(0, i, 1);
      //   }
      //
      //   //load remaining samples into beginning of chunk
      //   for(int i=0; i<remainingData; i++) {
      //     endChunk.set(s->get(frameSize-remainingData+i, 0), i, 0);
      //     endChunk.set(s->get(frameSize-remainingData+i, 1), i, 1);
      //   }
      //
      //   //filter remaining samples
      //   processFrame(endChunk);
      //
      //   //load remaining samples into end of filtered chunk
      //   for(int i=0; i<remainingData; i++) {
      //     filtered.set(endChunk.get(i, 0),frameSize-remainingData+i, 0);
      //     filtered.set(endChunk.get(i, 0),frameSize-remainingData+i, 1);
      //   }
      // }

      outPort->write(*s);
  }


	// timing how long the module took
	lastframe = ts.getCount();
	stopTime=yarp::os::Time::now();
	yInfo("elapsed time = %f\n",stopTime-startTime);
	startTime=stopTime;
}


bool AudioFilteringRateThread::processing() {
	// here goes the processing...
	return true;
}


void AudioFilteringRateThread::threadRelease() {
	// stop all ports
	inPort->interrupt();
	outPort->interrupt();


	// release all ports
	inPort->close();
	outPort->close();
}


void AudioFilteringRateThread::loadFile(yarp::os::ResourceFinder &rf) {

	// import all relevant data fron the .ini file
	yInfo("loading configuration file");
	try {
		frameSamples        =  rf.check("frameSamples",
										Value(4096),
										"frame samples (int)").asInt();

		nMics               =  rf.check("nMics",
										Value(2),
										"number mics (int)").asInt();

		micDistance         =  rf.check("micDistance",
										Value(0.145),
										"micDistance (double)").asDouble();

		C                   =  rf.check("C",
										Value(338),
										"C speed of sound (int)").asInt();

		samplingRate        =  rf.check("samplingRate",
										Value(48000),
										"sampling rate of mics (int)").asInt();

		// print information from rf to the console
		yInfo("micDistance = %f", micDistance);
		nBeamsPerHemi  = (int)((micDistance / C) * samplingRate) - 1;


		yInfo("frameSamples = %d", frameSamples);

		yInfo("nMics = %d", nMics);

	}

	catch (int a) {
		yError("Error in the loading of file");
	}

	yInfo("file successfully load");
}


void AudioFilteringRateThread::processFrame(Sound& input) {
   //cout << input.getSamples() << endl;

   int frameDataSize = input.getSamples() * input.getChannels();

    apm->set_stream_delay_ms(0);
    apm->gain_control()->set_stream_analog_level(analog_level);

   int16_t rawBuffer[frameDataSize];
   float floatBuffer[frameDataSize];
   ChannelBuffer<float> channelBuffer(input.getSamples(), input.getChannels());
   ChannelBuffer<float> filteredChannelBuffer(input.getSamples(), input.getChannels());

   float filteredBuffer[frameDataSize];
   int16_t filteredIntBuffer[frameDataSize];

   for(int i=0; i<frameDataSize; i++) {
     if(i%2==0)
        rawBuffer[i] = input.get(i/2, 0);
     else
        rawBuffer[i] = input.get(i/2, 1);
   }

   //convert to ChannelBuffer<float>
   S16ToFloat(rawBuffer, frameDataSize, floatBuffer);
   Deinterleave(floatBuffer, input.getSamples(), input.getChannels(), channelBuffer.channels());

   StreamConfig inputConfig(SAMPLE_RATE, input.getChannels(), false);
   StreamConfig outputConfig(SAMPLE_RATE, input.getChannels(), false);

   //process
   //beamformer->ProcessChunk(channelBuffer, &channelBuffer);

   if ((webrtcErr = apm->ProcessStream(channelBuffer.channels(), inputConfig, outputConfig, filteredChannelBuffer.channels())) < 0)
    cerr << "error processing stream: " << webrtcErr << " " << AudioProcessing::kNoError << endl;

   analog_level = apm->gain_control()->stream_analog_level();
   //run beamformer


   //convert back from ChannelBuffer<float> to int
   Interleave(filteredChannelBuffer.channels(), input.getSamples(), 1, filteredBuffer);
   FloatToS16(filteredBuffer, input.getSamples(), filteredIntBuffer);


   for(int i=0; i<input.getSamples(); i++) {
       input.set(filteredIntBuffer[i], i, 0);
       input.set(filteredIntBuffer[i], i, 1);
   }
}

void AudioFilteringRateThread::initializeAudioProcessing(SphericalPointf direction) {
  webrtc::Config config;

  vector<Point> array_geometry;

  array_geometry.push_back(Point(-0.04f, 0.f, 0.f));
  array_geometry.push_back(Point( 0.04f, 0.f, 0.f));

  beamformer = new NonlinearBeamformer(array_geometry, direction);
  beamformer->Initialize(10, SAMPLE_RATE);
  beamformer->AimAt(direction);

  apm = AudioProcessing::Create(config);

  analog_level = apm->gain_control()->stream_analog_level();

  apm->high_pass_filter()->Enable(true);

  apm->echo_cancellation()->enable_drift_compensation(false);
  apm->echo_cancellation()->Enable(true);

  apm->noise_suppression()->set_level(apm->noise_suppression()->kHigh);
  apm->noise_suppression()->Enable(true);

  apm->gain_control()->set_analog_level_limits(0, 255);
  apm->gain_control()->set_mode(apm->gain_control()->kAdaptiveAnalog);
  apm->gain_control()->Enable(true);

  //apm->voice_detection()->Enable(true);
}
