# iCub Audio Recognition

## About

This set of modules implement Audio Filtering, Speech Recognition, and Hotword Detection for the iCub. 

## Structure

### Audio Filtering

The Audio Filtering Module is located in the src/audioFiltering folder. Audio Filtering uses the [webrtc-audio-filtering library](https://github.com/TataLab/webrtc-audio-processing) for beamforming/filtering. The script setup.sh can be used to download and install this library.

The module then publishes filtered audio on the port /filteredAudio:o

### Speech Recognition

The folder src/PythonSpeechRecognizer contains multiple scripts for different forms of speech recognition.

#### hotword.py

This script performs basic Speech Recognition based on the model from the [Tensorflow Examples](https://www.tensorflow.org/versions/master/tutorials/audio_recognition). The recognized speech is then published on the port /speech:o.

#### recognize.py

This script performs large vocabulary speech recognition using Kaldi. The script needs a [speech recognition server](https://github.com/jcsilva/docker-kaldi-gstreamer-server) setup using [docker](https://www.docker.com/). 

#### PocketSphinx Speech Recognizer

The folder src/PocketsphinxSpeechRecognizer contains a module that performs speech recognition with PocketSphinx. 




