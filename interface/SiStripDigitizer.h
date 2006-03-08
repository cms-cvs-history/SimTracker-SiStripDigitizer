#ifndef SiStripDigitizer_h
#define SiStripDigitizer_h

/** \class SiStripDigitizer
 *
 *
 * \author Andrea Giammanco
 *

 *
 ************************************************************/

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "FWCore/EDProduct/interface/EDProduct.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimTracker/SiStripDigitizer/interface/SiStripDigitizerAlgorithm.h"
#include "DataFormats/SiStripDigi/interface/StripDigi.h"
#include "DataFormats/SiStripDigi/interface/StripDigiCollection.h"

namespace cms
{
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
    SiStripDigitizerAlgorithm stripDigitizer_;
    edm::ParameterSet conf_;
    std::vector<PSimHit> theStripHits;
    typedef std::map<unsigned int, std::vector<PSimHit>,std::less<unsigned int> > simhit_map;
    typedef simhit_map::iterator simhit_map_iterator;
    simhit_map SimHitMap;
    std::vector<StripDigi> collector;
    int numStrips;    // number of strips in the module
  };
}


#endif
