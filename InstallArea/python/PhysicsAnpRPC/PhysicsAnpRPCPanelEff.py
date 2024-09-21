import os
import sys
import re

import PhysicsAnpBase.PhysicsAnpBaseConfig as physicsBase
import PhysicsAnpRPC .PhysicsAnpRPCConfig  as config
import PhysicsAnpRPC .PhysicsAnpRPCNoise   as noise

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
    run.SetKey('Lists',              'm_muon_, m_rpc_hit_, m_l1mu_ctpi_, m_rpc_rdo_')

    if options.do_extrID:
        run.SetKey('GroupKeys::m_muon_', 'RpcExtrapolateID_, RpcHitOnTrackMS_')
    else:
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
    cand_ztp     = getPrepRpcCandZTP    ('prepRpcCandZTP',     options)
    cand_1mu     = getPrepRpcCand1mu    ('prepRpcCand1mu',     options)
    cand_multimu = getPrepRpcCandMultimu('prepRpcCandMultimu', options)

    cand_algs = [cand_ztp, cand_1mu, cand_multimu]
    prep_algs = []
    plot_algs = []

    if options.do_bme:
        prep_algs += [getPrepBmeEvent('prepBmeEvent', options, ROOT)]

    prep_algs += [config.getPrepReco   ('prepReco',     options),
                  config.getPrepRpcGeo ('prepRpcGeo',   options, ROOT),
                         getPrepRpcMuon('prepRpcMuon',  options)]

    prep_algs += [getPrepL1Muon('prepL1Muon',   options)] + cand_algs

    if options.do_rpc_muon:
        plot_algs += [getPlotRpcMuon('plotRpcMuon_nocut',                options),
                      getPlotRpcMuon('plotRpcMuon_loose_InBoundsLoose',  options),
                      getPlotRpcMuon('plotRpcMuon_loose_InBoundsTight',  options),
                      getPlotRpcMuon('plotRpcMuon_loose_InBoundsGeoCut', options),
                      getPlotRpcMuon('plotRpcMuon_tight_InBoundsLoose',  options),
                      getPlotRpcMuon('plotRpcMuon_tight_InBoundsTight',  options),
                      getPlotRpcMuon('plotRpcMuon_tight_InBoundsGeoCut', options)]

    #plot_algs += [getPlotRpcCand(cand_ztp, '',        options)]
    #plot_algs += [getPlotRpcCand(cand_1mu, '',        options)]

    if options.do_unbias:
        plot_algs += [getPlotRpcCand(cand_1mu, '_unbias', options)]

    if options.do_trigmu:
        plot_algs += [getPlotRpcCand(cand_1mu, '_trigmu', options)]

    if options.do_noise:
        plot_algs += [noise.getPlotRpcNoise(cand_multimu, '', options, do_strip_times=True, do_muon_veto=False)]

    if options.do_bme:
        plot_algs  = [getPlotBmeCand1mu(cand_1mu, '',     options)]
        plot_algs += [getMakeRpcNtuple (cand_1mu, '_BME', options, do_bme=True)]
        plot_algs += [getMakeRpcNtuple (cand_ztp, '_BME', options, do_bme=True)]

    if options.do_rpc_ntuple:
        plot_algs  = [getMakeRpcNtuple(cand_1mu, '', options, do_bme=False)]
        plot_algs += [getMakeRpcNtuple(cand_ztp, '', options, do_bme=False)]

    if options.do_clu_ntuple:
        #plot_algs  = [getMakeRpcNtuple(cand_1mu, '', options, do_bme=False)]
        plot_algs += [getMakeRpcNtuple(cand_ztp, '', options, do_bme=False)]
        #plot_algs += [getPlotRpcCand(cand_1mu, '',        options)]
        #plot_algs += [getPlotRpcCand(cand_1mu, '_mupos',  options)]
        #plot_algs += [getPlotRpcCand(cand_1mu, '_muneg',  options)]
    
    if options.print_rpc:
        plot_algs = [getPrintRpcPanel1mu(cand_1mu, '', options)]

    #-------------------------------------------------------------------------
    # check fake trigger rates
    #
    if options.do_fake_trig:
        cand_algs = [cand_multimu]
        prep_algs = []
        plot_algs = []

        prep_algs += [config.getPrepReco   ('prepReco',     options),
                      config.getPrepRpcGeo ('prepRpcGeo',   options, ROOT),
                             getPrepRpcMuon('prepRpcMuon',  options)]

        prep_algs += cand_algs
        for thr in ['MU4', 'MU10', 'MU11', 'MU20', 'MU21', 'mu20M21', 'mu26', 'mu20M6']:
            for source in ['Rpc','Tgc']:
                plot_algs += [getPlotFakeTrigCand(cand_multimu, '', options, thr, source)]

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
def getPrepRpcMuon(name, options, do_raw_chan=False):

    alg = physicsBase.AlgConfig(name, 'PrepRpcMuon')
    alg.SetKey('Debug',               options.debug)
    alg.SetKey('DebugBme',            'no')
    alg.SetKey('Print',              'yes')
    alg.SetKey('KeyRpcResult',       'RpcExtrapolationResult')
    alg.SetKey('KeyRpcResultGeoCut', 'RpcExtrapolationResultGeoCut')
    alg.SetKey('BmeNtupleInstance',  'bme')
    alg.SetKey('MissingGeoStripIds', 'miss_strip_ids.txt')

    if do_raw_chan:
        alg.SetKey('KeyRpcChans', 'm_rpc_rdo_')

    if options.do_extrID: alg.SetKey('KeyRpcExtrap', 'RpcExtrapolateID_')
    else:                 alg.SetKey('KeyRpcExtrap', 'RpcExtrapolate_')

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
def getPrepRpcCandZTP(name, options):

    alg = physicsBase.AlgConfig(name, 'PrepRpcCandZTP')
    alg.SetKey('Debug',               options.debug)
    alg.SetKey('Print',              'yes')

    alg.AddCuts('CutMuonTag',        getMuonTagCuts  (name))
    alg.AddCuts('CutMuonProbe',      getMuonProbeCuts(name))
    alg.AddCuts('CutMuonTrigMatch',  getMuonTriggerCuts(year=2017, match='match_'))

    return alg

#========================================================================================================
def getPrepRpcCand1mu(name, options):

    alg = physicsBase.AlgConfig(name, 'PrepRpcCandSingleMuon')
    alg.SetKey('Debug',               options.debug)
    alg.SetKey('Print',              'yes')

    if options.do_cosmic:
        addCuts(alg, 'CutMuonSig', getMuonCosmicCuts(name))
    else:
        addCuts(alg, 'CutMuonSig', getMuonProbeCuts(name))

    alg.AddCuts('CutMuonTrigMatch',  getMuonTriggerCuts(year=2017, match='match_'))

    return alg

#========================================================================================================
def getPrepRpcCandMultimu(name, options):

    alg = physicsBase.AlgConfig(name, 'PrepRpcCandMultiMuons')
    alg.SetKey('Debug',               options.debug)
    alg.SetKey('Print',              'yes')

    if options.do_cosmic:
        addCuts(alg, 'CutMuonSig', getMuonCosmicCuts(name))
    elif options.do_fake_trig:
        addCuts(alg, 'CutMuonSig', getMuonProbeLooseCuts(name))
    else:
        addCuts(alg, 'CutMuonSig', getMuonProbeCuts(name))

    alg.AddCuts('CutMuonTrigMatch', getMuonTriggerCuts(year=2017, match='match_'))
    if options.do_fake_trig:
        alg.AddCuts('CutEvent',         [CutItem('CutTrig', '[HLT_noalg_L1MU4] > 0 || [HLT_noalg_L1MU6] > 0 || [HLT_noalg_L1MU10] > 0 || [HLT_noalg_L1MU11] > 0 || [HLT_noalg_L1MU20] > 0 || [HLT_noalg_L1MU21] > 0 || [HLT_mu26_ivarmedium] > 0')])
    else:
        alg.AddCuts('CutEvent',         [CutItem('CutTrig', '[HLT_mu26_ivarmedium] > 0')])

    return alg

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
# Code to read and prepare BME hits from secondary readout
#========================================================================================================
def getPrepBmeEvent(name, options, ROOT):

    bme_ntuple_instance = 'bme'

    #----------------------------------------------------------------------------------------------------
    # Prep event loop for reading ntuples with bme secondary hits
    #
    alg_bme = physicsBase.AlgConfig('prepBmeHit', 'PrepBmeHit')
    alg_bme.SetKey('Debug',                 'no')
    alg_bme.SetKey('Print',                 'yes')
    alg_bme.SetKey('BmeHitVecNameIn',       'RPC_')
    alg_bme.SetKey('BmeNtupleInstance',     bme_ntuple_instance)
    alg_bme.SetKey('BmeGeoFile',            options.bme_geo)

    var_nicks = ['evtNum64:Event',
                 'runNum:Run',
                 'TDC:rpcTDC']

    var_vetos = ['evtNum']
    vec_vetos = []

    run = config.prepareReadNtuple(ROOT, options, [], hist_config=[])
    run.SetKey('Print',            'yes')
    run.SetKey('Debug',            'no')
    run.SetKey('DebugVars',        'no')
    run.SetKey('NPrint',           10000000)
    run.SetKey('NEventPerFile',    0)
    run.SetKey('FillTrueParts',    'no')
    run.SetKey('FillAllTrueParts', 'no')

    run.SetKey('Lists',            'RPC_')
    run.SetKey('AlgName',          'readBmeNtuple')
    run.SetKey('OutputFile',       '')
    run.SetKey('NtupleInstance',   'bme')

    run.SetKey('VarNicks',         ','.join(var_nicks))
    run.SetKey('VetoVars',         ','.join(var_vetos))
    run.SetKey('VetoVecs',         ','.join(vec_vetos))

    run.AddTopAlg('topAlg', [alg_bme])

    #----------------------------------------------------------------------------------------------------
    alg = physicsBase.AlgConfig(name, 'PrepBmeEvent')
    alg.SetKey('Debug',                 'no')
    alg.SetKey('Print',                 'yes')
    alg.SetKey('BmeListFileName',       options.bme_event_map)
    alg.SetKey('BmeNtupleInstance',     bme_ntuple_instance)
    alg.SetKey('ReadRegistry',          run.GetRegistryConfig())

    return alg

#========================================================================================================
# Configure plotting algorithms
#========================================================================================================
def getPlotRpcMuon(name, options):

    '''
    This algorithm makes basic plots for:
    -- selected muons
    -- results of extrapolating muons to the RPC detector
    
    This algorithm is intended for debugging and developing code
    '''

    alg = physicsBase.AlgConfig(name, 'PlotRpcMuon')
    alg.SetKey('Debug',           'no')
    alg.SetKey('Print',           'yes')
    alg.SetKey('HistKey',         'PlotRpcMuon')
    alg.SetKey('RpcResultKey',    'RpcExtrapolationResult')

    addCuts(alg, 'CutMuon',    getMuonCuts  (name))
    addCuts(alg, 'CutResult',  getResultCuts(name))

    return alg

#========================================================================================================
def getPlotRpcCand(cand, key, options):

    '''
    This function configures plotting algorithm(s) for plotting RPC response to muon passage
    These plots are used to measure RPC detector efficiency at various level of granularity.
    '''

    name = 'plotRpcCand_%s%s' %(getAlgName(cand), key)

    plot_cand = physicsBase.AlgConfig('plotCand', 'PlotCand')
    plot_cand.SetKey('HistKey', 'PlotCand')

    alg = physicsBase.AlgConfig('plotRpcCand', 'PlotRpcCand')
    alg.SetKey('Debug',            'no')
    alg.SetKey('Print',            'yes')
    alg.SetKey('HistKey',          'PlotRpcCand')
    alg.SetKey('PlotEachGap',      'yes')    
    alg.SetKey('KeyRpcResult',     'RpcExtrapolationResultGeoCut')
    alg.SetKey('DirName' ,         '')
    alg.SetKey('MuonRoIKey',       'MatchedRoIs')

    algs = [plot_cand, alg]
    cuts = getRpcCandCuts(name, options)

    chain = config.getRunChain(name, algs=algs, cuts=cuts, options=options)
    chain.SetKey('DirName',         name)
    chain.SetKey('Debug',           options.debug)
    chain.SetKey('Print',           'no')
    chain.SetKey('CandName',        getAlgName(cand))

    return chain

#========================================================================================================
def getPlotFakeTrigCand(cand, key, options, thr, source):

    '''
    '''

    plot_algs  = []

    #--------------------------------------------------------------------------------------------------------
    # Plot muon candidates
    #
    plot_cand = physicsBase.AlgConfig('plotCand', 'PlotCand')
    plot_cand.SetKey('KeyHist', 'PlotHLTNoalg')

    alg = physicsBase.AlgConfig('plotFakeTrigCand', 'PlotFakeTrigCand')
    alg.SetKey('Debug',            'no')
    alg.SetKey('Print',            'yes')
    alg.SetKey('DirName' ,         '')
    alg.SetKey('KeyL1MuonRoIInput',  'm_l1mu_ctpi_')
    if source.count('Tgc'):
        alg.SetKey('DoBarrel',  'no')
    if thr.count('mu20M6'):
        alg.SetKey('VetoRoIsMU21',  'yes')

    addCuts(alg, 'CutRpcRoI',  [CutItem('CutRpcSource', '[source] == 0')])
    addCuts(alg, 'CutTgcRoI',  [CutItem('CutTgcSource', '[source] == 1')])

    plot_algs += [plot_cand] + [alg]

    name = 'plotL1_%s%s%s%s' %(getAlgName(cand), key, thr, source)
    cuts = []
    if thr.count('MU'):
        cuts += [CutItem('NoalgTrigger', '[HLT_noalg_L1%s] > 0'%thr)]
    elif thr.count('mu20M21'):
        cuts += [CutItem('NoalgTrigger', '[HLT_noalg_L1MU20] > 0 && [HLT_noalg_L1MU21] == 0')]
    elif thr.count('mu20M6'):
        cuts += [CutItem('NoalgTrigger', '[HLT_noalg_L1MU20] > 0')]
    else:
        cuts += [CutItem('NoalgTrigger', '[HLT_mu26_ivarmedium] > 0')]

    chain = config.getRunChain(name, algs=plot_algs, cuts=cuts, options=options)
    chain.SetKey('DirName',         name)
    chain.SetKey('Debug',           options.debug)
    chain.SetKey('Print',           'no')
    chain.SetKey('CandName',        getAlgName(cand))

    return chain

#========================================================================================================
def getPlotBmeCand1mu(cand, key, options):

    '''
    This function configures algorithm(s) for making plots:
    -- select events with at least one signal muon
    -- use biased signal muons to measure RPC efficiency
    -- currently no event level trigger cuts are applied

    These plots are used to measure RPC detector efficiency at various level of granularity.
    '''

    alg = physicsBase.AlgConfig('plotBmeCand', 'PlotBmeCand')
    alg.SetKey('Debug',             'no')
    alg.SetKey('Print',             'yes')
    alg.SetKey('HistKey',           'PlotBmeCand')
    alg.SetKey('PlotEachGap',       'yes')    
    alg.SetKey('KeyRpcResult',      'RpcExtrapolationResultGeoCut')
    alg.SetKey('BmeNtupleInstance', 'bme')
    alg.SetKey('DirName' ,          '')

    name = 'plotBmeCand_%s%s' %(getAlgName(cand), key)

    chain = config.getRunChain(name, algs=[alg], cuts=[], options=options)
    chain.SetKey('DirName',         name)
    chain.SetKey('Debug',           options.debug)
    chain.SetKey('Print',           'no')
    chain.SetKey('CandName',        getAlgName(cand))

    return chain

#========================================================================================================
def getMakeRpcNtuple(cand, key, options, do_bme=False):

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
                      'Match2ndReadoutEvent',
                      ]

    copy_vars_geogap = ['gapId_stationName',
                        'gapId_stationEta ',
                        'gapId_stationPhi ',
                        'gapId_doubletR',
                        'gapId_doubletZ',
                        'gapId_doubletPhi',
                        'gapId_gasGap',
                        'stripWidthEta',
                        'stripWidthPhi',
                        'stripPitchEta',
                        'stripPitchPhi',
                        'stripLengthEta',
                        'stripLengthPhi',
                        ]

    copy_vars_geohit = ['gapId_identifier'] + copy_vars_geogap

    alg = physicsBase.AlgConfig('prepRpcCandNtuple', 'PrepRpcCandNtuple')
    alg.SetKey('Debug',                       'no')
    alg.SetKey('Print',                       'yes')
    alg.SetKey('KeyRpcResult',                'RpcExtrapolationResultGeoCut')
    alg.SetKey('BmeNtupleInstance',           'bme')
    alg.SetKey('OutRpcGaps',                  'gap_')
    alg.SetKey('OutRpcHitsEta',               'rpc_hit_eta_')
    alg.SetKey('OutRpcHitsPhi',               'rpc_hit_phi_')
    alg.SetKey('AddRpcHitsForSelectedGapIds', 'yes')

    cuts_cand   = [CutItem('CutMuon',   '[NGasGapInBoundsSelect] > 0')]
    cuts_muon   = [CutItem('CutMuon',   '[NGasGapInBoundsGeoCut] > 0')]
    cuts_gasgap = []

    if do_bme:
        #
        # Select only BME/BOE panels and also BOL station 6 panels
        #
        #cuts_gasgap = [CutItem('CutGasGap', '([gapId_stationName] == 53) || ([gapId_stationName] == 4 && (abs([gapId_stationEta]) > 5 || abs([gapId_stationEta]) == 1 || abs([gapId_stationEta]) == 4)) || ([gapId_stationName] == 2 && ([gapId_stationEta] == 4 || [gapId_stationEta] == 5)) || ([gapId_stationName] == 2 && (abs([gapId_stationEta]) == 1 || abs([gapId_stationEta]) == 4 || abs([gapId_stationEta]) == 6) && ([gapId_doubletR] == 2))', debug=False)]
        #
        # Select only small sectors panels: BMS and BOS
        cuts_gasgap = [CutItem('CutGasGap', '([gapId_stationName] == 3) || ([gapId_stationName] == 5)', debug=False)]

        #
        # Record secondary hits
        #
        alg.SetKey('OutBmeHits', 'bme_hit_')

    #if options.do_clu_ntuple:
    #    cuts_gasgap = [CutItem('CutGasGap', '(abs([gapId_stationEta]) == 1) || (abs([gapId_stationEta]) == 6)', debug=False)]

    alg.SetKey('CopyVarsCand',      ','.join(copy_vars_cand))
    alg.SetKey('CopyVarsGeoGap',    ','.join(copy_vars_geogap))
    alg.SetKey('CopyVarsGeoHit',    ','.join(copy_vars_geohit))

    addCuts(alg, 'CutCandOut', cuts_cand)
    addCuts(alg, 'CutMuon',    cuts_muon)
    addCuts(alg, 'CutGasGap',  cuts_gasgap)
    addCuts(alg, 'CutRpcHit',  cuts_gasgap)

    #----------------------------------------------------------------------------------------------------
    # Configure WriteNtuple and RunChain algorithms
    #
    import PhysicsAnpRPC.PhysicsAnpRPCNtuple as RPCNtuple

    ntp = RPCNtuple.getWriteNtupleAlgForRPC('writeNtuple', options, getAlgName(alg), 'ntp_%s' %getAlgName(cand), do_bme)

    name = 'makeRpcNtuple_%s%s' %(getAlgName(cand), key)

    chain = config.getRunChain(name, algs=[alg, ntp], cuts=getRpcCandCuts(name, options), options=options)
    chain.SetKey('DirName',         name)
    chain.SetKey('Debug',           options.debug)
    chain.SetKey('Print',           'no')
    chain.SetKey('CandName',        getAlgName(cand))

    return chain

#========================================================================================================
def getPrepGapBmeCandForNtuple(cand, key, options):

    '''
    This function configures algorithm that makes CandEvent 
    from RpcGapResults for muon hits in BOE/BME panels.
    '''

    name = 'prepGapCand_%s%s' %(getAlgName(cand), key)

    alg = physicsBase.AlgConfig(name, 'PrepGapCand')
    alg.SetKey('Debug',             'yes')
    alg.SetKey('Print',             'yes')
    alg.SetKey('HistKey',           'PrepBmeCand')
    alg.SetKey('PlotEachGap',       'yes')    
    alg.SetKey('KeyRpcResult',      'RpcExtrapolationResultGeoCut')
    alg.SetKey('SelectCandName' ,    getAlgName(cand))

    geo_gap_vars = ['gapId_stationName',
                    'gapId_stationEta',
                    'gapId_stationPhi',
                    'gapId_doubletR',
                    'gapId_doubletZ',
                    'gapId_doubletPhi',
                    'gapId_gasGap']

    alg.SetKey('CopyGeoGapVars', ','.join(geo_gap_vars))

    return alg

#========================================================================================================
def getPrintRpcPanel1mu(cand, key, options):

    '''
    This function configures algorithm(s) for printing gas gaps and hits
    '''

    name = 'printRpcPanel_%s%s' %(getAlgName(cand), key)

    alg = physicsBase.AlgConfig(name, 'PrintRpcPanel')
    alg.SetKey('Debug',               'no')
    alg.SetKey('PrintEachGap',        'yes')    
    alg.SetKey('PrintAllHits',        'yes')    
    alg.SetKey('KeyRpcResult',        'RpcExtrapolationResultGeoCut')
    alg.SetKey('SelectCandName' ,      getAlgName(cand))
    alg.SetKey('EventListFileNameBME', 'bme_list.txt')
    alg.SetKey('EventListFileNameBOE', 'boe_list.txt')
    alg.SetKey('BmeNtupleInstance',    'bme')

    event_cuts      = []
    specl_cuts_BME  = [CutItem('CutGapBME',      'gapId_stationName == 53')]
    specl_cuts_BOE  = [CutItem('CutGapBOE',      'gapId_stationName == 4')]
    specl_cuts_BOE += [CutItem('CutStationEta',  'abs([gapId_stationEta]) == 8')]
    specl_cuts_BOE += [CutItem('CutStationPhi',  'gapId_stationPhi == 7')]
    specl_cuts_BOE += [CutItem('CutDoubletZ',    'gapId_doubletZ == 1')]

    addCuts(alg, 'CutCandEvent',     event_cuts)
    addCuts(alg, 'CutSpecialGapBME', specl_cuts_BME)
    addCuts(alg, 'CutSpecialGapBOE', specl_cuts_BOE)

    return alg

#========================================================================================================
# Event cuts
#========================================================================================================
def getRpcCandCuts(key, options):

    cuts  = []
    if not key.count('makeRpcNtuple'):
        cuts += [CutItem('CutProbePt', '[ProbePt] > 10.0e3')]

    if   key.count('mupos'): cuts += [CutItem('CutProbePDG', '[ProbePDG] == -13')]
    elif key.count('muneg'): cuts += [CutItem('CutProbePDG', '[ProbePDG] == +13')]

    if key.count('unbias'):
        cuts += getNonMuonTriggerCuts(year=2018, match='')

    if key.count('trigmu'):
        cuts += [CutItem('CutTrigMatch', 'NMuonMatchTrig > 0')]

    if key.count('ZTP'):
        cuts += getMuonTriggerCuts(year=2017, match='')
        cuts += [CutItem('CutTagTrig', 'TagMatchTrig > 0')]
        cuts += [CutItem('CutMll',     '[Mll] > 50.0e3 && [Mll] < 150.0e3')]
        cuts += [CutItem('CutDRll',    'DRll > 0.1')]

    if options.min_lb and options.max_lb:
        cuts += [CutItem('CutLB', '[LumiBlock] >= %d && [LumiBlock] <= %d' %(options.min_lb, options.max_lb))]
    elif options.min_lb:
        cuts += [CutItem('CutLB', '[LumiBlock] >= %d' %(options.min_lb))]
    elif options.max_lb:
        cuts += [CutItem('CutLB', '[LumiBlock] <= %d' %(options.max_lb))]

    clog.info('%s %s' %(options.min_lb, options.max_lb))

    return cuts

#========================================================================================================
def getRoICandCuts(key, options):

    cuts  = []
    cuts += [CutItem('CutProbeEta', 'fabs([ProbeEta]) < 1.05')]

    if key.count('unbias'):
        cuts += getNonMuonTriggerCuts(year=2018, match='')
    
    if key.count('ZTP'):
        cuts += getMuonTriggerCuts(year=2017, match='')
        cuts += [CutItem('CutTagTrig', 'TagMatchTrig > 0')]
        if options.do_dphill_cut:
            cuts += [CutItem('CutDPhill',  'abs([DPhill]) < 2.8415926')]
    
    return cuts

#========================================================================================================
# Muon cuts
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
def getMuonProbeCuts(key):

    cuts  = []
    cuts += [CutItem('CutEta',      'fabs([Eta]) < 2.5')]
    cuts += [CutItem('CutPID',      'Medium == 1')]
    cuts += [CutItem('CutZ0Sin',    'fabs([Z0Sin]) < 0.5')]
    cuts += [CutItem('CutD0Sig',    'fabs([D0Sig]) < 3.0')]

    return cuts

#========================================================================================================
def getMuonProbeLooseCuts(key):

    cuts  = []
    cuts += [CutItem('CutEta',      'fabs([Eta]) < 2.5')]
    cuts += [CutItem('CutType',     '[muonType] < 2')]

    return cuts

#========================================================================================================
def getMuonTagCuts(key):

    cuts  = []
    cuts += [CutItem('CutPt',       'Pt  > 27.0e3')]
    cuts += [CutItem('CutEta',      'fabs([Eta]) < 2.5')]
    cuts += [CutItem('CutPID',      'Medium == 1')]
    cuts += [CutItem('CutZ0Sin',    'fabs([Z0Sin]) < 1.0')]
    cuts += [CutItem('CutD0Sig',    'fabs([D0Sig]) < 5.0')]

    return cuts

#========================================================================================================
def getMuonCosmicCuts(key):

    cuts  = []
    cuts += [CutItem('CutPt',       'Pt  > 10.0e3')]
    cuts += [CutItem('CutEta',      'fabs([Eta]) < 2.5')]
    cuts += [CutItem('CutPrec',     'numberOfPrecisionLayers > 2.5')]
    cuts += [CutItem('CutInBounds', 'NGasGapInBoundsGeoCut > 0')]

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

#========================================================================================================
# Trigger cuts
#========================================================================================================
def getMuonTriggers(year):

    trigger_muon_2015 = ['HLT_mu20_iloose_L1MU15', 
                         'HLT_mu50']

    trigger_muon_2016 = ['HLT_mu26_ivarmedium', 
                         'HLT_mu50']

    trigger_muon_2017 = ['HLT_mu26_ivarmedium', 
                         'HLT_mu50']
    
    if year == 2015: return trigger_muon_2015
    if year == 2016: return trigger_muon_2016
    if year == 2017: return trigger_muon_2017

    raise Exception('getMuonTriggers - failed to find triggers for year="%s"' %year)

#========================================================================================================
def getMuonTriggerCuts(year=2017, match=''):

    trigger_cuts = CutItem('CutTrigger')

    for trigger in getMuonTriggers(year):
        cut = CutItem('Cut%s' %trigger, '%s%s == 1' %(match, trigger))
        trigger_cuts.AddCut(cut, 'OR')

    return [trigger_cuts]

#=========================================================================================================
def getNonMuonTriggers(year):
   
    non_trigger_muon_2017 = ['HLT_xe110_pufit_L1XE50',
                             'HLT_xe110_pufit_L1XE55',
                             'HLT_xe110_pufit_L1XE60']

    non_trigger_muon_2018 = ['HLT_xe100_pufit_xe75_L1XE50',
                             'HLT_xe100_pufit_xe75_L1XE55',
                             'HLT_xe110_pufit_xe65_L1XE50']

    if year == 2017: return non_trigger_muon_2017
    if year == 2018: return non_trigger_muon_2018

    raise Exception('getNonMuonTriggers - failed to find triggers for year="%s"' %year)

#=========================================================================================================
def getNonMuonTriggerCuts(year=2017, match=''):

    cut_str = ''

    for trigger in getNonMuonTriggers(year):

        if len(cut_str) == 0:
            cut_str +=     '([%s] == 1)' %trigger
        else:
            cut_str += ' || ([%s] == 1)' %trigger

    return [CutItem('CutUnbiasTrig', cut_str)]

#=======================================================================================================
def registerTrigger(ROOT, triggerListFile=''):

    if not triggerListFile or not os.path.exists(triggerListFile): 
        raise Exception('registerTrigger - failed to find trigger list file: "%s"' %triggerListFile)

    triggers_list = [line.strip() for line in open(triggerListFile, 'r')]

    for trigger in triggers_list:
        ROOT.Anp.Var.RegisterDynamicVar(trigger)
