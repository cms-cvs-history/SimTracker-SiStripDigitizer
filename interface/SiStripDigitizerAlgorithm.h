#ifndef SiStripDigitizerAlgorithm_h
#define SiStripDigitizerAlgorithm_h

/** \class SiStripDigitizerAlgorithm
 *
 * SiStripDigitizerAlgorithm converts hits to digis
 *
 ************************************************************/

#include <string>

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackerDigiSimLink/interface/StripDigiSimLink.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/SiStripDigi/interface/SiStripDigi.h"
#include "DataFormats/SiStripDigi/interface/SiStripRawDigi.h"
#include "CondFormats/SiStripObjects/interface/SiStripNoises.h"
#include "CondFormats/SiStripObjects/interface/SiStripPedestals.h"
#include "CondFormats/SiStripObjects/interface/SiStripThreshold.h"
#include "CalibFormats/SiStripObjects/interface/SiStripGain.h"
#include "SimTracker/SiStripDigitizer/interface/SiTrivialDigitalConverter.h"
#include "SimTracker/SiStripDigitizer/interface/SiGaussianTailNoiseAdder.h"
#include "SimTracker/SiStripDigitizer/interface/SiHitDigitizer.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripFedZeroSuppression.h"

namespace CLHEP {
  class HepRandomEngine;
}

class SiStripDigitizerAlgorithm {
 public:
  typedef SiDigitalConverter::DigitalVecType DigitalVecType;
  typedef SiDigitalConverter::DigitalRawVecType DigitalRawVecType;
  typedef SiPileUpSignals::HitToDigisMapType HitToDigisMapType;
  typedef SiPileUpSignals::HitCounterToDigisMapType HitCounterToDigisMapType;
  typedef std::map< int, float, std::less<int> > hit_map_type;
  typedef float Amplitude;
  
  // Constructor
  SiStripDigitizerAlgorithm(const edm::ParameterSet& conf, CLHEP::HepRandomEngine&);

  // Destructor
  ~SiStripDigitizerAlgorithm();

  // Runs the algorithm
  void  run(edm::DetSet<SiStripDigi>&, edm::DetSet<SiStripRawDigi>&,
            const std::vector<std::pair<const PSimHit*, int > >  &, 
            StripGeomDetUnit *, GlobalVector, float, 
            edm::ESHandle<SiStripGain> &, edm::ESHandle<SiStripThreshold> &, 
            edm::ESHandle<SiStripNoises> &, edm::ESHandle<SiStripPedestals> &);

  // digisimlink
  std::vector<StripDigiSimLink> make_link() { return link_coll; }

  // ParticleDataTable
  void setParticleDataTable(const ParticleDataTable * pdt) { theSiHitDigitizer->setParticleDataTable(pdt); }
  
 private:
  edm::ParameterSet conf_;
  float theElectronPerADC;
  float theThreshold;          
  bool peakMode;
  bool noise;
  int theFedAlgo;
  bool zeroSuppression;
  float theTOFCutForPeak;
  float theTOFCutForDeconvolution;
  float tofCut;
  int numStrips; 
  int strip;     
  float noiseRMS;
  float pedValue;
  float cosmicShift;
  float inefficiency;
  float theStripInefficiency_[4]; // separate inefficiencies for TIB layers
  float pedOffset;

  size_t firstChannelWithSignal;
  size_t lastChannelWithSignal;
  size_t localFirstChannel;
  size_t localLastChannel;

  // local amplitude of detector channels (from processed PSimHit)
  std::vector<double> locAmpl;
  // total amplitude of detector channels
  std::vector<double> detAmpl;



  SiHitDigitizer* theSiHitDigitizer;
  SiPileUpSignals* theSiPileUpSignals;
  SiGaussianTailNoiseAdder* theSiNoiseAdder;
  SiTrivialDigitalConverter* theSiDigitalConverter;
  SiStripFedZeroSuppression* theSiZeroSuppress;
  CLHEP::HepRandomEngine& rndEngine;

  DigitalVecType digis;
  DigitalRawVecType rawdigis;
  std::vector<StripDigiSimLink> link_coll;
  CLHEP::RandFlat *theFlatDistribution;

  void push_link(const DigitalVecType&,
		 const HitToDigisMapType&,
		 const HitCounterToDigisMapType&,
		 const std::vector<double>&,
		 unsigned int);
  
  void push_link_raw(const DigitalRawVecType&,
		     const HitToDigisMapType&,
		     const HitCounterToDigisMapType&,
		     const std::vector<double>&,
		     unsigned int);
};

#endif
