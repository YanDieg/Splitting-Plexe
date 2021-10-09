#ifndef SPLITTING_H_
#define SPLITTING_H_

#include <algorithm>

#include "plexe/maneuver/Maneuver.h"

#include "plexe/messages/TestMSG_m.h" //CHANGE
#include "plexe/messages/InsideSplitArea_m.h" //CHANGE
#include "plexe/messages/InsideSplitAreaAck_m.h" //CHANGE
#include "plexe/messages/StartSplit_m.h" //CHANGE
#include "plexe/messages/SplitDone_m.h" //CHANGE
#include "plexe/messages/FinishAndUpdatePlatoons_m.h" //CHANGE
#include "plexe/messages/Again_m.h" //CHANGE
#include "plexe/messages/Abort_m.h" //CHANGE
#include "plexe/messages/BranchDir_m.h" //CHANGE
#include "plexe/messages/BranchDirAck_m.h" //CHANGE


using namespace veins;

namespace plexe {

struct SplittingParameters {
    int posX_start;
    int posY_start;
    int posX_finish;
    int posY_finish;
};

class Splitting : public Maneuver {

public:

    Splitting(GeneralPlatooningApp* app, int securityDistance, std::vector<int> vehicleDestination);
    ~Splitting(){};

    virtual void startManeuver(const void* parameters) override;


    void handleInsideSplitArea(const InsideSplitArea* msg);


    void handleInsideSplitAreaAck(const InsideSplitAreaAck* msg);

    void handleAgain(const Again* msg);


    void handleStartSplit(const StartSplit* msg);


    void handleSplitDone(const SplitDone* msg);


    void handleFinishAndUpdatePlatoons(const FinishAndUpdatePlatoons* msg);

    void handleBranchDirAck(const BranchDirAck* msg);
    void handleBranchDir(const BranchDir* msg);
    void handleTestMsg(const TestMSG* msg);

    void handleStartSplitAreaSelfMsg(cMessage* msg);


    void handleAbort();

    virtual void abortManeuver() override;
    virtual void onFailedTransmissionAttempt(const ManeuverMessage* mm) override;
    virtual void onManeuverMessage(const ManeuverMessage* mm) override;
    virtual void onPlatoonBeacon(const PlatooningBeacon* pb) override;
protected:
    /** Possible states a vehicle can be in during a laneChange maneuver */
    enum class SplittingManeuverState {
        WAIT_SPLIT_AREA,
        WAIT_ALL_INSIDE,
        PREPARE_TO_SPLIT,
        WAIT_ALL_FINISH,
        WAIT_NEW_PLATOON_INFO,
        IDLE,
        START_SPLIT,
        SPLIT,
        WAIT_DIR_RESP,
        WAIT_TO_BE_INSIDE,
        WAIT_FOLLOWERS_DIR

    };

    SplittingManeuverState splittingManeuverState;

    bool initializeSplittingManeuver();

    bool processInsideSplitArea(const InsideSplitArea* msg);

    bool processInsideSplitAreaAck(const InsideSplitAreaAck* msg);

    bool processStartSplit(const StartSplit* msg);

    bool processSplitDone(const SplitDone* msg);

    bool processFinishAndUpdatePlatoons(const FinishAndUpdatePlatoons* msg);

    bool processBranchDirAck(const BranchDirAck* msg);

    bool processBranchDir(const BranchDir* msg);

private:

    void sendAskBranchDir();
    void sendInsideSplitArea();
    void resetReceivedAck();

    int destination();

    virtual bool handleSelfMsg(cMessage* msg) override;

    std::map<int, bool> receivedAck;
    // -1 no destination value
    int nextDestination = -1;
    std::vector<int> vehicleDestination;
    std::vector<int> vehicleDestinationLocal;
    int securityDistance;
    SplittingParameters params;


};

} // namespace plexe

#endif
