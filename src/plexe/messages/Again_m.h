//
// Generated file, do not edit! Created by nedtool 5.6 from plexe/messages/Again.msg.
//

#ifndef __AGAIN_M_H
#define __AGAIN_M_H

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
 * Class generated from <tt>plexe/messages/Again.msg:30</tt> by nedtool.
 * <pre>
 * // Request to execute again the meneuver.
 * // Is sent from a member to platoon leader.
 * packet Again extends ManeuverMessage
 * {
 * }
 * </pre>
 */
class PLEXE_API Again : public ::ManeuverMessage
{
  protected:

  private:
    void copy(const Again& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Again&);

  public:
    Again(const char *name=nullptr, short kind=0);
    Again(const Again& other);
    virtual ~Again();
    Again& operator=(const Again& other);
    virtual Again *dup() const override {return new Again(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Again& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Again& obj) {obj.parsimUnpack(b);}


#endif // ifndef __AGAIN_M_H

