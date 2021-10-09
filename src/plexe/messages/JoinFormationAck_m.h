//
// Generated file, do not edit! Created by nedtool 5.6 from plexe/messages/JoinFormationAck.msg.
//

#ifndef __JOINFORMATIONACK_M_H
#define __JOINFORMATIONACK_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif

// dll export symbol
#ifndef PLEXE_API
#  if defined(PLEXE_EXPORT)
#    define PLEXE_API  OPP_DLLEXPORT
#  elif defined(PLEXE_IMPORT)
#    define PLEXE_API  OPP_DLLIMPORT
#  else
#    define PLEXE_API
#  endif
#endif



// cplusplus {{
#include "ManeuverMessage_m.h"
// }}

/**
 * Class generated from <tt>plexe/messages/JoinFormationAck.msg:31</tt> by nedtool.
 * <pre>
 * // Response to a JoinPlatoon.
 * // Is sent from the joiner to the leader of the Platoon.
 * // Confirms the joiner joined the Platoon successful at the given position.
 * packet JoinFormationAck extends ManeuverMessage
 * {
 *     double platoonSpeed;
 *     int platoonLane;
 *     int newPlatoonFormation[];
 * }
 * </pre>
 */
class PLEXE_API JoinFormationAck : public ::ManeuverMessage
{
  protected:
    double platoonSpeed;
    int platoonLane;
    int *newPlatoonFormation; // array ptr
    unsigned int newPlatoonFormation_arraysize;

  private:
    void copy(const JoinFormationAck& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const JoinFormationAck&);

  public:
    JoinFormationAck(const char *name=nullptr, short kind=0);
    JoinFormationAck(const JoinFormationAck& other);
    virtual ~JoinFormationAck();
    JoinFormationAck& operator=(const JoinFormationAck& other);
    virtual JoinFormationAck *dup() const override {return new JoinFormationAck(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual double getPlatoonSpeed() const;
    virtual void setPlatoonSpeed(double platoonSpeed);
    virtual int getPlatoonLane() const;
    virtual void setPlatoonLane(int platoonLane);
    virtual void setNewPlatoonFormationArraySize(unsigned int size);
    virtual unsigned int getNewPlatoonFormationArraySize() const;
    virtual int getNewPlatoonFormation(unsigned int k) const;
    virtual void setNewPlatoonFormation(unsigned int k, int newPlatoonFormation);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const JoinFormationAck& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, JoinFormationAck& obj) {obj.parsimUnpack(b);}


#endif // ifndef __JOINFORMATIONACK_M_H
