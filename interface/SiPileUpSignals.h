#ifndef Tracker_SiPileUpSignals_h
#define Tracker_SiPileUpSignals_h

#include <map>
#include <memory>
#include <vector>
#include "FWCore/Utilities/interface/Map.h"
#include "SimTracker/Common/interface/SimHitInfoForLinks.h"

class PSimHit;
/**
 * Class which takes the responses from each SimHit and piles-up them, within a given module.
 * More precisely, it keeps for each strip the link to each individual measurement.
 */
class SiPileUpSignals{ 
 public:
  // type used to describe the amplitude on a strip
  typedef float Amplitude;
  // associates to each strip a vector of pairs {simhit,amplitude}.
  // That allows later to comput the fraction of the contribution of each simhit to the ADC value
  typedef std::map<int, std::vector<std::pair<std::shared_ptr<SimHitInfoForLinks>, Amplitude> >, std::less<int> >  HitToDigisMapType;
  typedef std::map<uint32_t, HitToDigisMapType> signalMaps;
  
  SiPileUpSignals() { reset(); }

  virtual ~SiPileUpSignals() { }
  
  virtual void add(uint32_t detID,
                   const std::vector<double>& locAmpl,
                   const size_t& firstChannelWithSignal,
                   const size_t& lastChannelWithSignal,
                   const PSimHit* hit);

  void reset(){ resetSignals(); }
  
  const HitToDigisMapType* getSignal(uint32_t detID) const {
    auto where = signal_.find(detID);
    if(where == signal_.end()) {
      return 0;
    }
    return &where->second;
  }
  
 private:
  void resetSignals();
  signalMaps signal_;
};
#endif
