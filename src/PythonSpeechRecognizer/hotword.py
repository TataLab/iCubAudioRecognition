from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
import argparse
import sys
import tensorflow as tf
# pylint: disable=unused-import
from tensorflow.contrib.framework.python.ops import audio_ops as contrib_audio
# pylint: enable=unused-import
import yarp
from pprint import pprint
import pyaudio
import numpy as np
import requests
import threading
import json
from scipy import signal
import wave
import Queue
from collections import deque
from time import time

FLAGS = None
RECOGNITION_SAMPLE_RATE=16000

yarp.Network.init()
#writePort = yarp.BufferedPortBottle()
#writePort.open('/speech/text')

def load_graph(filename):
  """Unpersists graph from file as default graph."""
  with tf.gfile.FastGFile(filename, 'rb') as f:
    graph_def = tf.GraphDef()
    graph_def.ParseFromString(f.read())
    tf.import_graph_def(graph_def, name='')

def load_labels(filename):
  """Read in labels, one label per line."""
  return [line.rstrip() for line in tf.gfile.GFile(filename)]



class DataProcessor(yarp.PortReader):
    def __init__(self):
        yarp.PortReader.__init__(self)
        #self.audio = pyaudio.PyAudio()

        load_graph('audio_recognition_graph.pb');
        self.labels = load_labels('audio_recognition_labels.txt');

        self.sess = tf.Session();
        self.buffer = deque()
        self.lastPrediction = time()

    def recognize(self):

        with open('/tmp/test.wav', 'rb') as wav_file:
          wav_data = wav_file.read()

        softmax_tensor = self.sess.graph.get_tensor_by_name('labels_softmax:0')
        predictions, = self.sess.run(softmax_tensor, {'wav_data:0': wav_data})

        # Sort to show labels in order of confidence
        top_k = predictions.argsort()[-3:][::-1]

        if predictions[top_k[0]] < 0.5:
            return False;

        if time()-self.lastPrediction < 1.0:
            return False;

        self.lastPrediction = time()

        for node_id in top_k:
          human_string = self.labels[node_id]
          score = predictions[node_id]
          print('%s (score = %.5f)' % (human_string, score))

        print("\n")

        return True


    def read(self,connection):
        start = time()
        #print("in DataProcessor.read")
        if not(connection.isValid()):
            print("Connection shutting down")
            return False

        s = yarp.Sound()

        #print("Trying to read from connection")
        ok = s.read(connection)

        if not(ok):
            print("Failed to read input")
            return False

        data = []

        for i in range(s.getSamples()):
            data.append(s.get(i, 0));

        try:
            #data = np.array(data);

            srRatio = int(s.getFrequency()/RECOGNITION_SAMPLE_RATE);

            resampledData = data[0::srRatio]

            self.buffer.extend(resampledData)

            while len(self.buffer) > RECOGNITION_SAMPLE_RATE:
                self.buffer.popleft()

            #print(len(data))
            #print(len(resampledData))

            recognizeData = np.array(list(self.buffer), dtype='<i2');

            waveFile = wave.open('/tmp/test.wav', 'wb')
            waveFile.setnchannels(1)
            waveFile.setsampwidth(s.getBytesPerSample())
            waveFile.setframerate(int(RECOGNITION_SAMPLE_RATE))
            waveFile.writeframes(b''.join(np.array(recognizeData).tobytes()))
            waveFile.close()

            self.recognize()
            print("Time elapsed: ", time()-start)

        except Exception as e:
            print(e)

        return True




speechAudioPort = yarp.Port()
r = DataProcessor()
speechAudioPort.setReader(r)
speechAudioPort.open("/speech/audio");

yarp.Network.connect("/grabber", "/speech/audio");

yarp.Time.delay(100)
print("Test program timer finished")

yarp.Network.fini();
