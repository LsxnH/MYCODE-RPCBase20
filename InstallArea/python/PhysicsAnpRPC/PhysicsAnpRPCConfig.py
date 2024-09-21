
import logging
import os
import re
import sys

import PhysicsAnpBase.PhysicsAnpBaseConfig as physicsBase
import PhysicsAnpRPC .PhysicsAnpRPCUtils   as Utils

options = None

#========================================================================================================
def prepareOptionParser():
    
    from optparse import OptionParser
    
    p = OptionParser()

    p.add_option('--dirs-key',          type='string', default=None,               dest='dirskey')
    p.add_option('--file-key',          type='string', default='.root$',           dest='filekey')
    p.add_option('--data-key',          type='string', default='data_[0-9]+.root', dest='datakey')    
    p.add_option('--output', '-o',      type='string', default=None,               dest='output')
    p.add_option('--trees',             type='string', default=None,               dest='trees')
    p.add_option('--save-input-files',  type='string', default=None)
    p.add_option('--bme-event-map',     type='string', default='')
    p.add_option('--bme-geo',           type='string', default='')
    p.add_option('--rpc-geo',           type='string', default=None)
    p.add_option('--out-geo',           type='string', default='')
    p.add_option('--debug-alg',         type='string', default=None)
    p.add_option('--print-alg',         type='string', default=None)

    p.add_option('--nevent', '-n',      type='int',    default=0)
    p.add_option('--nprint',            type='int',    default=10000)
    p.add_option('--min-lb',            type='int',    default=None)
    p.add_option('--max-lb',            type='int',    default=None)
    p.add_option('--lumi',              type='float',  default=20280.2)

    p.add_option('--batch', '-b',        action='store_true',  default=False, dest='batch')
    p.add_option('--debug', '-d',        action='store_true',  default=False, dest='debug')
    p.add_option('--debug-all',          action='store_true',  default=False, dest='debug_all')
    p.add_option('--debug-run',          action='store_true',  default=False, dest='debug_run')
    p.add_option('--debug-prep',         action='store_true',  default=False, dest='debug_prep')
    p.add_option('--debug-reco',         action='store_true',  default=False, dest='debug_reco')
    p.add_option('--print-vars',         action='store_true',  default=False, dest='print_vars')
    p.add_option('--print-cuts',         action='store_true',  default=False, dest='print_cuts')
    p.add_option('--print-true',         action='store_true',  default=False, dest='print_true')
    p.add_option('--print-reco-event',   action='store_true',  default=False, dest='print_reco_event')
    p.add_option('--print-cand-event',   action='store_true',  default=False, dest='print_cand_event')
    p.add_option('--print-rpc',          action='store_true',  default=False, dest='print_rpc')

    p.add_option('--test', '-t',         action='store_true',  default=False, dest='test')    
    p.add_option('--do-bme',             action='store_true',  default=False, dest='do_bme')
    p.add_option('--do-cosmic',          action='store_true',  default=False, dest='do_cosmic')
    p.add_option('--dry-run',            action='store_true',  default=False, dest='dryrun')
    p.add_option('--draw',               action='store_true',  default=False, dest='draw')
    p.add_option('--write',              action='store_true',  default=False, dest='write')

    p.add_option('--do-extrID',          action='store_true',  default=False, dest='do_extrID')
    p.add_option('--do-unbias',          action='store_true',  default=False, dest='do_unbias')
    p.add_option('--do-trigmu',          action='store_true',  default=False, dest='do_trigmu')
    p.add_option('--do-rpc-muon',        action='store_true',  default=False, dest='do_rpc_muon')
    p.add_option('--do-rpc-ntuple',      action='store_true',  default=False, dest='do_rpc_ntuple')
    p.add_option('--do-clu-ntuple',      action='store_true',  default=False, dest='do_clu_ntuple')
    p.add_option('--do-noise',           action='store_true',  default=False, dest='do_noise')
    p.add_option('--do-dphill-cut',      action='store_true',  default=False, dest='do_dphill_cut')
    p.add_option('--do-trig',            action='store_true',  default=False, dest='do_trig')
    p.add_option('--do-save-all-hit',    action='store_true',  default=False)
    p.add_option('--small-noise-hists',  action='store_true',  default=False)
    p.add_option('--do-fake-trig',       action='store_true',  default=False, dest='do_fake_trig')

    global options
    options = p.parse_args()[0]
    
    return p

#========================================================================================================
clog = physicsBase.getLog(os.path.basename(__file__))

load_libs = None

#========================================================================================================
def loadPhysicsAnpRPCLibs(ROOT):

    global load_libs

    if load_libs:
        return

    clog.info('loadPhysicsAnpRPCLibs - load shared libraries')

    from ROOT import Anp

    Utils.setPlotDefaults(ROOT)
    
    ROOT.gSystem.Load('libPhysicsAnpData')
    ROOT.gSystem.Load('libPhysicsAnpDataDict')
    ROOT.gSystem.Load('libPhysicsAnpBase')
    ROOT.gSystem.Load('libPhysicsAnpBaseDict')
    ROOT.gSystem.Load('libPhysicsAnpRPC')
    ROOT.gSystem.Load('libPhysicsAnpRPCDict')

    #
    # ROOT hack to load dictionary classes
    #
    load_libs = ROOT.Anp.ReadNtuple()

#========================================================================================================
def prepareReadNtuple(ROOT, options, tree, hist_config=None):
    
    ROOT.gROOT.SetBatch(True)

    run = physicsBase.ReadNtuple('readNtuple')
    run.SetKey('TreeName',       tree)
    run.SetKey('PrintFiles',     'no')
    run.SetKey('NPrint',         options.nprint)
    run.SetKey('NEvent',         options.nevent)
    run.SetKey('Debug',          options.debug_run or options.debug_all)
    run.SetKey('FileKeys',       '.root')
    run.SetKey('CloseFile',      'yes')
    run.SetKey('Print',          'yes')
    run.SetPar('HistMan::Debug', 'no')
    run.SetPar('HistMan::Sumw2', 'yes')
    
    if options.output:
        run.SetKey('OutputFile', options.output)
        
    #
    # Read histogram definitions
    #
    histman  = ROOT.Anp.HistMan.Instance()
    histdirs = []
    
    testArea   = os.environ.get('TestArea')
    sourceArea = os.environ.get('SourceArea')

    if type(hist_config) == type([]):
        for hconfig in hist_config:
            if os.path.isdir(hconfig):
                histdirs += [hconfig]
                continue

            histPath   = '%s/%s/' %(testArea,   hconfig.rstrip('/'))
            sourcePath = '%s/%s/' %(sourceArea, hconfig.rstrip('/'))
            
            if os.path.isdir(histPath):
                histdirs += [histPath]
            elif os.path.isdir(sourcePath):
                histdirs += [sourcePath]
            else:
                clog.warning('prepareReadNtuple - invalid paths: "%s" and "%s"' %(histPath, sourcePath))

    for hdir in histdirs:
        clog.info('prepareReadNtuple - read xml files from: %s' %hdir)                
        for f in os.listdir(hdir):
            if f.count('.xml'):
                histman.ReadFile('%s/%s' %(hdir.rstrip('/'), f))

    return run

#========================================================================================================
def getCutValue(cuts, name, default):

    if type(cuts) != dict:
        return default

    try:
        return cuts[name]
    except KeyError:
        return default   

#========================================================================================================
def getRunChain(name, algs=None, cuts=None, options=None):
    
    alg = physicsBase.AlgConfig(name, 'RunChain')

    if type(cuts) == type([]):
        physicsBase.addCuts(alg, 'CutCand', cuts)
    
    if type(algs) == type([]) and len(algs):
        alg.AddAlg(algs)

    return alg

#========================================================================================================
def addPrintEvent(name, options, top_algs):

    if not options.print_reco_event and not options.print_cand_event:
        return

    alg = physicsBase.AlgConfig(name, 'PrintEvent')
    alg.SetKey('PrintRecoEvent',    options.print_reco_event)
    alg.SetKey('PrintCandEvent',    options.print_cand_event)
    alg.SetKey('PrintHits',         'yes')
    alg.SetKey('PrintTruth',        'no')

    top_algs += [alg]

#========================================================================================================
# Read and prepare RPC geometry from dedicated ROOT file
#========================================================================================================
def getPrepRpcGeo(name, options, ROOT):

    run = prepareReadNtuple(ROOT, options, [], hist_config=[])
    run.SetKey('AlgName',          'readGeoNtuple')
    run.SetKey('OutputFile',       '')
    run.SetKey('Print',            'yes')
    run.SetKey('Debug',            'no')
    run.SetKey('DebugVars',        'no')
    run.SetKey('NPrint',           10000000)
    run.SetKey('NEventPerFile',    0)
    run.SetKey('FillTrueParts',    'no')
    run.SetKey('FillAllTrueParts', 'no')

    run.SetKey('NtupleInstance',        'rpc_geometry')
    run.SetKey('Lists',                 'm_rpc_geo_')
    run.SetKey('GroupKeys::m_rpc_geo_', 'phiStrip_, etaStrip_')

    run.AddTopAlg('topAlg', [])

    alg = physicsBase.AlgConfig(name, 'PrepRpcGeo')
    alg.SetKey('Debug',                 'no')
    alg.SetKey('Print',                 'yes')
    alg.SetKey('TreeName',              'geometry')
    alg.SetKey('OutGeoStripFilePath',   options.out_geo)

    alg.SetKey('GeoNtupleInstance',     'rpc_geometry')
    alg.SetKey('GeoVecGapName',         'm_rpc_geo_')
    alg.SetKey('GeoVecEtaStripName',    'etaStrip_')
    alg.SetKey('GeoVecPhiStripName',    'phiStrip_')
    alg.SetKey('ReadRegistry',          run.GetRegistryConfig())

    #
    # Choose best geometry file
    #
    geo_filepath = options.rpc_geo

    if os.path.isdir('/afs/cern.ch/user/a/arpcpadm/public'):
        geo_filepath = '/afs/cern.ch/user/a/arpcpadm/public/OfflineGeometry/rpc_geometry_athena_21.0.71_2018-09-12.root'
    elif os.path.isdir('/lustre/AtlUser/rustem/public/OfflineGeometry'):
        geo_filepath = '/lustre/AtlUser/rustem/public/OfflineGeometry/rpc_geometry_athena_21.0.71_2018-09-12.root'
                        
    if geo_filepath and os.path.isfile(geo_filepath):
        alg.SetKey('FilePath', geo_filepath)
    else:
        raise Exception('getPrepRpcGeo - failed to determine geometry file location: "%s"' %geo_filepath)

    return alg

#========================================================================================================
# Configure event preparation algorithms
#========================================================================================================
def getPrepReco(name, options):

    alg = physicsBase.AlgConfig(name, 'PrepReco')
    alg.SetKey('Debug',         options.debug)
    alg.SetKey('DebugVars',     'no')
    alg.SetKey('Print',         'yes')
    alg.SetKey('AddLeptonMass', 'yes')
    
    return alg
