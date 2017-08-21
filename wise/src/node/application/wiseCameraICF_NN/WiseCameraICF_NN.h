// *****************************************************************************
//  Copyright (C): Juan C. SanMiguel, 2015
//  Author(s): Juan C. SanMiguel
//  Developed at Universidad Autonoma of Madrid
//  This file is distributed under the terms in the attached LICENSE_2 file.  
//  If you do not find this file, copies can be found by writing to:
//  - juan.carlos.sanmiguel@uam.es
//  - andrea.cavallaro@eecs.qmul.ac.uk                                        
// *****************************************************************************

#ifndef __WISECAMERAS_ICF_NN_H__
#define __WISECAMERAS_ICF_NN_H__

#include "WiseCameraSimplePeriodicTracker.h"
#include "WiseDefinitionsTracking.h" //include for definitions of states and measurements
#include "WiseCameraICFMsg_m.h"
#include "WiseCameraICF_utils.h" //include specific-structures for single-target tracking using ICF (in 'icf' namespace)

#define MAX_ITER_BUFFER_ICFNN 20

/*! \class WiseCameraICF_NN
 *  \brief This class implements distributed multi-target tracking based on Information Consensus Filter
 *
 *  The KCF algorithm has been modified to be used for multi-target tracking by using ground-truth for
 *  associating previous targets and obtained measurements - see function make_measurements()
 *
 *  More details available at "Kamal et al., XXXXX, IEEE PAMI2015"
 */
class WiseCameraICF_NN : public WiseCameraSimplePeriodicTracker
{

private:
    static ofstream logger; //!< Used to collect node print-out

        unsigned int dimS; //!< Dimension of the state vector
        unsigned int dimM; //!< Dimension of the measurement vector

        std::vector<icf::node_ctrl_t> node_controls; //!< Structure for controlling node's operations

        //Target management
        std::vector<cv::Mat> zlist; //list containing measurements and clutter
        int n_targets;       //!< Number of targets being tracked by the node
        int max_neigb_network; //!< Maximum number of neighbors in the network (connectivity degree)

        //ICF filters for all targets (prior/posterior state & covariance, transition/measurement matrices,...)
        std::vector<icf::ICF_t> ICFs;//!< List of ICFs for each target
        float procNoiseCov;            //!< Common process noise covariance (input in .ned)
        float measNoiseCov;            //!< Common measurement noise covariance (input in .ned)

       //Consensus variables (generic for all targets)
    double alpha;               //!< Rate of adaptation of the consensus (input in .ned)
    unsigned long iter_max;    //!< Maximum number of iterations to be done (input in .ned)
    string share;        //!< ...

    std::vector<icf::icf_state_t> camStatus;

    //Association MTT variables
    double lambda; //!< clutter per camera sensor (high value --> more clutter measures)
    int countClutters;

    bool collectNetworkStats;
    bool collectPowerStats;
    bool collectAccuracyStats;

public:
    virtual ~WiseCameraICF_NN();

protected:
    // Functions to be implemented from WiseCameraSimplePeriodicTracker class
    virtual void at_startup();                       //!< Init internal variables. To implement in superclasses of WiseCameraSimplePeriodicTracker.
    virtual void at_finishSpecific();
    virtual void at_timer_fired(int index);     //!< Response to alarms generated by specific tracker. To implement in superclasses of WiseCameraSimplePeriodicTracker.
    virtual void at_tracker_init();                 //!< Init resources. To implement in superclasses of WiseCameraSimplePeriodicTracker.
    virtual bool at_tracker_first_sample();        //!< Operations at 1st example. To implement in superclasses of WiseCameraSimplePeriodicTracker.
    virtual bool at_tracker_end_first_sample();   //!< Operations at the end of 1st example. To implement in superclasses of WiseCameraSimplePeriodicTracker.
    virtual bool at_tracker_sample();               //!< Operations at the >1st example. To implement in superclasses of WiseCameraSimplePeriodicTracker.
    virtual bool at_tracker_end_sample();          //!< Operations at the end of >1st example. To implement in superclasses of WiseCameraSimplePeriodicTracker.

    // Functions to be implemented from WiseBaseApplication class
    virtual void process_network_message(WiseApplicationPacket *); //!< Processing of packets received from network. To implement in superclasses of WiseBaseApplication.
    virtual void handleDirectApplicationMessage(WiseApplicationPacket *); //!< Processing of packets received from network. To implement in superclasses of WiseBaseApplication.
    virtual void make_measurements(const std::vector<WiseTargetDetection>&);  //!< Conversion of camera detections into ordered lists of measurements for tracking. To implement in superclasses of WiseBaseApplication.

    virtual void handleMacControlMessage(cMessage *);

private:
    void readParameters();
    void initStructures();        //!< Initialization of internal variables&structures.
    int sendICFmessage(int tid);  //!< Send a message to neigbourgs to perfom consensus.
    void logResult(int tid);       //!< Write results in a logfile

    //void dataAssociation();
    void consensusStart(int tid);
    void consensusProcess(int tid, int k);
    void checkNextConsensusRound(int tid, int k);

    void generateClutterMeasurements (std::vector<cv::Mat> *zlist);

    void dataAssociation();
    cv::Mat compute_association_NN (std::vector<cv::Mat> zlist);

    void hungarian(cv::Mat weights, cv::Mat *outMatches, cv::Mat *outCost);
    int** mat_to_matrix(cv::Mat, int rows, int cols);
    cv::Mat matrix_to_mat(int**, int rows, int cols);

    int findIndexInBuffer(int tid, int iter_index);
    void storeDataInBuffer(int tid, int iter_index, int nodeID, WiseCameraICFMsg *m);
};

#endif // __WISECAMERAS_ICF_NN_H__