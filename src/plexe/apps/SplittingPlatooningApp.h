
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

#ifndef SPLITTINGPLATOONINGAPP_H_
#define SPLITTINGPLATOONINGAPP_H_

#include <algorithm>
#include <memory>

#include "plexe/apps/BaseApp.h"
#include "plexe/apps/GeneralPlatooningApp.h"
#include "plexe/maneuver/Maneuver.h"
#include "plexe/maneuver/Splitting.h"


#include "plexe/messages/ManeuverMessage_m.h"
#include "plexe/messages/UpdatePlatoonData_m.h"



#include "plexe/scenarios/BaseScenario.h"


#include "veins/modules/mobility/traci/TraCIConstants.h"
#include "veins/modules/utility/SignalManager.h"

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "plexe/messages/BranchWarning_m.h" //CHANGE
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

namespace plexe {

class SplittingPlatooningApp : public GeneralPlatooningApp {

public:
    /** c'tor for LaneChangePlatooningApp */
    SplittingPlatooningApp()
        : GeneralPlatooningApp()
        , splittingManeuver(nullptr)

    {
    }

    /** d'tor for LaneChangePlatooningApp */
    virtual ~SplittingPlatooningApp();

    virtual void startSplittingManeuver(int posX_start, int posY_start, int posX_finish, int posY_finish);

    /** override from GeneralPlatooningApp */
    virtual void initialize(int stage) override;

    /** override from GeneralPlatooningApp */
    virtual void handleSelfMsg(cMessage* msg) override;

    virtual void handleLowerMsg(cMessage* msg) override;

    void sendScheduleMsg(int estimate_time, cMessage* msg);

    void sendTimeoutMsgForSplitArea();

    void sendTimeoutMsg();

    void resetTimeoutMsg();

    int getLeaderSpeed();

    int getFollowerSpeed();

protected:
    /** used to receive the "retries exceeded" signal **/
    virtual void receiveSignal(cComponent* src, simsignal_t id, cObject* value, cObject* details) override;

    /**
     * Handles PlatoonBeacons
     *
     * @param PlatooningBeacon pb to handle
     */
    virtual void onPlatoonBeacon(const PlatooningBeacon* pb) override;

    /**
     * Handles ManeuverMessages
     *
     * @param ManeuverMessage mm to handle
     */
    virtual void onManeuverMessage(ManeuverMessage* mm) override;



private:
    /** platoons change lane implementation */
    Maneuver* splittingManeuver;

    // message used to schedule timeouts
    cMessage* timeoutMsg;
};

} // namespace plexe

#endif
