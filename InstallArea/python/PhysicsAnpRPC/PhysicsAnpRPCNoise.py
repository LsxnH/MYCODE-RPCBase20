import os
import sys
import re

import PhysicsAnpBase.PhysicsAnpBaseConfig as physicsBase
import PhysicsAnpRPC .PhysicsAnpRPCConfig  as config

from PhysicsAnpBase .PhysicsAnpBaseConfig  import CutItem
from PhysicsAnpBase .PhysicsAnpBaseConfig  import addCuts
from PhysicsAnpBase .PhysicsAnpBaseConfig  import getAlgName
from PhysicsAnpRPC  .PhysicsAnpRPCConfig   import getCutValue

clog = physicsBase.getLog(os.path.basename(__file__))

#========================================================================================================
# Top level C++ module to read ntuples and configure and run algs
#========================================================================================================
def prepareReadModule(ROOT, options, files, top_algs):

    run = config.prepareReadNtuple(ROOT, options, files, hist_config=['PhysicsAnpRPC/config/rpc-eff'])

    if options.trees != None:
        run.SetKey('TreeName', options.trees)
    else:
        run.SetKey('TreeName', 'nominal')

    var_nicks = []
    var_vetos = []
    vec_vetos = []

    run.SetKey('VarNicks',    ','.join(var_nicks))
    run.SetKey('VetoVars',    ','.join(var_vetos))
    run.SetKey('VetoVecs',    ','.join(vec_vetos))

    run.SetKey('Print',              'yes')
    run.SetKey('Debug',              options.debug_run or options.debug)
    run.SetKey('DebugVars',          'no')
    run.SetKey('NPrint',             1000)
    run.SetKey('NEventPerFile',      0)
    run.SetKey('FillTrueParts',      'no')
    run.SetKey('FillAllTrueParts',   'no')
    run.SetKey('PrintObjectFactory', 'no')

    run.SetKey('PrefixMuon',         'm_muon_')
    run.SetKey('PrefixHit',          'm_rpc_hit_')

    run.SetKey('Lists',              'm_muon_, m_rpc_hit_')
    run.SetKey('GroupKeys::m_muon_', 'RpcExtrapolate_, RpcHitOnTrackMS_')
    
    for f in files:
        run.StoreInputFile(f)

    if options.print_vars:
        ROOT.Anp.Var.PrintAllVars()   

    run.AddTopAlg('topAlg', top_algs)

    if options.debug_all:
        run.SetKey('Debug', 'yes')

    return run

#========================================================================================================
# Configure top level algorithms
#========================================================================================================
def prepareJobConfig(ROOT, options, files):

    clog.info('prepareJobConfig - prepare python configuration...')

    #-------------------------------------------------------------------------
    # Configure ntuple processing helper algorithms and candidate maker(s)
    #
    cand_1mu = getPrepRpcCand1mu('prepRpcCand1mu', options)

    cand_algs = [cand_1mu]
    prep_algs = []

    prep_algs += [config.getPrepReco   ('prepReco',     options),
                  config.getPrepRpcGeo ('prepRpcGeo',   options, ROOT),
                         getPrepRpcMuon('prepRpcMuon',  options)] + cand_algs

    plot_algs = [getPlotRpcNoise(cand_1mu, '', options)]

    #-------------------------------------------------------------------------
    # Configure top level algorithms
    #
    top_algs  = prep_algs
    top_algs += plot_algs

    config.addPrintEvent('printEvent', options, top_algs)
    
    #-------------------------------------------------------------------------
    # Prepare and configure top level C++ module
    #
    run = prepareReadModule(ROOT, options, files, top_algs)

    clog.info('prepareJobConfig - process up to %d events' %options.nevent)
    clog.info('prepareJobConfig - all done')

    return run

#========================================================================================================
def getPrepRpcMuon(name, options):

    alg = physicsBase.AlgConfig(name, 'PrepRpcMuon')
    alg.SetKey('Debug',               options.debug)
    alg.SetKey('DebugBme',            options.do_bme)
    alg.SetKey('Print',              'yes')
    alg.SetKey('KeyRpcExtrap',       'RpcExtrapolate_')
    alg.SetKey('KeyRpcResult',       'RpcExtrapolationResult')
    alg.SetKey('KeyRpcResultGeoCut', 'RpcExtrapolationResultGeoCut')
    alg.SetKey('BmeNtupleInstance',  'bme')
    alg.SetKey('MissingGeoStripIds', 'miss_strip_ids.txt')

    rpc_intime_cut   = [CutItem('CutHitTime',     'fabs([prdTime]) < 12.5')]
    rpc_residual_cut = [CutItem('CutHitResidual', 'fabs([HitResidual]) < 30.0')]

    rpc_clus_intime_cut   = [CutItem('CutClusterTime',     'fabs([ClusterPrdTime]) < 12.5')]
    rpc_clus_residual_cut = [CutItem('CutClusterResidual', 'fabs([ClusterResidual]) < 30.0')]

    addCuts(alg, 'CutInBounds',       getResultCuts('InBoundsLoose'))
    addCuts(alg, 'CutInBoundsTight',  getResultCuts('InBoundsTight'))
    addCuts(alg, 'CutInBoundsGeoCut', getResultCuts('InBoundsGeoCut'))

    addCuts(alg, 'CutRpcInTimeHit',   rpc_intime_cut)
    addCuts(alg, 'CutRpcResidualHit', rpc_residual_cut)

    addCuts(alg, 'CutRpcInTimeCluster',   rpc_clus_intime_cut)
    addCuts(alg, 'CutRpcResidualCluster', rpc_clus_residual_cut)

    return alg

#========================================================================================================
def getPrepRpcCand1mu(name, options):

    alg = physicsBase.AlgConfig(name, 'PrepRpcCandSingleMuon')
    alg.SetKey('Debug',               options.debug)
    alg.SetKey('Print',              'yes')

    addCuts(alg, 'CutMuonSig', getMuonProbeCuts(name))

    return alg

#========================================================================================================
# Configure plotting algorithms
#========================================================================================================
def getPlotRpcNoise(cand, key, options, do_strip_times=True, do_muon_veto=True):

    '''
    This function configures algorithm(s) for making plots:
    -- select events with at least one signal muon
    -- use biased signal muons to measure RPC efficiency
    -- currently no event level trigger cuts are applied

    These plots are used to measure RPC detector efficiency at various level of granularity.
    '''

    name = 'plotRpcNoise_%s%s' %(getAlgName(cand), key)

    alg = physicsBase.AlgConfig(name, 'PlotRpcNoise')
    alg.SetKey('Debug',            'no')
    alg.SetKey('Print',            'yes')
    alg.SetKey('HistKey',          'PlotRpcNoise')
    alg.SetKey('PlotEachGap',      'yes')
    alg.SetKey('PlotStripHists',    do_strip_times)
    alg.SetKey('DoMuonVeto',        do_muon_veto)
    alg.SetKey('KeyRpcResult',     'RpcExtrapolationResultGeoCut')
    alg.SetKey('SelectCandName' ,   getAlgName(cand))

    if options.small_noise_hists:
        alg.SetKey('DoLBTree',       'yes')
        alg.SetKey('PlotStrip2d',    'no')
        alg.SetKey('PlotStripHists', 'no')

    alg.AddCuts('CutHitBC0',   [CutItem('CutTime', 'fabs([prdTime]) <  12.5')])
    alg.AddCuts('CutHitNoise', [CutItem('CutTime', '[prdTime] < -12.5 && fabs([prdTime]) < 87.5')])

    if options.do_trig:
        alg.AddCuts('CutRecoEvent', [CutItem('CutTrig', 'HLT_mu26_ivarmedium > 0 || HLT_mu50 > 0')])

    return alg

#========================================================================================================
# Event cuts
#========================================================================================================
def getEventCuts(key):

    cuts = []
    cuts += [CutItem('CutGRL',           'PassGRL > 0')]
    cuts += [CutItem('CutJetCleaning',   'NJetBad == 0')]
    cuts += [CutItem('CutEventCleaning', 'PassEventCleaning > 0')]
    
    return cuts

#========================================================================================================
def getMuonProbeCuts(key):

    cuts  = []
    cuts += [CutItem('CutPt',       'Pt  > 10.0e3')]
    cuts += [CutItem('CutEta',      'fabs([Eta]) < 2.5')]
    cuts += [CutItem('CutPID',      'Tight == 1')]
    cuts += [CutItem('CutZ0Sin',    'fabs([Z0Sin]) < 0.5')]
    cuts += [CutItem('CutD0Sig',    'fabs([D0Sig]) < 3.0')]
    cuts += [CutItem('CutInBounds', 'NGasGapInBoundsGeoCut > 0')]

    return cuts

#========================================================================================================
def getMuonCuts(key):

    cuts  = []
    cuts += [CutItem('CutPt',       'Pt  > 10.0e3')]
    cuts += [CutItem('CutEta',      'Eta < 2.5',  abs=True)]

    if key.count('_loose'):
        cuts += [CutItem('CutPID',      'Loose == 1')]
        cuts += [CutItem('CutZ0Sin',    'Z0Sin < 0.5', abs=True)]
        cuts += [CutItem('CutD0Sig',    'D0Sig < 3.0', abs=True)]

    elif key.count('_tight'):
        cuts += [CutItem('CutPID',      'Tight == 1')]
        cuts += [CutItem('CutZ0Sin',    'Z0Sin < 0.5', abs=True)]
        cuts += [CutItem('CutD0Sig',    'D0Sig < 3.0', abs=True)]

    if key.count('InBoundsLoose'):
        cuts += [CutItem('CutInBounds', 'NGasGapInBounds > 0')]
    elif key.count('InBoundsTight'):
        cuts += [CutItem('CutInBounds', 'NGasGapInBoundsTight > 0')]

    return cuts

#========================================================================================================
def getResultCuts(key):

    cuts = []

    if key.count('InBoundsLoose'):
        cuts += [CutItem('CutValid',    'localTrackPosValid > 0'),
                 CutItem('CutInBounds', 'localTrackPosInBounds > 0')]
    elif key.count('InBoundsTight'):
        cuts += [CutItem('CutValid',    'localTrackPosValid > 0'),
                 CutItem('CutInBounds', 'localTrackPosInBoundsTight > 0')]
    elif key.count('InBoundsGeoCut'):
        cuts += [CutItem('CutValid',    'localTrackPosValid > 0'),
                 CutItem('CutInBounds', 'localTrackPosInBoundsTight > 0'),
                 CutItem('CutGeoEta',   'ClosestGeoStripResidualEta < 20.0', abs=True),
                 CutItem('CutGeoPhi',   'ClosestGeoStripResidualPhi < 20.0', abs=True)]
    else:
        pass

    return cuts
