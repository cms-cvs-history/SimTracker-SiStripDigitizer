#include "SimTracker/SiStripDigitizer/interface/SiPileUpSignals.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

void SiPileUpSignals::resetSignals(){
  signal_.clear();
}

void SiPileUpSignals::add(uint32_t detID, const std::vector<double>& locAmpl,
                          const size_t& firstChannelWithSignal,
                          const size_t& lastChannelWithSignal,
                          const PSimHit* hit) {
  HitToDigisMapType& theMapLink = signal_[detID];
  for(size_t iChannel = firstChannelWithSignal; iChannel<lastChannelWithSignal; ++iChannel) {
    theMapLink[iChannel].emplace_back(std::shared_ptr<SimHitInfoForLinks>(new SimHitInfoForLinks(hit)), Amplitude(locAmpl[iChannel]));
  }
}
