#include "SimTracker/SiStripDigitizer/interface/SiPileUpSignals.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

void SiPileUpSignals::resetSignals(){
  signal_.clear();
}

void SiPileUpSignals::add(uint32_t detID, const std::vector<double>& locAmpl,
                          const size_t& firstChannelWithSignal,
                          const size_t& lastChannelWithSignal,
                          unsigned int trackID) {
  SignalMapType& theSignal = signal_[detID];
  for (size_t iChannel=firstChannelWithSignal; iChannel<lastChannelWithSignal; ++iChannel) {
    if(locAmpl[iChannel] != 0.0) {
      ChannelMapType& theChannel = theSignal[iChannel];
      theChannel[trackID] += locAmpl[iChannel];
    }
  }
}
