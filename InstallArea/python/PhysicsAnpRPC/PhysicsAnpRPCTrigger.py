import os
import sys
import re

import PhysicsAnpBase.PhysicsAnpBaseConfig  as physicsBase
import PhysicsAnpRPC .PhysicsAnpRPCConfig   as config
import PhysicsAnpRPC .PhysicsAnpRPCPanelEff as panels
import PhysicsAnpRPC.PhysicsAnpRPCNtuple    as ntuple
import PhysicsAnpRPC.PhysicsAnpRPCUtils     as utils

from PhysicsAnpBase .PhysicsAnpBaseConfig  import CutItem
from PhysicsAnpBase .PhysicsAnpBaseConfig  import addCuts
from PhysicsAnpBase .PhysicsAnpBaseConfig  import getAlgName
from PhysicsAnpRPC  .PhysicsAnpRPCConfig   import getCutValue

clog = physicsBase.getLog(os.path.basename(__file__))

#========================================================================================================
# Configure top level algorithms
#========================================================================================================
def prepareJobConfig(ROOT, options, files):

    clog.info('prepareJobConfig - prepare python configuration...')

    #-------------------------------------------------------------------------
    # Configure ntuple processing helper algorithms and candidate maker(s)
    #
    cand_ztp = panels.getPrepRpcCandZTP('prepRpcCandZTP', options)
    cand_1mu = panels.getPrepRpcCand1mu('prepRpcCand1mu', options)

    cand_algs = [cand_ztp, cand_1mu]
    prep_algs = []
    plot_algs = []

    prep_algs += [config.getPrepReco   ('prepReco',     options),
                  config.getPrepRpcGeo ('prepRpcGeo',   options, ROOT),
                  panels.getPrepRpcMuon('prepRpcMuon',  options, do_raw_chan=True)]

    prep_algs += [getPrepL1Muon('prepL1Muon',   options)] + cand_algs
                  
    prep_algs += [getMakeTriggerNtuple(cand_ztp, '', options)]

    plot_algs += [getPlotL1RoICand(cand_ztp, '',        options),
                  getPlotL1RoICand(cand_ztp, '_mupos',  options),
                  getPlotL1RoICand(cand_ztp, '_muneg',  options),
                  getPlotL1RoICand(cand_1mu, '',        options),
                  getPlotL1RoICand(cand_1mu, '_unbias', options)]

    #-------------------------------------------------------------------------
    # Configure top level algorithms
    #
    top_algs  = prep_algs
    top_algs += plot_algs

    config.addPrintEvent('printEvent', options, top_algs)
    
    #-------------------------------------------------------------------------
    # Prepare and configure top level C++ module
    #
    run = panels.prepareReadModule(ROOT, options, files, top_algs)

    clog.info('prepareJobConfig - process up to %d events' %options.nevent)
    clog.info('prepareJobConfig - all done')

    return run

#========================================================================================================
def getPrepL1Muon(name, options):

    alg = physicsBase.AlgConfig(name, 'PrepL1Muon')
    alg.SetKey('Debug',               options.debug_alg == 'PrepL1Muon')
    alg.SetKey('Print',               options.print_alg == 'PrepL1Muon')
    alg.SetKey('KeyL1MuonRoIInput',  'm_l1mu_ctpi_')
    alg.SetKey('KeyL1MuonRoIOutput', 'MatchedRoIs')

    addCuts(alg, 'CutRpcRoI',  [CutItem('CutRpcSource', '[source] == 0')])

    return alg

#========================================================================================================
def getPlotL1MuonAlg(name, options, thr=None, sector=None, hemi=None, do_plateau=False, do_bc0=False, do_nocrack=False):
    
    roi_cuts  = []
    muon_cuts = []
    sector_cuts = []

    if hemi != None and sector == None:
        name += '_hemi%d' %hemi
        roi_cuts += [CutItem('CutHemi', '[hemisphere] == %d' %hemi)]
  
    if hemi != None and sector != None:
        name += '_hemi%d_sector%02d' %(hemi, sector)
        muon_cuts += [CutItem('CutTriggerSector', '[TriggerSector] == %d'%(sector + 32*hemi))]

    if thr:
        name += '_thr%d' %thr
        roi_cuts += [CutItem('CutThr',  '[thrNumber] >= %d' %thr)]

    if do_bc0:
        name += '_bc0'
        roi_cuts += [CutItem('CutBcid', '[bcid_rel] == 0')]

    if do_plateau: 
        name += '_plateau'
        muon_cuts += [CutItem('CutPlateauPt', '[Pt] > 25.0e3')]

    if do_nocrack: 
        name += '_nocrack'
        muon_cuts += [CutItem('CutCrackEta', 'fabs([Eta]) > 0.1')]

    alg = physicsBase.AlgConfig(name, 'PlotL1Muon')
    alg.SetKey('MuonRoIKey',   'MatchedRoIs')
    alg.SetKey('Debug',        'no')
    alg.SetKey('Print',        'no')
    alg.SetKey('Plot2d',       'yes')
    alg.SetKey('HistKey',      'PlotL1Muon')

    addCuts(alg, 'CutRoI',  roi_cuts)
    addCuts(alg, 'CutMuon', muon_cuts)

    return alg

#========================================================================================================
def getPlotL1RoICand(cand, key, options):

    '''
    This function configures algorithm(s) for plotting muons
    -- select Z tag&probe candidate events
    -- rerequire that tag muon matches primary single muon trigger
    -- use unbiased probe muons to measure RPC efficience

    These plots are used to measure RPC detector efficiency at various level of granularity.
    '''

    #--------------------------------------------------------------------------------------------------------
    # Plot denominator/all muons
    #
    plot_algs  = []    
    plot_algs += [getPlotL1MuonAlg('plotL1Muon', options)]
    plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, do_plateau=True)]
    plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, do_nocrack=True)]
    plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, do_plateau=True, do_nocrack=True)]

    for sector in range(0, 32):
        sector_algs = [getPlotL1MuonAlg('plotL1Muon', options, sector=sector, hemi=0),
                       getPlotL1MuonAlg('plotL1Muon', options, sector=sector, hemi=1)]
        
        for alg in sector_algs:                
            alg.SetKey('DirName', 'sectors/%s' %(getAlgName(alg)))
            
        plot_algs += sector_algs

    #--------------------------------------------------------------------------------------------------------
    # Plot numerator muons matched to L1 trigger threshods
    #
    for thr in range(1, 7):
        plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, thr=thr)]
        plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, thr=thr, do_plateau=True)]
        plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, thr=thr, do_plateau=True, do_bc0=True)]
        plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, thr=thr, do_bc0=True)]
        plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, thr=thr, do_nocrack=True)]
        plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, thr=thr, do_nocrack=True, do_bc0=True)]
        plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, thr=thr, do_plateau=True, do_nocrack=True)]
        plot_algs += [getPlotL1MuonAlg('plotL1Muon', options, thr=thr, do_plateau=True, do_nocrack=True, do_bc0=True)]

        for sector in range(0, 32):
            sector_algs = [getPlotL1MuonAlg('plotL1Muon', options, thr=thr, sector=sector, hemi=0),
                           getPlotL1MuonAlg('plotL1Muon', options, thr=thr, sector=sector, hemi=1)]

            for alg in sector_algs:                
                alg.SetKey('DirName', 'sectors/%s' %(getAlgName(alg)))

            plot_algs += sector_algs

    #--------------------------------------------------------------------------------------------------------
    # Plot muon candidates
    #
    plot_cand = physicsBase.AlgConfig('plotCand', 'PlotCand')
    plot_cand.SetKey('HistKey', 'PlotCand')

    plot_algs += [plot_cand]

    name = 'plotL1_%s%s' %(getAlgName(cand), key)
    cuts = getRoICandCuts(name, options)

    chain = config.getRunChain(name, algs=plot_algs, cuts=cuts, options=options)
    chain.SetKey('DirName',         name)
    chain.SetKey('Debug',           options.debug)
    chain.SetKey('Print',           'no')
    chain.SetKey('CandName',        getAlgName(cand))

    return chain

#========================================================================================================
def getMakeTriggerNtuple(cand, key, options):

    '''
    This function configures algorithm(s) for making plots:
    -- select events with at least one signal muon
    -- use biased signal muons to measure RPC efficiency
    -- currently no event level trigger cuts are applied

    These plots are used to measure RPC detector efficiency at various level of granularity.
    '''

    copy_vars_cand = ['Run',
                      'LumiBlock',
                      'Event',
                      'bcid',
                      'NRecoVtx',
                      'ActualInteractions',
                      'AverageInteractions',
                      ]

    copy_vars_geogap = ['gapId_stationName',
                        'gapId_stationEta ',
                        'gapId_stationPhi ',
                        'gapId_doubletR',
                        'gapId_doubletZ',
                        'gapId_doubletPhi',
                        'gapId_gasGap',
                        'globalX',
                        'globalY',
                        'globalZ',
                        'globalEta',
                        'globalPhi',
                        ]

    copy_vars_geohit = ['gapId_identifier'] + copy_vars_geogap

    cuts_cand = [CutItem('CutMuon', '[NGasGapInBoundsSelect] > 0')]
    cuts_muon = []

    alg = physicsBase.AlgConfig('prepRpcCandNtuple', 'PrepRpcCandNtuple')
    alg.SetKey('Debug',             'no')
    alg.SetKey('Print',             'yes')
    alg.SetKey('KeyRpcResult',      'RpcExtrapolationResult')
    alg.SetKey('KeyL1MuonRoI',      'MatchedRoIs')
    alg.SetKey('OutRpcRoIs',        'matched_roi_')
    alg.SetKey('OutRpcGaps',        'gap_')
    alg.SetKey('OutRpcHitsEta',     'rpc_hit_eta_')
    alg.SetKey('OutRpcHitsPhi',     'rpc_hit_phi_')
    alg.SetKey('OutRpcChans',       'rpc_chan_')
    alg.SetKey('OutBmeHits',        'bme_hit_')

    alg.SetKey('CopyVarsCand',      ','.join(copy_vars_cand))
    alg.SetKey('CopyVarsGeoGap',    ','.join(copy_vars_geogap))
    alg.SetKey('CopyVarsGeoHit',    ','.join(copy_vars_geohit))

    addCuts(alg, 'CutCandOut', cuts_cand)
    addCuts(alg, 'CutMuon',    cuts_muon)

    if options.do_save_all_hit:
        alg.SetKey('AddRpcHitsForSelectedGapIds', 'no')

    #----------------------------------------------------------------------------------------------------
    # Configure WriteNtuple and RunChain algorithms
    #
    ntp = ntuple.getWriteNtupleAlgForTrigger('writeNtuple', options, getAlgName(alg), 'ntp_%s' %getAlgName(cand))

    name = 'makeRpcNtuple_%s%s' %(getAlgName(cand), key)
    cuts = getRoICandCuts(name, options)

    chain = config.getRunChain(name, algs=[alg, ntp], cuts=cuts, options=options)
    chain.SetKey('DirName',         name)
    chain.SetKey('Debug',           options.debug)
    chain.SetKey('Print',           'no')
    chain.SetKey('CandName',        getAlgName(cand))

    return chain

#========================================================================================================
# Cuts
#========================================================================================================
def getRoICandCuts(key, options):

    cuts  = []
    cuts += [CutItem('CutProbeEta', 'fabs([ProbeEta]) < 1.05')]

    if   key.count('mupos'): cuts += [CutItem('CutProbePDG', '[ProbePDG] == -13')]
    elif key.count('muneg'): cuts += [CutItem('CutProbePDG', '[ProbePDG] == +13')]

    if key.count('unbias'):
        cuts += panels.getNonMuonTriggerCuts(year=2018, match='')
    
    if key.count('ZTP'):
        cuts += panels.getMuonTriggerCuts(year=2017, match='')
        cuts += [CutItem('CutTagTrig', '[TagMatchTrig] > 0')]
        cuts += [CutItem('CutMll',     '[Mll] > 50.0e3 && [Mll] < 150.0e3')]
        cuts += [CutItem('CutDRll',    '[DRll] > 0.1')]
        cuts += [CutItem('CutOS',      '[LeptonChargeSum] == 0')]
    
    return cuts
