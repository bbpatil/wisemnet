// *****************************************************************************************
// Copyright (C) 2017 Juan C. SanMiguel and Andrea Cavallaro
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
 * \file WiseMultiCameraPeriodicApp_ICF.cc
 * \author Juan C. SanMiguel (2017)
 * \brief Header file for the WiseMultiCameraPeriodicApp_ICF class
 * \version 1.4
 */

#ifndef __WISEMULTICAMERAPERIODICAPP_ICF__
#define __WISEMULTICAMERAPERIODICAPP_ICF__

#include <wiseCameraPeriodicApp/WiseCameraPeriodicApp.h>
#include <wiseMultiCameraPeriodicApp_ICF/WiseMultiCameraPeriodicApp_ICFPacket_m.h>
#include <wiseMultiCameraPeriodicApp_ICF/WiseMultiCameraPeriodicApp_ICF_utils.h> //include specific-structures for single-target tracking using ICF (in 'icf' namespace)

/*! \class WiseMultiCameraPeriodicApp_ICF
 *  \brief This class implements distributed single-target tracking based on the Information Consensus Filter
 *
 * This file is part of the implementation for the ICF tracker for single targets described in:
 *      Kamal et al, "Information Weighted Consensus Filters and Their Application
 *      in Distributed Camera Networks", IEEE TAC 2013
 *      http://ieeexplore.ieee.org/document/6576197/
 *
 */
class WiseMultiCameraPeriodicApp_ICF : public WiseCameraPeriodicApp
{

private:
    int _tracking_step_counter;  //!< internal counter for tracking steps

    //Target management
    int _n_targets;             //!< Number of targets being tracked by the node
    int _max_neigb_network;     //!< Maximum number of neighbors in the network (connectivity degree)

    //ICF filters for all targets (prior/posterior state & covariance, transition/measurement matrices,...)
    std::vector<icf::ICF_t> _ICFs;  //!< List of ICFs for each target
    unsigned int _dimS;             //!< Dimension of the state vector
    unsigned int _dimM;             //!< Dimension of the measurement vector
    float _procNoiseCov;            //!< Process noise covariance (input in .ned)
    float _measNoiseCov;            //!< Measurement noise covariance (input in .ned)

    //Consensus variables (generic for all targets)
    double _alpha;              //!< Rate of adaptation of the consensus (input in .ned)
    unsigned long _iter_max;    //!< Maximum number of iterations to be done (input in .ned)
    string _share;              //!< Type of neighbors to exchange data ("FOV" or "COM")

    //Buffers
    std::vector<std::vector<icf::round_data_t>>_rdata; //!< Structure to save own data for each round of the consensus
    std::vector<icf::node_ctrl_t> _node_controls; //!< Structure to save data received from other nodes
    std::vector<icf::icf_state_t> _camStatus;  //!< Status of neighbor cameras during the consensus iterations

    bool _collectNetworkStats;      //!< FLAG to collect and log network statistics
    bool _collectPowerStats;        //!< FLAG to collect and log power consumption statistics
    bool _collectAccuracyStats;     //!< FLAG to collect and log accuracy statistics
    bool _displayStats;             //!< FLAG to show data during runtime

protected:
    //Functions to be defined in sub-classes (mandatory)
    void at_timer_fired(int index);  //!< Response to alarms generated by specific algorithm. To define in superclass (mandatory)
    void at_startup();                 //!< Initialize internal variables. To define in sub-classes for each specific algorithm (mandatory)
    void at_finishSpecific();           //!< Release resources. To define in sub-classes for each specific algorithm (mandatory)
    bool at_init();                     //!< Initialize resources based on sampled data. To define in sub-classes for each specific algorithm (mandatory)
    bool at_sample();                   //!< Operations for processing each sample. To define in sub-classes for each specific algorithm (mandatory)
    bool at_end_sample();               //!< Operations after processing each sample. To define in sub-classes for each specific algorithm (mandatory)
    void make_measurements(const std::vector<WiseTargetDetection>&); //!< Conversion of camera detections into ordered lists of measurements for tracking. To define in sub-classes for each specific algorithm (mandatory)
    bool process_network_message(WiseBaseAppPacket *m); //!< Operations for processing each packet from network. To define in sub-classes for each specific algorithm (mandatory)
    void handleDirectApplicationMessage(WiseBaseAppPacket *); //!< Process a received packet from a direct node-to-node links (to be implemented in superclasses of WiseBaseApp)

    bool at_first_sample();        //!< Operations at 1st example.
    bool at_end_first_sample();   //!< Operations at the end of 1st example.
    bool displayStats();            //!< Display visual data while the simulation is running
private:
    void readParameters();
    void initStructures();         //!< Initialization of internal variables&structures.
    int sendICFmessage(int tid);   //!< Send a message to neigbourgs to perfom consensus.
    void logResult(int tid);       //!< Write results in a logfile

    void consensusStart(int tid);           //!< Prepare the data and start the consensus
    void consensusProcess(int tid);         //!< Process each round of the consensus
    void checkNextConsensusRound(int tid);  //!< Check availability of data for next consensus round
};

#endif // __WISEMULTICAMERAPERIODICAPP_ICF__
