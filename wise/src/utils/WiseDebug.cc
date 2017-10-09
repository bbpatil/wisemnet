// *****************************************************************************************
// Copyright (C) 2011 Christian Nastasi, 2017 Juan C. SanMiguel and Andrea Cavallaro
// You may use, distribute and modify this code under the terms of the ACADEMIC PUBLIC
// license (see the attached LICENSE_WISE file).
//
// This file is part of the WiseMnet simulator
//
// Updated contact information:
//  - Juan C. SanMiguel - Universidad Autonoma of Madrid - juancarlos.sanmiguel@uam.es
//  - Andrea Cavallaro - Queen Mary University of London - a.cavallaro@qmul.ac.uk
//
// Please cite the following reference when publishing results obtained with WiseMnet:
//   J. SanMiguel & A. Cavallaro,
//   "Networked Computer Vision: the importance of a holistic simulator",
//   IEEE Computer, 50(7):35-43, Jul 2017, http://ieeexplore.ieee.org/document/7971873/
// *****************************************************************************************

/**
 * \file WiseDebug.cc
 * \author Juan C. SanMiguel (2017)
 * \author C. Nastasi (2011)
 * \brief Source file to create a unique WiseDebugLogger object
 * \version 1.4
 */
#include <WiseDebug.h>

#if (defined WISE_DEBUG_LEVEL)

std::fstream WiseDebugLogger::writer("wise_debug_log.txt", std::fstream::out);

#endif // WISE_DEBUG

