#ifndef _TRACKER_SiHitDigitizer_H_
#define _TRACKER_SiHitDigitizer_H_
 
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimTracker/SiStripDigitizer/interface/SiChargeCollectionDrifter.h"
#include "SimTracker/SiStripDigitizer/interface/SiChargeDivider.h"
#include "SimTracker/SiStripDigitizer/interface/SiInduceChargeOnStrips.h"

#include <map>

class SiStripDetType;
/**
* Digitizes the response for a single SimHit.
*/
class SiHitDigitizer{
 public:

  typedef std::map< int, float, std::less<int> > hit_map_type;
  SiHitDigitizer(const edm::ParameterSet& conf, const StripGeomDetUnit *det);

  ~SiHitDigitizer();

  void setChargeDivider(SiChargeDivider* cd){
    if (theSiChargeDivider) delete theSiChargeDivider;
    theSiChargeDivider = cd;
  }
  void setChargeCollectionDrifter(SiChargeCollectionDrifter* cd){
    if (theSiChargeCollectionDrifter) delete theSiChargeCollectionDrifter;
    theSiChargeCollectionDrifter = cd;
  }
  void setInduceChargeOnStrips(SiInduceChargeOnStrips* cd){
    if (theSiInduceChargeOnStrips) delete theSiInduceChargeOnStrips;
    theSiInduceChargeOnStrips = cd;
  }
  
  hit_map_type processHit(const PSimHit&, const StripGeomDetUnit&, GlobalVector);
  
 private:
  SiChargeDivider* theSiChargeDivider;
  SiChargeCollectionDrifter* theSiChargeCollectionDrifter;
  SiInduceChargeOnStrips* theSiInduceChargeOnStrips;

  edm::ParameterSet conf_;
  double depletionVoltage;
  double appliedVoltage;
  double chargeMobility;
  double temperature;
  double rhall;
  double holeBeta;
  double holeSaturationVelocity;
  bool noDiffusion;
  double chargeDistributionRMS;
  double gevperelectron;
  LocalVector DriftDirection(const StripGeomDetUnit*,GlobalVector);
  typedef GloballyPositioned<double>      Frame;
  float tanLorentzAnglePerTesla;   //Lorentz angle tangent per Tesla

};

#endif
