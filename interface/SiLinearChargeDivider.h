#ifndef Tracker_SiLinearChargeDivider_H
#define Tracker_SiLinearChargeDivider_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimTracker/SiStripDigitizer/interface/SiChargeDivider.h"
#include "SimTracker/Common/interface/SiG4UniversalFluctuation.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

/**
 * Concrete implementation of SiChargeDivider. 
 * It divides the charge on the line connecting entry and exit point of the SimTrack in the Silicon.
 */
class SiLinearChargeDivider : public SiChargeDivider{
 public:

  SiLinearChargeDivider(){
  }
  SiLinearChargeDivider(const edm::ParameterSet& conf);

  virtual ~SiLinearChargeDivider(){
  }

  SiChargeDivider::ionization_type divide(const PSimHit&, const StripGeomDetUnit& det);

  void setParticleDataTable(const ParticleDataTable * pdt);
  
 private:
  edm::ParameterSet conf_;
  
  float PeakShape(const PSimHit&, const StripGeomDetUnit& det);
  float DeconvolutionShape( const PSimHit&, const StripGeomDetUnit& det);
  float TimeResponse( const PSimHit&, const StripGeomDetUnit& det); 
  void fluctuateEloss(int particleId, float momentum, float eloss, float length, int NumberOfSegmentation, float elossVector[]);   
  bool peakMode;
  bool fluctuateCharge;
  double timeResPeak, timeResDeco;
  int  chargedivisionsPerStrip;
  double deltaCut ;
  double cosmicShift;
  SiG4UniversalFluctuation fluctuate; 
  const ParticleDataTable * theParticleDataTable;

};

#endif
