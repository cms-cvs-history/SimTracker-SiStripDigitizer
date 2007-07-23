#ifndef SiStripDigitizer_h
#define SiStripDigitizer_h

/** \class SiStripDigitizer
 *
 *
 * \author Andrea Giammanco
 *

 *
 ************************************************************/
#include "boost/shared_ptr.hpp"

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/Common/interface/EDProduct.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/CommonDetUnit/interface/GeomDetType.h"

#include "SimDataFormats/CrossingFrame/interface/MixCollection.h"
#include "SimTracker/SiStripDigitizer/interface/SiStripDigitizerAlgorithm.h"

//SiStripPedestalsService
#include "CommonTools/SiStripZeroSuppression/interface/SiStripNoiseService.h"
//Active module selection
#include "SimTracker/Common/interface/SimHitSelectorFromDB.h"
#include "CalibFormats/SiStripObjects/interface/SiStripDetCabling.h"

#include <string>
#include <vector>

namespace cms {
  class SiStripDigitizer : public edm::EDProducer
  {
  public:
  
    // The following is not yet used, but will be the primary
    // constructor when the parameter set system is available.
    //
    explicit SiStripDigitizer(const edm::ParameterSet& conf);
  
    virtual ~SiStripDigitizer();
  
    virtual void produce(edm::Event& e, const edm::EventSetup& c);
  
  private:
    std::map<GeomDetType* , boost::shared_ptr<SiStripDigitizerAlgorithm> > theAlgoMap; 
    std::vector<edm::DetSet<SiStripDigi> > theDigiVector;
    std::vector<edm::DetSet<StripDigiSimLink> > theDigiLinkVector;
  
    edm::ParameterSet conf_;
    SiStripNoiseService SiStripNoiseService_;  
    std::map<uint32_t, std::vector<int> > theDetIdList;
    SimHitSelectorFromDB SimHitSelectorFromDB_;
    typedef std::vector<std::string> vstring;
    vstring trackerContainers;
    bool useConfFromDB;
    typedef std::map<unsigned int, std::vector<PSimHit>,std::less<unsigned int> > simhit_map;
    typedef simhit_map::iterator simhit_map_iterator;
    simhit_map SimHitMap;
    int numStrips;    // number of strips in the module
    edm::ESHandle<SiStripDetCabling> SiStripDetCabling_;
  };
}
#endif
