#include "SimTracker/SiStripDigitizer/interface/SiTrivialInduceChargeOnStrips.h"

#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetType.h"
#include <Math/ProbFuncMathCore.h>
#include "DataFormats/SiStripDetId/interface/TECDetId.h"
#include "DataFormats/SiStripDetId/interface/TIBDetId.h"
#include "DataFormats/SiStripDetId/interface/TIDDetId.h"
#include "DataFormats/SiStripDetId/interface/TOBDetId.h"

#include <algorithm>
#include <iostream>

const int
SiTrivialInduceChargeOnStrips::
Ntypes = 14;

const  std::string 
SiTrivialInduceChargeOnStrips::
type[Ntypes] = { "IB1", "IB2","OB1","OB2","W1a","W2a","W3a","W1b","W2b","W3b","W4","W5","W6","W7"};

inline uint16_t 
SiTrivialInduceChargeOnStrips::
indexOf(const std::string& t) { return std::find( type, type + Ntypes, t) - type;}

inline uint16_t
SiTrivialInduceChargeOnStrips::
typeOf(const StripGeomDetUnit& det) {
  DetId id = det.geographicalId();
  switch (det.specificType().subDetector()) {
  case GeomDetEnumerators::TIB: {return (TIBDetId(id).layer() < 3) ? indexOf("IB1") : indexOf("IB2");}
  case GeomDetEnumerators::TOB: {return (TOBDetId(id).layer() > 4) ? indexOf("OB1") : indexOf("OB2");}
  case GeomDetEnumerators::TID: {return indexOf("W1a") -1 + TIDDetId(id).ring();} //fragile: relies on ordering of 'type'
  case GeomDetEnumerators::TEC: {return indexOf("W1b") -1 + TECDetId(id).ring();} //fragile: relies on ordering of 'type'
  default: throw cms::Exception("Invalid subdetector") << id();
  }
}

SiTrivialInduceChargeOnStrips::
SiTrivialInduceChargeOnStrips(const edm::ParameterSet& conf,double g) 
  : Nsigma(3.), geVperElectron(g)  {
  std::string mode = conf.getParameter<bool>("APVpeakmode") ? "Peak" : "Dec";
  for(int i=0; i<Ntypes; i++)
    signalCoupling.push_back(conf.getParameter<std::vector<double> >("CouplingConstant"+mode+type[i]));
}

void 
SiTrivialInduceChargeOnStrips::
induce(SiChargeCollectionDrifter::collection_type collection_points, 
       const StripGeomDetUnit& det, 
       std::vector<double>& localAmplitudes, 
       unsigned int& recordMinAffectedStrip, 
       unsigned int& recordMaxAffectedStrip) {

  std::vector<double>& coupling = signalCoupling.at(typeOf(det));
  const StripTopology& topology = dynamic_cast<const StripTopology&>(det.specificTopology());
  unsigned Nstrips =  topology.nstrips();

  for (SiChargeCollectionDrifter::collection_type::const_iterator 
	 signalpoint = collection_points.begin();  signalpoint != collection_points.end();  signalpoint++ ) {
    
    //In strip coordinates:
    double chargePosition = topology.strip(signalpoint->position());
    double chargeSpread = signalpoint->sigma() / topology.localPitch(signalpoint->position());
    
    uint16_t fromStrip = std::max( 0,         int(   std::floor( chargePosition - Nsigma*chargeSpread) ));
    uint16_t untilStrip = std::min( Nstrips, unsigned(std::ceil( chargePosition + Nsigma*chargeSpread) ));
    for (uint16_t strip = fromStrip;  strip < untilStrip; strip++) {

      double chargeDepositedOnStrip = chargeDeposited( strip, Nstrips, signalpoint->amplitude(), chargeSpread, chargePosition);

      uint16_t affectedFromStrip = std::max( unsigned(0),  strip - coupling.size() + 1 );
      uint16_t affectedUntilStrip = std::min( Nstrips,  strip + coupling.size() );  
      for (uint16_t affectedStrip = affectedFromStrip;  affectedStrip < affectedUntilStrip;  affectedStrip++) {
	localAmplitudes.at( affectedStrip ) += chargeDepositedOnStrip * coupling.at(abs( affectedStrip - strip )) ;
      }

      if(  affectedFromStrip < recordMinAffectedStrip  ) recordMinAffectedStrip = affectedFromStrip;
      if( affectedUntilStrip > recordMaxAffectedStrip+1) recordMaxAffectedStrip = affectedUntilStrip-1;
    }
  }
  return;
}

inline double
SiTrivialInduceChargeOnStrips::
chargeDeposited(uint16_t strip, uint16_t Nstrips, double amplitude, double chargeSpread, double chargePosition) const {
  double integralUpToStrip = (strip == 0)         ? 0. : ( ROOT::Math::normal_cdf(   strip, chargeSpread, chargePosition) );
  double integralUpToNext  = (strip+1 == Nstrips) ? 1. : ( ROOT::Math::normal_cdf( strip+1, chargeSpread, chargePosition) );
  double percentOfSignal = integralUpToNext - integralUpToStrip;
  
  return percentOfSignal * amplitude / geVperElectron;
}
