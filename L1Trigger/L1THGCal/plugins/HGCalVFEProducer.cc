#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

#include "L1Trigger/L1THGCal/interface/HGCalVFEProcessorBase.h"

#include <sstream>
#include <memory>


class HGCalVFEProducer : public edm::stream::EDProducer<>  {  
 public:    
  HGCalVFEProducer(const edm::ParameterSet&);
  ~HGCalVFEProducer() { }
  
  virtual void beginRun(const edm::Run&, 
                        const edm::EventSetup&);
  virtual void produce(edm::Event&, const edm::EventSetup&);
  
 private:
  // inputs
  edm::EDGetToken inputee_, inputfh_;
  edm::ESHandle<HGCalTriggerGeometryBase> triggerGeometry_;
  
  std::unique_ptr<HGCalVFEProcessorBase> vfeProcess_;
};

DEFINE_FWK_MODULE(HGCalVFEProducer);

HGCalVFEProducer::
HGCalVFEProducer(const edm::ParameterSet& conf):
  inputee_(consumes<HGCEEDigiCollection>(conf.getParameter<edm::InputTag>("eeDigis"))),
  inputfh_(consumes<HGCHEDigiCollection>(conf.getParameter<edm::InputTag>("fhDigis"))) 
{   
  //setup VFE parameters
  const edm::ParameterSet& vfeParamConfig = conf.getParameterSet("VFEparam");
  const std::string& vfeProcessorName = vfeParamConfig.getParameter<std::string>("VFEProcessorName");
  HGCalVFEProcessorBase* vfeProc = HGCalVFEProcessorBaseFactory::get()->create(vfeProcessorName, vfeParamConfig);
  vfeProcess_.reset(vfeProc);
    
  vfeProcess_->setProduces(*this);
}

void HGCalVFEProducer::beginRun(const edm::Run& /*run*/, 
                                          const edm::EventSetup& es) {					  				  
  es.get<IdealGeometryRecord>().get(triggerGeometry_);
  vfeProcess_->setGeometry(triggerGeometry_.product());
}

void HGCalVFEProducer::produce(edm::Event& e, const edm::EventSetup& es) {
    
  edm::Handle<HGCEEDigiCollection> ee_digis_h;
  edm::Handle<HGCHEDigiCollection> fh_digis_h, bh_digis_h;

  e.getByToken(inputee_,ee_digis_h);
  e.getByToken(inputfh_,fh_digis_h);

  const HGCEEDigiCollection& ee_digis = *ee_digis_h;
  const HGCHEDigiCollection& fh_digis = *fh_digis_h;
  //const HGCHEDigiCollection& bh_digis = *bh_digis_h;
  
  // First find modules containing hits and prepare list of hits for each module
  std::unordered_map<uint32_t, HGCEEDigiCollection> hit_modules_ee;
  for(const auto& eedata : ee_digis) {    
    uint32_t module = triggerGeometry_->getModuleFromCell(eedata.id());
    auto itr_insert = hit_modules_ee.emplace(module,HGCEEDigiCollection());
    itr_insert.first->second.push_back(eedata);
  }
    
  std::unordered_map<uint32_t,HGCHEDigiCollection> hit_modules_fh;
  for(const auto& fhdata : fh_digis) {
    uint32_t module = triggerGeometry_->getModuleFromCell(fhdata.id());
    auto itr_insert = hit_modules_fh.emplace(module, HGCHEDigiCollection());
    itr_insert.first->second.push_back(fhdata);
  }

  for( const auto& module_hits : hit_modules_ee ) {
    vfeProcess_->reset();
    vfeProcess_->vfeProcessing(module_hits.second, HGCHEDigiCollection(), HGCHEDigiCollection());  
  } //end loop on EE modules
      
  for( const auto& module_hits : hit_modules_fh ) {    
    vfeProcess_->reset();
    vfeProcess_->vfeProcessing(module_hits.second, HGCHEDigiCollection(), HGCHEDigiCollection());
  } //end loop on FH modules
 
  vfeProcess_->putInEvent(e);
   
}
