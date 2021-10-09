#include "Splitting.h"

#include "plexe/maneuver/Maneuver.h"
#include "plexe/apps/SplittingPlatooningApp.h"
#include "plexe/messages/InsideSplitArea_m.h" //CHANGE
#include "plexe/messages/InsideSplitAreaAck_m.h" //CHANGE
#include "plexe/messages/StartSplit_m.h" //CHANGE
#include "plexe/messages/SplitDone_m.h" //CHANGE
#include "plexe/messages/FinishAndUpdatePlatoons_m.h" //CHANGE
#include "plexe/messages/ConfirmPlatoonInfo_m.h" //CHANGE
#include "plexe/messages/Again_m.h" //CHANGE
#include "plexe/messages/Abort_m.h" //CHANGE
#include "plexe/messages/BranchDir_m.h" //CHANGE
#include "plexe/messages/BranchDirAck_m.h" //CHANGE
#include "plexe/messages/TestMSG_m.h" //CHANGE

#include "plexe/utilities/BasePositionHelper.h"
#include "veins/modules/mobility/traci/TraCIConstants.h"


namespace plexe {


Splitting::Splitting(GeneralPlatooningApp* app, int securityDistance, std::vector<int> vehicleDestination)
    : Maneuver(app)
    , splittingManeuverState(SplittingManeuverState::IDLE)
    , securityDistance(securityDistance)
    , vehicleDestination(vehicleDestination)
{
}

void Splitting::onManeuverMessage(const ManeuverMessage* mm)
{

    if (const InsideSplitArea* msg = dynamic_cast<const InsideSplitArea*>(mm)) {
        handleInsideSplitArea(msg);
    } else if (const InsideSplitAreaAck* msg = dynamic_cast<const InsideSplitAreaAck*>(mm)) {
        handleInsideSplitAreaAck(msg);
    } else if (const StartSplit* msg = dynamic_cast<const StartSplit*>(mm)) {
        handleStartSplit(msg);
    } else if (const SplitDone* msg = dynamic_cast<const SplitDone*>(mm)) {
        handleSplitDone(msg);
    } else if (const FinishAndUpdatePlatoons* msg = dynamic_cast<const FinishAndUpdatePlatoons*>(mm)) {
        handleFinishAndUpdatePlatoons(msg);
    } else if (const Again* msg = dynamic_cast<const Again*>(mm)) {
        handleAgain(msg);
    } else if (const Abort* msg = dynamic_cast<const Abort*>(mm)) {
        handleAbort();
    } else if (const BranchDir* msg = dynamic_cast<const BranchDir*>(mm)) {
        handleBranchDir(msg);
    } else if (const BranchDirAck* msg = dynamic_cast<const BranchDirAck*>(mm)) {
        handleBranchDirAck(msg);
    } else if (const TestMSG* msg = dynamic_cast<const TestMSG*>(mm)) {
        handleTestMsg(msg);
   }

}

void Splitting::handleTestMsg(const TestMSG* msg) {
    LOG << "MERDA VERA " << positionHelper->getId() << "\n";
    if (true) {
        /*params->posX_start = msg->getPosX_start();
        params->posY_start = msg->getPosY_start();
        params->posX_finish = msg->getPosX_finish();
        params->posY_finish = msg->getPosY_finish();*/



        /*BranchDirAck* response = new BranchDirAck("BranchDirAck");
        nextDestination = vehicleDestination[positionHelper->getPosition()];

        LOG << "MERDA\n";
        //response->setResponse(true);
        app->fillManeuverMessage(response, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), msg->getVehicleId());
        response->setDest(nextDestination);

        app->sendUnicast(response, msg->getVehicleId());

        splittingManeuverState = SplittingManeuverState::WAIT_SPLIT_AREA;*/
    }
}


bool Splitting::handleSelfMsg(cMessage* msg)
{
    std::string title = msg->getName();
    if (title.compare("TimeoutMsg") == 0) {
        abortManeuver();
    }
    else if (title.compare("StartSplitArea") == 0) {
        LOG << positionHelper->getId() << " handle self message split area \n";
        handleStartSplitAreaSelfMsg(msg);
    }

    return true;
}


//STEP 0 - send branch dir (set timeout and ack)
void Splitting::startManeuver(const void* parameters)
{
    nextDestination = vehicleDestination[positionHelper->getId()]; //get destination from platooncar info
    if (initializeSplittingManeuver())
    {

        params = {};
        SplittingParameters* parameters_t = (SplittingParameters*) parameters;
        params.posX_finish = parameters_t->posX_finish;
        params.posY_finish = parameters_t->posY_finish;
        params.posX_start = parameters_t->posX_start;
        params.posY_start = parameters_t->posY_start;
        sendAskBranchDir();

        vehicleDestinationLocal.resize(vehicleDestination.size());
        vehicleDestinationLocal[positionHelper->getMemberPosition(positionHelper->getLeaderId())] = nextDestination;

        splittingManeuverState = SplittingManeuverState::WAIT_FOLLOWERS_DIR;

        //sendSplitAreaStart(positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
        //sendLaneChangeRequest(positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId());
    }
}

void Splitting::sendAskBranchDir()
{
    for (int i: positionHelper->getPlatoonFormation()) {
        if (i != positionHelper->getLeaderId()){
            BranchDir* msg = new BranchDir("BranchDir");

            //TestMSG* msg = new TestMSG("TestMSG");

            app->fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), i);

            msg->setPosX_start(params.posX_start);
            msg->setPosY_start(params.posY_start);
            msg->setPosX_finish(params.posX_finish);
            msg->setPosY_finish(params.posY_finish);

            LOG << "Leader " << positionHelper->getId() << " sending BranchDir to the follower with id " << i << "\n";
            app->sendUnicast(msg, i);

        }
    }
}

bool Splitting::initializeSplittingManeuver()
{
    if (splittingManeuverState == SplittingManeuverState::IDLE && app->getPlatoonRole() == PlatoonRole::LEADER) {
        if (app->isInManeuver())
        {
            LOG << positionHelper->getId() << " cannot begin the maneuver because already involved in another one\n";
            return false;
        }

        // if (!isLaneFree(nextDestination)) return false;

        resetReceivedAck();

        static_cast<SplittingPlatooningApp*>(app)->sendTimeoutMsg();

        app->setInManeuver(true, this);

        return true;
    }
    else {
        return false;
    }
}

//STEP 1 - Follower handle branch dir ask and send ACK
void Splitting::handleBranchDir(const BranchDir* msg) {
    if (processBranchDir(msg)) {
        LOG << positionHelper->getId() << " handle BranchDir of leader (" << msg->getVehicleId() << ")\n";
        SplittingParameters params_temp = {};
        params_temp.posX_start = msg->getPosX_start();
        params_temp.posY_start = msg->getPosY_start();
        params_temp.posX_finish = msg->getPosX_finish();
        params_temp.posY_finish = msg->getPosY_finish();

        params = params_temp;

        BranchDirAck* response = new BranchDirAck("BranchDirAck");
        nextDestination = vehicleDestination[positionHelper->getPosition()];

        //response->setResponse(true);
        app->fillManeuverMessage(response, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), msg->getVehicleId());
        response->setDest(nextDestination);

        app->sendUnicast(response, msg->getVehicleId());

        LOG << positionHelper->getId() << " sending BranchDirAck to the leader (" << msg->getVehicleId() << ")\n";

        splittingManeuverState = SplittingManeuverState::WAIT_SPLIT_AREA;

    }
    else {
        abortManeuver();
    }
}

bool Splitting::processBranchDir(const BranchDir* msg)
{

    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) {
        LOG << positionHelper->getPlatoonId() << " different from " << msg->getPlatoonId() << "\n";
        return false;
    }

    if (app->getPlatoonRole() != PlatoonRole::FOLLOWER) {

        LOG << " is not follower \n";
        return false;
    }

    if (splittingManeuverState != SplittingManeuverState::IDLE || app->isInManeuver()) {
        LOG << " is not IDLE or is another Maneuver \n";
        return false;
    }

    //if (!isLaneFree(msg->getPlatoonLaneDestination())) return false;

    app->setInManeuver(true, this);

    return true;
}

// STEP 2 - Manage ACK and send self SPLIT AREA START (selg mess)

void Splitting::handleBranchDirAck(const BranchDirAck* msg)
{
    if (processBranchDirAck(msg)) {
        LOG << positionHelper->getId() << " handle all BranchDirAck of all follower (" << ")\n";

        cMessage* selfmsg = new cMessage("StartSplitArea");

        VEHICLE_DATA data;
        // get information about the vehicle via traci
        plexeTraciVehicle->getVehicleData(&data);

        //estimate time (rounded)
        int estimate_time = (params.posX_start - data.positionX) / data.speedX;

        if (estimate_time < 0)
            estimate_time = 0;

        LOG << positionHelper->getId() << " send self message to prepare start split area \n";
        static_cast<SplittingPlatooningApp*>(app)->sendScheduleMsg(estimate_time, selfmsg);


        cMessage* startMessage = new cMessage();


        splittingManeuverState = SplittingManeuverState::WAIT_SPLIT_AREA;
    }
}


bool Splitting::processBranchDirAck(const BranchDirAck* msg) {
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return false;

    if (app->getPlatoonRole() != PlatoonRole::LEADER) return false;

    if (splittingManeuverState != SplittingManeuverState::WAIT_FOLLOWERS_DIR || !app->isInManeuver()) return false;

    receivedAck[msg->getVehicleId()] = true;
    vehicleDestinationLocal[positionHelper->getMemberPosition(msg->getVehicleId())] = msg->getDest();

    LOG << positionHelper->getId() << " handle BranchDirAck of follower (" << msg->getVehicleId() << ")\n";

    for (auto const& x : receivedAck)
    {
        if(!x.second) return false;
    }

    static_cast<SplittingPlatooningApp*>(app)->resetTimeoutMsg();
    resetReceivedAck();

    return true;
}

// STEP 3 Handle self start area

void Splitting::handleStartSplitAreaSelfMsg(cMessage* msg) {
    VEHICLE_DATA data;
    // get information about the vehicle via traci
    plexeTraciVehicle->getVehicleData(&data);

    std::cout << data.positionX << " " << params.posX_start << "\n";
    if (data.positionX < params.posX_start) {
        int estimate_time = (params.posX_start - data.positionX) / data.speedX;
        std::cout << estimate_time << "\n";

        if (estimate_time < 0)
            estimate_time = 0;
        LOG << positionHelper->getId() << " send self message to prepare start split area \n";
        static_cast<SplittingPlatooningApp*>(app)->sendScheduleMsg(estimate_time, msg);
    }
    else {
        sendInsideSplitArea();
    }
}

void Splitting::sendInsideSplitArea()
{
    resetReceivedAck();
    static_cast<SplittingPlatooningApp*>(app)->sendTimeoutMsgForSplitArea();
    for (int i: positionHelper->getPlatoonFormation()) {
        if (i != positionHelper->getLeaderId()){
            LOG << positionHelper->getId() << " sending LeaderInsideSplit to " << i << "\n";
            InsideSplitArea* msg = new InsideSplitArea("InsideSplitArea");
            app->fillManeuverMessage(msg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), i);
            app->sendUnicast(msg, i);

            //riduco velocità di 20 perchè sono all'interno dell'area
            plexeTraciVehicle->setCruiseControlDesiredSpeed((static_cast<SplittingPlatooningApp*>(app)->getLeaderSpeed() - 20)/3.6);
            splittingManeuverState = SplittingManeuverState::WAIT_ALL_INSIDE;
        }
    }
}

// STEP 4 Inside Split Area

void Splitting::handleInsideSplitArea(const InsideSplitArea* msg)
{
    if (processInsideSplitArea(msg)) {
        plexeTraciVehicle->setCACCConstantSpacing(10); //andrà messo come par

        plexeTraciVehicle->setCruiseControlDesiredSpeed((static_cast<SplittingPlatooningApp*>(app)->getFollowerSpeed() - 20)/3.6);
        splittingManeuverState = SplittingManeuverState::WAIT_TO_BE_INSIDE;

    } else {
        abortManeuver();
    }
}

//support step 2
bool Splitting::processInsideSplitArea(const InsideSplitArea* msg)
{
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) {
        LOG << positionHelper->getPlatoonId() << " different from " << msg->getPlatoonId() << "\n";
        return false;
    }

    if (app->getPlatoonRole() != PlatoonRole::FOLLOWER) {

        LOG << " is not follower \n";
        return false;
    }

    if (splittingManeuverState != SplittingManeuverState::WAIT_SPLIT_AREA || !app->isInManeuver()) {
        LOG << " is not WAIT_SPLIT_AREA or is another Maneuver \n";
        return false;
    }

    return true;
}

void Splitting::onPlatoonBeacon(const PlatooningBeacon* pb)
{
    if (splittingManeuverState == SplittingManeuverState::WAIT_TO_BE_INSIDE) {
        // check correct role
        ASSERT(app->getPlatoonRole() == PlatoonRole::FOLLOWER);

        // if the message comes from the front vehicle
        int frontId = positionHelper->getFrontId();
        if (pb->getVehicleId() == frontId) {
            Coord frontPosition(pb->getPositionX(), pb->getPositionY(), 0);
            // get my position
            veins::TraCICoord traciPosition = mobility->getManager()->getConnection()->omnet2traci(mobility->getPositionAt(simTime()));
            Coord position(traciPosition.x, traciPosition.y);
            // compute distance
            double distance = position.distance(frontPosition) - pb->getLength();

            LOG << positionHelper->getId() << " data beacon " << distance << " \n";
            if (distance > 7) {
                VEHICLE_DATA data;
                plexeTraciVehicle->getVehicleData(&data);
                if (data.positionX > params.posX_start and data.positionX < (params.posX_finish - 100)) {

                    LOG << positionHelper->getId() << " sending FollowerInsideSplit to the leader \n";
                    InsideSplitAreaAck* response = new InsideSplitAreaAck("InsideSplitAreaAck");
                    response->setResponse(true);
                    app->fillManeuverMessage(response, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), positionHelper->getLeaderId());
                    app->sendUnicast(response, positionHelper->getLeaderId());

                    //riduco la velocità di 20 perchè sono all'interno dell'area

                    splittingManeuverState = SplittingManeuverState::PREPARE_TO_SPLIT;
                }
            }
        }
    }
}


//STEP 5

void Splitting::handleInsideSplitAreaAck(const InsideSplitAreaAck* msg)
{
    if (processInsideSplitAreaAck(msg)) {
        if (nextDestination >= 0)
        {

            plexeTraciVehicle->setFixedLane(nextDestination, false);
            positionHelper->setPlatoonLane(nextDestination);

            // send response to followers
            for (int i: positionHelper->getPlatoonFormation())
            {
                if (i != positionHelper->getLeaderId())
                {
                    LOG << "Leader " << positionHelper->getId() << " sending StartSplit to the follower with id " << i << "\n";
                    StartSplit* response = new StartSplit("StartSplit");
                    app->fillManeuverMessage(response, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), i);
                    app->sendUnicast(response, i);
                }
            }

            splittingManeuverState = SplittingManeuverState::WAIT_ALL_FINISH;
            static_cast<SplittingPlatooningApp*>(app)->sendTimeoutMsg();


        } else
            abortManeuver();
    }
}

bool Splitting::processInsideSplitAreaAck(const InsideSplitAreaAck* msg)
{
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return false;

    if (app->getPlatoonRole() != PlatoonRole::LEADER) return false;

    if (splittingManeuverState != SplittingManeuverState::WAIT_ALL_INSIDE || !app->isInManeuver()) return false;

    receivedAck[msg->getVehicleId()] = true;

    for (auto const& x : receivedAck)
    {
        if(!x.second) return false;
    }

    static_cast<SplittingPlatooningApp*>(app)->resetTimeoutMsg();
    resetReceivedAck();
    return true;
}

// STEP 6
void Splitting::handleStartSplit(const StartSplit* msg)
{
    if (processStartSplit(msg)) {

        plexeTraciVehicle->setFixedLane(nextDestination, false);

        positionHelper->setPlatoonLane(nextDestination);

        // send response to the leader
        LOG << positionHelper->getId() << " sending SplitDone to the leader (" << msg->getVehicleId() << ")\n";

        SplitDone* response = new SplitDone("SplitDone");
        app->fillManeuverMessage(response, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), msg->getVehicleId());
        app->sendUnicast(response, msg->getVehicleId());

        splittingManeuverState = SplittingManeuverState::WAIT_NEW_PLATOON_INFO;

    } else {
        abortManeuver();
    }
}

bool Splitting::processStartSplit(const StartSplit* msg)
{
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return false;

    if (app->getPlatoonRole() != PlatoonRole::FOLLOWER) return false;

    if (splittingManeuverState != SplittingManeuverState::PREPARE_TO_SPLIT || !app->isInManeuver()) return false;

    if (nextDestination < 0) return false;

    return true;
}


//STEP 7
void Splitting::handleSplitDone(const SplitDone* msg)
{
    if (processSplitDone(msg)) {

        std::vector<int> old_formation = positionHelper->getPlatoonFormation();


        // send response to followers
        std::vector<int> new_formation_0;
        std::vector<int> new_formation_1;

        for (int i: old_formation) {
            if (vehicleDestinationLocal[i] == 0) {
                LOG << vehicleDestinationLocal[i] << " ecco qua \n";
                new_formation_0.push_back(i);
            }
            else
                new_formation_1.push_back(i);
        }

        int new_platoon_id_0 = positionHelper->getPlatoonId() + 1;


        for (int i: old_formation)
        {

            if (i != positionHelper->getLeaderId())
            {
                LOG << "Leader " << positionHelper->getId() << " sending FinishAndUpdatePlatoons to the follower with id " << i << " and PLATOON ID "<< nextDestination << "\n";
                FinishAndUpdatePlatoons* updatemsg = new FinishAndUpdatePlatoons("FinishAndUpdatePlatoons");
                app->fillManeuverMessage(updatemsg, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), i);

                if (vehicleDestination[i] == 1) {
                    updatemsg->setIdNewPlatoon(new_platoon_id_0 + 1);
                    updatemsg->setNewFormationArraySize(new_formation_1.size());
                    for (unsigned int i = 0; i < new_formation_1.size(); i++) {
                        updatemsg->setNewFormation(i, new_formation_1[i]);
                    }
                }
                else {
                    updatemsg->setIdNewPlatoon(new_platoon_id_0);
                    updatemsg->setNewFormationArraySize(new_formation_0.size());
                    for (unsigned int i = 0; i < new_formation_0.size(); i++) {
                        updatemsg->setNewFormation(i, new_formation_0[i]);
                    }

                }
                app->sendUnicast(updatemsg, i);

            }
        }


        if (vehicleDestination[0] == 1) {
            positionHelper->setPlatoonId(new_platoon_id_0 + 1);
            positionHelper->setPlatoonFormation(new_formation_1);
            traciVehicle->changeVehicleRoute({"gneE0", "gneE3", "gneE6", "gneE8"});
        } else {
            positionHelper->setPlatoonId(new_platoon_id_0);
            positionHelper->setPlatoonFormation(new_formation_0);
        }





        plexeTraciVehicle->setCruiseControlDesiredSpeed(static_cast<SplittingPlatooningApp*>(app)->getLeaderSpeed()/3.6);

        nextDestination = -1;
        app->setInManeuver(false, this);
        splittingManeuverState = SplittingManeuverState::IDLE;


    }
}

bool Splitting::processSplitDone(const SplitDone* msg)
{
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return false;

    if (app->getPlatoonRole() != PlatoonRole::LEADER) return false;

    if (splittingManeuverState != SplittingManeuverState::WAIT_ALL_FINISH || !app->isInManeuver()) return false;

    receivedAck[msg->getVehicleId()] = true;

    for (auto const& x : receivedAck)
    {
        if(!x.second) return false;
    }

    static_cast<SplittingPlatooningApp*>(app)->resetTimeoutMsg();
    resetReceivedAck();
    return true;
}


//STEP 8

void Splitting::handleFinishAndUpdatePlatoons(const FinishAndUpdatePlatoons* msg)
{
    std::vector<int> new_formation;
    if (processFinishAndUpdatePlatoons(msg)) {

        int vehicle_id = positionHelper->getId();

        new_formation.resize(msg->getNewFormationArraySize());
        for (unsigned int i = 0; i < msg->getNewFormationArraySize(); i++) {
            LOG << msg->getNewFormation(i) << " ecco qua 2\n";
            new_formation[i] = msg->getNewFormation(i);

        }

        positionHelper->setPlatoonId(msg->getIdNewPlatoon());
        positionHelper->setPlatoonFormation(new_formation);

        if (positionHelper->isLeader()) {

            plexeTraciVehicle->setActiveController(ACTIVE_CONTROLLER::ACC);
            plexeTraciVehicle->setCruiseControlDesiredSpeed(static_cast<SplittingPlatooningApp*>(app)->getLeaderSpeed()/3.6);
            app->setPlatoonRole(PlatoonRole::LEADER);

            if (nextDestination == 1) {
                traciVehicle->changeVehicleRoute({"gneE0", "gneE3", "gneE6", "gneE8"});

            }

            nextDestination = -1;
            app->setInManeuver(false, this);
            splittingManeuverState = SplittingManeuverState::IDLE;


        }
        else {
            app->setPlatoonRole(PlatoonRole::FOLLOWER);
            plexeTraciVehicle->setActiveController(ACTIVE_CONTROLLER::CACC);
            plexeTraciVehicle->setCruiseControlDesiredSpeed(static_cast<SplittingPlatooningApp*>(app)->getFollowerSpeed()/3.6);

            if (nextDestination == 1) {
                traciVehicle->changeVehicleRoute({"gneE0", "gneE3", "gneE6", "gneE8"});
            }


            LOG << "Follower " << positionHelper->getId() << " sending ConfirmPlatoon to the Leader with id " << positionHelper->getLeaderId() << "\n";
            //ConfirmPlatoonInfo* response = new ConfirmPlatoonInfo("ConfirmPlatoonInfo");
            //app->fillManeuverMessage(response, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), positionHelper->getLeaderId());
            //app->sendUnicast(response, positionHelper->getLeaderId());
            plexeTraciVehicle->setCACCConstantSpacing(securityDistance);

            nextDestination = -1;
            app->setInManeuver(false, this);
            splittingManeuverState = SplittingManeuverState::IDLE;

        }

        positionHelper->dumpVehicleData();

    } else {
        abortManeuver();
    }
}


bool Splitting::processFinishAndUpdatePlatoons(const FinishAndUpdatePlatoons* msg)
{
    if (msg->getPlatoonId() != positionHelper->getPlatoonId()) return false;
    if (app->getPlatoonRole() != PlatoonRole::FOLLOWER) return false;
    if (splittingManeuverState != SplittingManeuverState::WAIT_NEW_PLATOON_INFO || !app->isInManeuver()) return false;

    return true;
}






//OTHER

void Splitting::resetReceivedAck() {
    for (int i : positionHelper->getPlatoonFormation()) {
        if (i != positionHelper->getLeaderId()) {
            receivedAck[i] = false;
        }
    }
}


void Splitting::handleAbort()
{
    nextDestination = -1;
    app->setInManeuver(false, this);
    splittingManeuverState = SplittingManeuverState::IDLE;
}

void Splitting::abortManeuver()
{
    for (int i: positionHelper->getPlatoonFormation())
    {
        if (i != positionHelper->getId())
        {
            LOG << "Platoon member " << positionHelper->getId() << " sending Abort to member " << i << "\n";
            Abort* response = new Abort("Abort");
            app->fillManeuverMessage(response, positionHelper->getId(), positionHelper->getExternalId(), positionHelper->getPlatoonId(), i);
            app->sendUnicast(response, i);
        }
    }
    nextDestination = -1;
    app->setInManeuver(false, this);
    splittingManeuverState = SplittingManeuverState::IDLE;
}

void Splitting::onFailedTransmissionAttempt(const ManeuverMessage* mm)
{
    throw cRuntimeError("Impossible to send this packet: %s. Maximum number of unicast retries reached", mm->getName());
}


void Splitting::handleAgain(const Again* msg)
{
    LOG<<"Restart laneChange";
    startManeuver(nullptr);
}


} // namespace plexe
