#include "SimTracker/SiStripDigitizer/interface/SiTrivialZeroSuppress.h"
//#include "Utilities/Notification/interface/TimingReport.h" // ???

//Author: Matthew Pearson, March 2003, ported to CMSSW by Andrea Giammanco, November 2005

//Constructor
SiTrivialZeroSuppress::SiTrivialZeroSuppress(const edm::ParameterSet const& conf, float noise) : conf_(conf), theNumFEDalgos(4)
{
  noiseInAdc=noise;
  initParams(conf_);
}

//Read parameters needed for the zero suppression algorithm 
//(which is in SiTrivialZeroSuppress::trkFEDclusterizer)
void SiTrivialZeroSuppress::initParams(edm::ParameterSet const& conf_)
{
  //Read parameters
  //  static SimpleConfigurable<short> algoConf(4,"SiStripFedDigitizer:algorithm");
  //  static SimpleConfigurable<float> lowthreshConf(2.,"SiStripFedDigitizer:lowThresh");
  //  static SimpleConfigurable<float> highthreshConf(5.,"SiStripFedDigitizer:highThresh");
  algoConf=conf_.getParameter<int>("FedAlgorithm");
  lowthreshConf=conf_.getParameter<double>("FedLowThreshold");
  highthreshConf=conf_.getParameter<double>("FedHighThreshold");

  theFEDalgorithm  = algoConf;
  theFEDlowThresh  = lowthreshConf * noiseInAdc;
  theFEDhighThresh = highthreshConf * noiseInAdc;

  //Check zero suppress algorithm
  if (theFEDalgorithm < 1 || theFEDalgorithm > theNumFEDalgos) {
    cout<<"SiTrivialZeroSuppress FATAL ERROR: Unknown zero suppress algorithm "<<theFEDalgorithm<<endl;
    exit(1);
  }
  
  //Check thresholds
  if (theFEDlowThresh > theFEDhighThresh) {
    cout<<"SiTrivialZeroSuppress FATAL ERROR: Low threshold exceeds high threshold: "<<theFEDlowThresh<<" > "<<theFEDhighThresh<<endl;
    exit(2);
  }
}

//Zero suppress method, which called the SiTrivialZeroSuppress::trkFEDclusterizer
SiZeroSuppress::DigitalMapType SiTrivialZeroSuppress::zeroSuppress(const DigitalMapType& notZeroSuppressedMap)
{
  //  const string s1("SiTrivialZeroSuppress::zeroSuppress");
  //  TimeMe time_me(s1);
  
  return trkFEDclusterizer(notZeroSuppressedMap); 
}

//This performs the zero suppression
SiZeroSuppress::DigitalMapType SiTrivialZeroSuppress::trkFEDclusterizer(const DigitalMapType &in) 
{
  const string s2("SiTrivialZeroSuppress::trkFEDclusterizer1");
  //  TimeMe time_me(s2); // ???
  
  DigitalMapType selectedSignal;
  register DigitalMapType::const_iterator i, iPrev, iNext, iPrev2, iNext2;
  
  for (i = in.begin(); i != in.end(); i++) {
  
    //Find adc values for neighbouring strips
    int strip = i->first;
    int adc   = i->second;
    iPrev  = in.find(strip - 1);
    iNext  = in.find(strip + 1);
    //Set values for strips just outside APV or module to infinity.
    //This is to avoid losing strips at the edges, 
    //which otherwise would pass cuts if strips were next to each other.
    int adcPrev = -99999;
    int adcNext = -99999;
    if ( ((strip)%128) == 127) adcNext = 99999;
    if ( ((strip)%128) == 0)   adcPrev = 99999;
    //Otherwise if strip was found then find it's ADC count.
    if ( iPrev  != in.end() ) adcPrev  = iPrev->second;
    if ( iNext  != in.end() ) adcNext  = iNext->second;
    int adcMaxNeigh = max(adcPrev, adcNext);
     
    //Find adc values for next neighbouring strips
    iPrev2 = in.find(strip - 2); 
    iNext2 = in.find(strip + 2);
    //See above 
    int adcPrev2 = -99999;
    int adcNext2 = -99999;
    if ( ((strip)%128) == 126) adcNext2 = 99999;
    if ( ((strip)%128) == 1)   adcPrev2 = 99999; 
    if ( iPrev2 != in.end() ) adcPrev2 = iPrev2->second; 
    if ( iNext2 != in.end() ) adcNext2 = iNext2->second; 
 
    //    cout << " strip " << strip << " adc " << adc << " adcPrev " << adcPrev
    //   << " adcNext " << adcNext << " adcMaxNeigh " << adcMaxNeigh << endl;
    //cout << "  theFEDlowThresh " <<  theFEDlowThresh << " theFEDhighThresh " << theFEDhighThresh << endl;
 
    // Decide if this strip should be accepted.
    bool accept = false;
    switch (theFEDalgorithm) {
 
    case 1: 
      accept = (adc >= theFEDlowThresh);
      break;

    case 2:
      accept = (adc >= theFEDhighThresh || (adc >= theFEDlowThresh && 
					    adcMaxNeigh >= theFEDlowThresh));
      break;
      
    case 3:
      accept = (adc >= theFEDhighThresh || (adc >= theFEDlowThresh && 
					    adcMaxNeigh >= theFEDhighThresh));
      break;

    case 4:
      accept = ((adc >= theFEDhighThresh) ||           //Test for adc>highThresh (same as algorithm 2)
                ((adc >= theFEDlowThresh) &&           //Test for adc>lowThresh, with neighbour adc>lowThresh (same as algorithm 2)
		 (adcMaxNeigh >= theFEDlowThresh)) || 
		((adc < theFEDlowThresh) &&            //Test for adc<lowThresh
		 (((adcPrev  >= theFEDhighThresh) &&   //with both neighbours>highThresh
		   (adcNext  >= theFEDhighThresh)) ||    
		  ((adcPrev  >= theFEDhighThresh) &&   //OR with previous neighbour>highThresh and
		   (adcNext  >= theFEDlowThresh)  &&   //both the next neighbours>lowThresh
		   (adcNext2 >= theFEDlowThresh))  ||    
		  ((adcNext  >= theFEDhighThresh) &&   //OR with next neighbour>highThresh and
		   (adcPrev  >= theFEDlowThresh)  &&   //both the previous neighbours>lowThresh
		   (adcPrev2 >= theFEDlowThresh))  ||
		  ((adcNext  >= theFEDlowThresh)  &&   //OR with both next neighbours>lowThresh and
		   (adcNext2 >= theFEDlowThresh)  &&   //both the previous neighbours>lowThresh
		   (adcPrev  >= theFEDlowThresh)  && 
		   (adcPrev2 >= theFEDlowThresh)))));
      break;
    }

    if (accept) {   
      selectedSignal[strip] = adc;
      //    cout << " selected strip: " << strip << "  adc: " << adc << endl;
    }  
  }
  return selectedSignal;
}