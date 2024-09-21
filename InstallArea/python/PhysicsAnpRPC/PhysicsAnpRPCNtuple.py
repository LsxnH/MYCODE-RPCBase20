import os
import sys
import re

import PhysicsAnpBase .PhysicsAnpBaseConfig as physicsBase

from PhysicsAnpBase.PhysicsAnpBaseConfig import CutItem
from PhysicsAnpBase.PhysicsAnpBaseConfig import addCuts
from PhysicsAnpBase.PhysicsAnpBaseConfig import getAlgName

clog = physicsBase.getLog(os.path.basename(__file__))

#========================================================================================================
def getWriteNtupleAlgForRPC(name, options, cand_name, tree_name, do_bme=False):

    event_keys = [        
        'Run                      :type=Int',
        'LumiBlock                :type=Int',
        'Event                    :type=ULong',
        'bcid                     :type=Int',
        'NRecoVtx                 :type=Int',
        'ActualInteractions       :type=Float',
        'AverageInteractions      :type=Float',
        'Match2ndReadoutEvent     :type=Short',
        ]

    muon_keys = ['Pt',
                 'Eta',
                 'Phi',
                 'D0Sig',
                 'Z0Sin',
                 'TriggerSector:       :type=Short',
                 'PDG:                 :type=Short',
                 'Loose:               :type=Short',
                 'Medium:              :type=Short',
                 'Tight:               :type=Short',
                 'NGasGapInBounds      :type=Short',
                 'NGasGapInBoundsTight :type=Short',
                 'NGasGapInBoundsGeoCut:type=Short',
                 'NGasGapInBoundsSelect:type=Short',
                 ]

    event_keys += muon_keys

    geo_keys = ['gapId_identifier          :type=ULong',
                'gapId_panelIdEta32        :type=ULong',
                'gapId_panelIdPhi32        :type=ULong',
                'gapId_stationName         :type=Int',
                'gapId_stationEta          :type=Int',
                'gapId_stationPhi          :type=Int',
                'gapId_doubletR            :type=Int',
                'gapId_doubletZ            :type=Int',
                'gapId_doubletPhi          :type=Int',
                'gapId_gasGap              :type=Int',
                ]

    gap_keys = ['localTrackPosY            :type=Float',
                'localTrackPosZ            :type=Float',
                'localTrackPosValid        :type=Short',
                'localTrackPosInBoundsTight:type=Short',
                'ClosestHitResidualEta     :type=Float',
                'ClosestHitResidualPhi     :type=Float',
                'ClosestGeoStripEtaNumber  :type=Int',
                'ClosestGeoStripEtaPosZ    :type=Float',
                'ClosestGeoStripPhiNumber  :type=Int',
                'ClosestGeoStripPhiPosY    :type=Float',
                'ClosestGeoStripResidualEta:type=Float',
                'ClosestGeoStripResidualPhi:type=Float',
                ]

    rpc_hit_keys = ['stripNumber     :type=Short',  
                    'prdTime         :type=Float',]

    if True:
        rpc_hit_keys += ['gapId_identifier:type=ULong']
    else:
        rpc_hit_keys += geo_keys

    rpc_eta_keys = rpc_hit_keys + ['StripPosZ:type=Float']
    rpc_phi_keys = rpc_hit_keys + ['StripPosY:type=Float']

    bme_keys = ['detID      :type=Short',
                'isMiddle   :type=Short',
                'isPaired   :type=Short',
                'rpcTDC     :type=Short',
                'CH         :type=Short',
                'Side       :type=Short',
                'Coarse     :type=Int',
                'Fine       :type=Int',
                'Ch_Mask    :type=Short',
                'Mode       :type=Short',
	            'Width      :type=Float',
                ]

    if options.do_clu_ntuple:
        event_keys = [        
                      'Run                      :type=Int',
                      'LumiBlock                :type=Int',
                      'ActualInteractions       :type=Float',
                      'AverageInteractions      :type=Float',
                      ]

        muon_keys = ['Pt',
                     'Eta',
                     'Phi',
                     'PDG                 :type=Short',
                     'MatchMU20           :type=Short',
                    ]

        event_keys += muon_keys 

        gap_keys   = [
                      'NRpcSignalHitEta          :type=Short',
                      'NRpcSignalHitPhi          :type=Short',
                      'NRpcClusterEta            :type=Short',
                      'NRpcClusterPhi            :type=Short',
                      'ClosestClusterSizeEta     :type=Short',
                      'ClosestClusterSizePhi     :type=Short',
                      ]

        geo_keys = ['gapId_identifier          :type=ULong',
                    'gapId_stationName         :type=Int',
                    'gapId_stationEta          :type=Int',
                    'gapId_stationPhi          :type=Int',
                    'gapId_doubletR            :type=Int',
                    'gapId_doubletZ            :type=Int',
                    'gapId_doubletPhi          :type=Int',
                    'gapId_gasGap              :type=Int',
                    'stripWidthEta',
                    'stripWidthPhi',
                    'stripLengthEta',
                    'stripLengthPhi',
                    ]

        rpc_eta_keys = []
        rpc_phi_keys = []
        bme_keys     = []

    alg = physicsBase.AlgConfig(name, 'WriteNtuple')    
    alg.SetKey('CandName',       cand_name)
    alg.SetKey('TreeName',       tree_name)
    alg.SetKey('Debug',          options.debug_alg == 'WriteNtuple')
    alg.SetKey('UseTopLevelDir', 'yes')
    alg.SetKey('Print',          'yes')
    alg.SetKey('PrintCuts',      'no')
    alg.SetKey('WriteReco',      'no')
    alg.SetKey('WriteCandReco',  'no')
    alg.SetKey('WriteCand',      'yes')

    branch_list = 'gap_, rpc_hit_eta_, rpc_hit_phi_'

    if options.do_bme:
        branch_list += ', bme_hit_'

    alg.SetKey('BranchObjKeys', branch_list)
    alg.SetKey('KeyEvent',      str(',').join(event_keys  ))
    alg.SetKey('gap_',          str(',').join(gap_keys + geo_keys))
    alg.SetKey('rpc_hit_eta_',  str(',').join(rpc_eta_keys))
    alg.SetKey('rpc_hit_phi_',  str(',').join(rpc_phi_keys))
    alg.SetKey('bme_hit_',      str(',').join(bme_keys    ))

    return alg

#========================================================================================================
def getWriteNtupleAlgForTrigger(name, options, cand_name, tree_name):

    event_keys = [        
        'Run                      :type=Int',
        'LumiBlock                :type=Int',
        'Event                    :type=ULong',
        'bcid                     :type=Int',
        'NRecoVtx                 :type=Int',
        'ActualInteractions       :type=Float',
        'AverageInteractions      :type=Float',
        ]

    muon_keys = ['Pt',
                 'Eta',
                 'Phi',
                 'D0Sig',
                 'Z0Sin',
                 'TriggerSector:       :type=Short',
                 'PDG:                 :type=Short',
                 'Loose:               :type=Short',
                 'Medium:              :type=Short',
                 'Tight:               :type=Short',
                 'NGasGapInBounds      :type=Short',
                 'NGasGapInBoundsTight :type=Short',
                 'NGasGapInBoundsGeoCut:type=Short',
                 'NGasGapInBoundsSelect:type=Short',
                 'NL1RoIMatch          :type=Short',
                 ]

    event_keys += muon_keys

    geo_keys = ['gapId_identifier          :type=ULong',
                'gapId_stationName         :type=Int',
                'gapId_stationEta          :type=Int',
                'gapId_stationPhi          :type=Int',
                'gapId_doubletR            :type=Int',
                'gapId_doubletZ            :type=Int',
                'gapId_doubletPhi          :type=Int',
                'gapId_gasGap              :type=Int',
                'globalX                   :type=Float',
                'globalY                   :type=Float',
                'globalZ                   :type=Float',
                'globalEta                 :type=Float',
                'globalPhi                 :type=Float',
                ]

    gap_keys = ['localTrackPosY              :type=Float',
                'localTrackPosZ              :type=Float',
                'localTrackPosValid          :type=Short',
                'localTrackPosInBoundsTight  :type=Short',
                'ClosestHitResidualEta       :type=Float',
                'ClosestHitResidualPhi       :type=Float',
                'ClosestGeoStripEtaIdentifier:type=ULong',
                'ClosestGeoStripEtaNumber    :type=Int',
                'ClosestGeoStripEtaPosZ      :type=Float',
                'ClosestGeoStripPhiIdentifier:type=ULong',
                'ClosestGeoStripPhiNumber    :type=Int',
                'ClosestGeoStripPhiPosY      :type=Float',
                'ClosestGeoStripResidualEta  :type=Float',
                'ClosestGeoStripResidualPhi  :type=Float',
                ]


    rpc_chan_keys = ['padSector     :type=Short', 
                     'padOnlineId   :type=Short',
                     'cmaOnlineId   :type=Short',
                     'channel       :type=Short',
                     'bcid          :type=Short',
                     'time          :type=Short',
                     'ijk           :type=Short',
                     'ovl           :type=Short',
                     'thr           :type=Short',
                    ]

    rpc_hit_keys = ['stripNumber     :type=Short',  
                    'StripIdentifier :type=ULong',  
                    'prdTime         :type=Float',]

    if True:
        rpc_hit_keys += ['gapId_identifier:type=ULong']
    else:
        rpc_hit_keys += geo_keys

    rpc_eta_keys = rpc_hit_keys + ['StripPosZ:type=Float',
                                   'GlobalStripPosX:type=Float',
                                   'GlobalStripPosY:type=Float',
                                   'GlobalStripPosZ:type=Float']

    rpc_phi_keys = rpc_hit_keys + ['StripPosY:type=Float',
                                   'GlobalStripPosX:type=Float',
                                   'GlobalStripPosY:type=Float',
                                   'GlobalStripPosZ:type=Float']

    roi_keys = ['MuonRoIDR   :type=Float',
                'eta         :type=Float',
                'phi         :type=Float',
                'roiNumber   :type=Short',
                'source      :type=Short',
                'hemisphere  :type=Short',
                'sectorID    :type=Short',
                'thrNumber   :type=Short',
                'overlapFlags:type=Short',
                'bcid_rel    :type=Short']

    alg = physicsBase.AlgConfig(name, 'WriteNtuple')    
    alg.SetKey('CandName',       cand_name)
    alg.SetKey('TreeName',       tree_name)
    alg.SetKey('Debug',          options.debug_alg == 'WriteNtuple')
    alg.SetKey('UseTopLevelDir', 'yes')
    alg.SetKey('Print',          'yes')
    alg.SetKey('PrintCuts',      'no')
    alg.SetKey('WriteReco',      'no')
    alg.SetKey('WriteCandReco',  'no')
    alg.SetKey('WriteCand',      'yes')

    alg.SetKey('BranchObjKeys',  'gap_, rpc_chan_, rpc_hit_eta_, rpc_hit_phi_, matched_roi_')

    alg.SetKey('KeyEvent',     str(',').join(event_keys  ))
    alg.SetKey('gap_',         str(',').join(gap_keys + geo_keys))
    alg.SetKey('rpc_chan_',    str(',').join(rpc_chan_keys))
    alg.SetKey('rpc_hit_eta_', str(',').join(rpc_eta_keys))
    alg.SetKey('rpc_hit_phi_', str(',').join(rpc_phi_keys))
    alg.SetKey('matched_roi_', str(',').join(roi_keys))

    return alg
