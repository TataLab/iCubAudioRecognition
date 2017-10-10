# Copyright: (C) 2013 Istituto Italiano di Tecnologia
# Author: Elena Ceseracciu
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

if (NOT iCubAudioRecognition_FOUND)

set(iCubAudioRecognition_LIBRARIES "" CACHE INTERNAL "List of iCubAudioRecognition libraries")

include("/usr/local/lib/iCubAudioRecognition/iCubAudioRecognition-export-install.cmake")
include("/usr/local/lib/iCubAudioRecognition/iCubAudioRecognition-export-install-includes.cmake")

set (iCubAudioRecognition_FOUND TRUE)
endif (NOT iCubAudioRecognition_FOUND)
