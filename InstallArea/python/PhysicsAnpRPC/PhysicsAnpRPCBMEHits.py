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
def prepareJobConfig(ROOT, options, files):

    clog.info('prepareJobConfig - prepare python configuration...')

    #-------------------------------------------------------------------------
    # Configure ntuple processing helper algorithms and candidate maker(s)
    #
    prep_algs = [getPrepBmeHit ('prepBmeHit',  options),
                 getPrepBmeList('prepBmeList', options)]
    plot_algs = []

    #-------------------------------------------------------------------------
    # Configure top level algorithms
    #
    top_algs  = prep_algs
    top_algs += plot_algs

    addPrintEvent('printEvent', options, top_algs)
    
    #-------------------------------------------------------------------------
    # Prepare and configure top level C++ module
    #
    run = prepareReadModule(ROOT, options, files, top_algs)

    clog.info('prepareJobConfig - process up to %d events' %options.nevent)
    clog.info('prepareJobConfig - all done')

    return run

#========================================================================================================
# Top level C++ module to read ntuples and configure and run algs
#========================================================================================================
def prepareReadModule(ROOT, options, files, top_algs):

    run = config.prepareReadNtuple(ROOT, options, files, hist_config=['PhysicsAnpRPC/config/rpc-eff'])

    if options.trees != None:
        run.SetKey('TreeName', options.trees)
    else:
        run.SetKey('TreeName', 'TDC')

    var_nicks = ['evtNum64:Event',
                 'runNum:Run',
                 'TDC:rpcTDC']

    var_vetos = []
    vec_vetos = []

    run.SetKey('VarNicks',    ','.join(var_nicks))
    run.SetKey('VetoVars',    ','.join(var_vetos))
    run.SetKey('VetoVecs',    ','.join(vec_vetos))

    run.SetKey('Print',            'yes')
    run.SetKey('Debug',            options.debug_run or options.debug)
    run.SetKey('DebugVars',        'no')
    run.SetKey('NPrint',           100000)
    run.SetKey('NEventPerFile',    0)
    run.SetKey('FillTrueParts',    'no')
    run.SetKey('FillAllTrueParts', 'no')

    run.SetKey('Lists', 'RPC_')
    
    for f in files:
        run.StoreInputFile(f)

    if options.print_vars:
        ROOT.Anp.Var.PrintAllVars()   

    run.AddTopAlg('topAlg', top_algs)

    if options.debug_all:
        run.SetKey('Debug', 'yes')

    return run

#========================================================================================================
# Analysis algorithm configurations
#========================================================================================================
def getPrepBmeHit(name, options):

    alg = physicsBase.AlgConfig(name, 'PrepBmeHit')
    alg.SetKey('Debug',                 options.debug)
    alg.SetKey('Print',                 'yes')
    alg.SetKey('BmeHitVecNameIn',       'RPC_')

    return alg

#========================================================================================================
def getPrepBmeList(name, options):

    alg = physicsBase.AlgConfig(name, 'PrepBmeList')
    alg.SetKey('Debug',                 'no')
    alg.SetKey('Print',                 'yes')
    alg.SetKey('BmeListFileName',       'bme_list.txt')

    return alg

#========================================================================================================
def addPrintEvent(name, options, top_algs):

    if not options.print_reco_event and not options.print_cand_event:
        return

    alg = physicsBase.AlgConfig(name, 'PrintEvent')
    alg.SetKey('PrintRecoEvent',    options.print_reco_event)
    alg.SetKey('PrintCandEvent',    options.print_cand_event)
    alg.SetKey('PrintHits',         'yes')
    alg.SetKey('PrintTruth',        'yes')

    top_algs += [alg]
