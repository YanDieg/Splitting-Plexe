//
// Copyright (C) 2012-2021 Michele Segata <segata@ccs-labs.org>
// Copyright (C) 2018-2021 Julian Heinovski <julian.heinovski@ccs-labs.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//



#include "plexe/apps/GeneralPlatooningApp.h"
#include "plexe/protocols/BaseProtocol.h"
#include "SplittingPlatooningApp.h"
#include "plexe/scenarios/SplittingManeuverScenario.h"
#include "veins/modules/mac/ieee80211p/Mac1609_4.h"
#include "plexe/messages/BranchWarning_m.h" //CHANGE



using namespace veins;

namespace plexe {

Define_Module(SplittingPlatooningApp);

void SplittingPlatooningApp::initialize(int stage)
{
    GeneralPlatooningApp::initialize(stage);

    if (stage == 1) {
        std::string splittingManeuverName = par("splittingManeuver").stdstringValue();

        std::string vehicleDestinationString = par("vehicleDestination");

        if (splittingManeuverName == "Splitting") {
            std::vector<int> vehicleDestination;
            if (!vehicleDestinationString.empty()) {
                std::string::size_type start = 0;
                do {
                    size_t x = vehicleDestinationString.find(",", start);
                    if (x == std::string::npos)
                        break;
                    vehicleDestination.push_back(std::atoi(vehicleDestinationString.substr(start, x-start).c_str()));
                    start = x + 1;

                }
                while (true);

                vehicleDestination.push_back(std::atoi(vehicleDestinationString.substr(start).c_str()));
            }

            activeManeuver = splittingManeuver = new Splitting(this, par("securityDistance").intValue(), vehicleDestination);
        }
        else
            throw new cRuntimeError("Invalid laneChange maneuver implementation chosen");

        scenario = FindModule<BaseScenario*>::findSubModule(getParentModule());
    }
}

void SplittingPlatooningApp::handleSelfMsg(cMessage* msg)
{
    if (splittingManeuver && splittingManeuver->handleSelfMsg(msg)) return;
    BaseApp::handleSelfMsg(msg);
}

void SplittingPlatooningApp::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* frame = check_and_cast<BaseFrame1609_4*>(msg);

    cPacket* enc = frame->getEncapsulatedPacket();
    ASSERT2(enc, "received a BaseFrame1609_4s with nothing inside");

    if (enc->getKind() == MANEUVER_TYPE) {
        ManeuverMessage* mm = check_and_cast<ManeuverMessage*>(frame->decapsulate());
        if (BranchWarning* msg = dynamic_cast<BranchWarning*>(mm)) {
            if (positionHelper->isLeader() and !isInManeuver())
                LOG << "print position\n";
                startSplittingManeuver(msg->getPosX_start(), msg->getPosY_start(), msg->getPosX_finish(), msg->getPosY_finish());
            delete msg;
        }
        else if (UpdatePlatoonData* msg = dynamic_cast<UpdatePlatoonData*>(mm)) {
            handleUpdatePlatoonData(msg);
            delete msg;
        }
        else if (UpdatePlatoonFormation* msg = dynamic_cast<UpdatePlatoonFormation*>(mm)) {
            handleUpdatePlatoonFormation(msg);
            delete msg;
        }
        else {
            onManeuverMessage(mm);
        }
        delete frame;
    }
    else {
        BaseApp::handleLowerMsg(msg);
    }

}

void SplittingPlatooningApp::onPlatoonBeacon(const PlatooningBeacon* pb)
{
    splittingManeuver->onPlatoonBeacon(pb);
    // maintain platoon
    BaseApp::onPlatoonBeacon(pb);
}



void SplittingPlatooningApp::onManeuverMessage(ManeuverMessage* mm)
{
    if (activeManeuver) {
        activeManeuver->onManeuverMessage(mm);
    }
    else {
        splittingManeuver->onManeuverMessage(mm);
    }
    delete mm;
}


void SplittingPlatooningApp::startSplittingManeuver(int posX_start, int posY_start, int posX_finish, int posY_finish)
{
    SplittingParameters params;
    params.posX_start = posX_start;
    params.posY_start = posY_start;
    params.posX_finish = posX_finish;
    params.posY_finish = posY_finish;
    splittingManeuver->startManeuver(&params);
}



void SplittingPlatooningApp::receiveSignal(cComponent* src, simsignal_t id, cObject* value, cObject* details)
{
    if (id == Mac1609_4::sigRetriesExceeded) {
        BaseFrame1609_4* frame = check_and_cast<BaseFrame1609_4*>(value);
        ManeuverMessage* mm = check_and_cast<ManeuverMessage*>(frame->getEncapsulatedPacket());
        if (frame) {
            splittingManeuver->onFailedTransmissionAttempt(mm);
        }
    }
}

void SplittingPlatooningApp::sendTimeoutMsg()
{
    timeoutMsg = new cMessage("TimeoutMsg");
    take(timeoutMsg);
    scheduleAt(simTime() + SimTime(1), timeoutMsg);
}

void SplittingPlatooningApp::sendTimeoutMsgForSplitArea()
{
    timeoutMsg = new cMessage("TimeoutMsg");
    take(timeoutMsg);
    float time_t = (12*static_cast<SplittingManeuverScenario*>(scenario)->getPlatoonSpeed())/110;
    scheduleAt(simTime() + SimTime(time_t), timeoutMsg);
}

int SplittingPlatooningApp::getLeaderSpeed()
{
    return static_cast<SplittingManeuverScenario*>(scenario)->getPlatoonSpeed();
}

int SplittingPlatooningApp::getFollowerSpeed()
{
    return (static_cast<SplittingManeuverScenario*>(scenario)->getPlatoonSpeed() + 20);
}


void SplittingPlatooningApp::sendScheduleMsg(int estimate_time, cMessage* msg) {
    scheduleAt(simTime() + SimTime(estimate_time + 0.5), msg);
}

void SplittingPlatooningApp::resetTimeoutMsg()
{
    cancelAndDelete(timeoutMsg);
}

SplittingPlatooningApp::~SplittingPlatooningApp()
{
    delete splittingManeuver;
}



} // namespace plexe
