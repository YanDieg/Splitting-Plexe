//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
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

#include "AppRSU.h"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"
#include "plexe/messages/PlexeInterfaceControlInfo_m.h"
#include "plexe/messages/BranchWarning_m.h"

using namespace veins;

namespace plexe {

Define_Module(plexe::AppRSU);

void AppRSU::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    cMessage* startMessage = new cMessage();
    scheduleAt((simTime() + 0.2), startMessage);

    posX_start = 200;
    posY_start = 200;
    posX_finish = 200;
    posY_finish = 200;
}

void AppRSU::handleSelfMsg(cMessage* msg) {


    BranchWarning* warning = new BranchWarning();
    warning->setKind(MANEUVER_TYPE);
    warning->setPosX_start(300);
    warning->setPosY_start(200);
    warning->setPosX_finish(1000);
    warning->setPosY_finish(200);

    BaseFrame1609_4* frame = new BaseFrame1609_4("BaseFrame1609_4", warning->getKind());

    frame->setChannelNumber(static_cast<int>(Channel::cch));
    frame->encapsulate(warning);
    // send unicast frames using 11p only
    PlexeInterfaceControlInfo* ctrl = new PlexeInterfaceControlInfo();
    ctrl->setInterfaces(PlexeRadioInterfaces::VEINS_11P);
    frame->setControlInfo(ctrl);

    sendDown(frame);

    scheduleAt(simTime() + 1, msg);
}

void AppRSU::onWSA(DemoServiceAdvertisment* wsa)
{

}

void AppRSU::onWSM(BaseFrame1609_4* msg)
{


}
}
