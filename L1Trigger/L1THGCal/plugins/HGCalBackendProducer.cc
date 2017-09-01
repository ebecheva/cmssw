#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

#include "L1Trigger/L1THGCal/interface/HGCalBackendProcessorBase.h"

#include <sstream>
#include <memory>


class HGCalBackendProducer : public edm::stream::EDProducer<> {  
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

  std::unique_ptr<HGCalBackendProcessorBase> backendProcess_;
};

DEFINE_FWK_MODULE(HGCalBackendProducer);

HGCalBackendProducer::
HGCalBackendProducer(const edm::ParameterSet& conf):
 
input_cell_(consumes<l1t::HGCalTriggerCellBxCollection>(conf.getParameter<edm::InputTag>("bxCollection_be"))),
input_sums_(consumes<l1t::HGCalTriggerSumsBxCollection>(conf.getParameter<edm::InputTag>("bxCollection_be")))
{   
  //setup Backend parameters
  const edm::ParameterSet& beParamConfig = conf.getParameterSet("Backendparam");
  const std::string& beProcessorName = beParamConfig.getParameter<std::string>("BeProcessorName");
  HGCalBackendProcessorBase* beProc = HGCalBackendFactory::get()->create(beProcessorName, beParamConfig);
  backendProcess_.reset(beProc);

  backendProcess_->setProduces(*this);

}

void HGCalBackendProducer::beginRun(const edm::Run& /*run*/, 
                                          const edm::EventSetup& es) {				  
  es.get<IdealGeometryRecord>().get(triggerGeometry_);
}

void HGCalBackendProducer::produce(edm::Event& e, const edm::EventSetup& es) {
  
  edm::Handle<l1t::HGCalTriggerCellBxCollection> trigCellBxColl;
  //edm::Handle<l1t::HGCalTriggerSumsBxCollection> trigSumsBxColl;

  e.getByToken(input_cell_,trigCellBxColl);
  //e.getByToken(input_sums_,trigSumsBxColl);

  const l1t::HGCalTriggerCellBxCollection& trigCell = *trigCellBxColl;
  //const l1t::HGCalTriggerSumsBxCollection& trigSums = *trigSumsBxColl;

  backendProcess_->reset();  
  backendProcess_->run(trigCell,es,e);
  backendProcess_->putInEvent(e);
   
}
