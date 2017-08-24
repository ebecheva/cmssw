#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"


#include <sstream>
#include <memory>


class HGCalBackendProducer : public edm::EDProducer  {  
 public:    
  HGCalBackendProducer(const edm::ParameterSet&);
  ~HGCalBackendProducer() { }
  
  virtual void beginRun(const edm::Run&, 
                        const edm::EventSetup&);
  virtual void produce(edm::Event&, const edm::EventSetup&);
  
 private:
  // inputs
  edm::EDGetToken input_cell_, input_sums_;
  edm::ESHandle<HGCalTriggerGeometryBase> triggerGeometry_;
  
};

DEFINE_FWK_MODULE(HGCalBackendProducer);

HGCalBackendProducer::
HGCalBackendProducer(const edm::ParameterSet& conf):
  input_cell_(consumes<l1t::HGCalTriggerCellBxCollection>(conf.getParameter<edm::InputTag>("bxCollection_be"))),
  input_sums_(consumes<l1t::HGCalTriggerSumsBxCollection>(conf.getParameter<edm::InputTag>("bxCollection_be")))
{  
  
  produces<l1t::HGCalTriggerCellBxCollection>("bxCollection");
  produces<l1t::HGCalTriggerSumsBxCollection>("bxCollection");
  
}

void HGCalBackendProducer::beginRun(const edm::Run& /*run*/, 
                                          const edm::EventSetup& es) {
					  					  
  es.get<IdealGeometryRecord>().get(triggerGeometry_);
}

void HGCalBackendProducer::produce(edm::Event& e, const edm::EventSetup& es) {
  
  std::unique_ptr<l1t::HGCalTriggerCellBxCollection> 
    		fe_output_trig_cell( new l1t::HGCalTriggerCellBxCollection );
 
  std::unique_ptr<l1t::HGCalTriggerSumsBxCollection> 
    		fe_output_trig_sums( new l1t::HGCalTriggerSumsBxCollection );
  
  
 
  e.put(std::move(fe_output_trig_cell), "bxCollection");
  e.put(std::move(fe_output_trig_sums), "bxCollection");
   
}
