import FWCore.ParameterSet.Config as cms

from L1Trigger.L1THGCal.hgcalTriggerGeometryESProducer_cfi import *
from L1Trigger.L1THGCal.hgcalBackEndProducer_cfi import *


hgcalBE = cms.Sequence(hgcalBackEndProducer)

