//
// Generated file, do not edit! Created by nedtool 5.6 from plexe/messages/BranchWarning.msg.
//

#ifndef __BRANCHWARNING_M_H
#define __BRANCHWARNING_M_H

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
 * Class generated from <tt>plexe/messages/BranchWarning.msg:31</tt> by nedtool.
 * <pre>
 * // Message from all vehicles in the Platoon to the leader to acknoledge the updated formation.
 * // Is similar to a PlatoonBeacon.
 * packet BranchWarning extends ManeuverMessage
 * {
 *     int posX_start;
 *     int posY_start;
 * 
 *     int posX_finish;
 *     int posY_finish;
 * }
 * </pre>
 */
class PLEXE_API BranchWarning : public ::ManeuverMessage
{
  protected:
    int posX_start;
    int posY_start;
    int posX_finish;
    int posY_finish;

  private:
    void copy(const BranchWarning& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const BranchWarning&);

  public:
    BranchWarning(const char *name=nullptr, short kind=0);
    BranchWarning(const BranchWarning& other);
    virtual ~BranchWarning();
    BranchWarning& operator=(const BranchWarning& other);
    virtual BranchWarning *dup() const override {return new BranchWarning(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getPosX_start() const;
    virtual void setPosX_start(int posX_start);
    virtual int getPosY_start() const;
    virtual void setPosY_start(int posY_start);
    virtual int getPosX_finish() const;
    virtual void setPosX_finish(int posX_finish);
    virtual int getPosY_finish() const;
    virtual void setPosY_finish(int posY_finish);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const BranchWarning& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, BranchWarning& obj) {obj.parsimUnpack(b);}


#endif // ifndef __BRANCHWARNING_M_H

