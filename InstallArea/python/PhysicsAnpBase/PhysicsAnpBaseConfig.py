
import logging
import os
import re
import sys

loggers = {}
    
#========================================================================================================
def getLog(name, level='INFO', debug=False, print_time=False):

    global loggers

    if name in loggers:
        return loggers[name]

    if print_time:
        f = logging.Formatter('%(asctime)s - %(name)s: %(levelname)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
    else:
        f = logging.Formatter('%(name)s: %(levelname)s - %(message)s')
        
    h = logging.StreamHandler(sys.stdout)
    h.setFormatter(f)
    
    log = logging.getLogger(name)
    log.addHandler(h)
    
    if debug:
        log.setLevel(logging.DEBUG)
    else:
        if level == 'DEBUG':   log.setLevel(logging.DEBUG)
        if level == 'INFO':    log.setLevel(logging.INFO)
        if level == 'WARNING': log.setLevel(logging.WARNING)    
        if level == 'ERROR':   log.setLevel(logging.ERROR)

    return log

#========================================================================================================
clog = getLog(os.path.basename(__file__))

#========================================================================================================
def setRegistryKey(reg, key, value):
    """ Module helper function """

    if type(value) == bool:
        if value: reg.SetVal(key, 'yes')
        else:     reg.SetVal(key, 'no')
    elif type(value) == int or type(value) == long:
        reg.SetValueLong  (key, '%d' %value)
    elif type(value) == float:
        reg.SetValueDouble(key, '%12g' %value)
    elif type(value) == str:
        reg.SetVal(key, value)
    elif type(value) == type(reg):
        reg.SetVal(key, value)
    elif type(value) == type([]):
        vs = ''
        for i in range(0, len(value)):
            v = value[i]
            
            if i+1 < len(value):
                vs = vs + '%s,' %v
            else:
                vs = vs + '%s'  %v
                
        reg.SetVal(key, vs)
    elif hasattr(value, 'GetConfigRegistry'):
        reg.SetVal(key, value.GetConfigRegistry())
    else:
        raise Exception('setRegistryKey - unknown type for %s: value=%s, type(value)=%s' %(key, value, type(value)))

#========================================================================================================
def getRegistry():
    import ROOT
    from ROOT import Anp

    return ROOT.Anp.Registry()

#========================================================================================================
class AlgConfig:
    """AlgConfig - python configurable for C++ Anp::AlgEvent class"""
        
    def __init__(self, alg_name, alg_type):
        self._alg_name = alg_name
        self._alg_type = alg_type
        self._algs     = []
        self._keys     = {}
        self._target   = ''
        self._reg      = getRegistry()        
        self._log      = getLog(alg_name)
        
    def Print(self):
        self._log.info('AlgConfig: '+self.GetAlgName())

    def GetAlgName(self): return self._alg_name
    def GetAlgType(self): return self._alg_type

    def GetTarget(self):
        return self._target

    def SetTarget(self, target):
        self._target = target

    def GetConfigRegistry(self):
        reg = getRegistry()
        reg.Merge(self._reg)

        setRegistryKey(reg, 'AlgType',   self._alg_type)
        setRegistryKey(reg, 'AlgName',   self._alg_name)

        alg_list = ''
        for alg in self._algs:
            alg_list += alg[0] + ', '
            key_ = alg[0]
            
            if not reg.KeyExists(key_):
                setRegistryKey(reg, key_, alg[1].GetConfigRegistry())
            else:
                self._log.error('GetConfigRegistry - AlgEvent key already exists: %s' %key_)
                sys.exit(1)

        setRegistryKey(reg, 'AlgList', alg_list)

        for key, val in self._keys.iteritems():
            
            if not reg.KeyExists(key):
                setRegistryKey(reg, key, val.GetConfigRegistry())
            else:
                self._log.warning('GetConfigRegistry - SelectKey key already exists: %s' %key_)
        
        return reg

    def SetAlg(self, key, alg):

        if type(alg) != type(self):
            self._log.warning('SetAlg - %s: wrong algorithm type for key=%s: %s != %s' %(self.GetAlgName(), key, type(alg), type(self)))
            return
        
        if self.GetAlg(key) == None:
            self._algs.append([key, alg])
        else:
            self._log.warning('SetAlg - error: key already exists: %s' %key)            

    def AddAlg(self, alg):

        if type(alg) == type([]):
            for inalg in alg:
                self.AddAlg(inalg)
        elif hasattr(alg, 'GetAlgName'):
            self.SetAlg(alg.GetAlgName(), alg)
        else:
            self._log.warning('AddAlg - unknown algorithm type: %s' %str(type(alg)))
            
    def SetKey(self, key, value):
        setRegistryKey(self._reg, key, value)

    def SetPar(self, key, value):
        setRegistryKey(self._reg, key, value)

    def SetGlobalPar(self, key, value):
        setRegistryKey(self._reg, key, value)
	
        for myalg in self._algs:
            myalg[1].SetGlobalPar(key, value)

    def DelKey(self, key):
        if self._reg.KeyExists(key):
            self._reg.RemoveKey(key)
            
    def DelPar(self, key):

        if self._reg.KeyExists(key):
            self._reg.RemoveKey(key)

    def GetAlg(self, key):

        for alg in self._algs:
            if alg[0] == key:
                return alg[1]

        return None

    def AddCuts(self, key, cuts):
        addCuts(self, key, cuts)

    def GetSelectKey(self, key, key_type):

        if key_type != 'AND' and key_type != 'OR':
            self._log.warning('GetSelectKey(%s, %s) - unknown type key' %(key, key_type))
            return None

        if key not in self._keys:
            self._keys[key] = SelectKey(key, key_type)

        return self._keys[key]

    def AddSelectKey(self, key, key_type, selection, decision=True):

        if key == None:
            return None

        if type(key) != type(''):
            self._log.warning('AddSelectKey - bad key typy %s for key=%s' %(type(key), key))
            return None

        if key not in self._keys:
            self._keys[key] = self.GetSelectKey(key, key_type)
        else:
            if key_type != self._keys[key].GetType():
                self._log.warning('AddSelectKey - key type mismatch: %s!=%s for key=%s' %(key_type, self._keys[key].GetType(), key))
                return None

        self._keys[key].AddSelectKey(selection, decision)

    def AddSelectKeyObject(self, key):

        if key == None:
            return None
        if not hasattr(key, 'GetKeyName'):
            clog.warning('AddSelectKeyObject - key has wrong type')
            return None
        if key.GetKeyName() in self._keys:
            clog.warning('AddSelectKeyObject - key already exists: %s' %key.GetKeyName())
            return None
        
        self._keys[key.GetKeyName()] = key

#========================================================================================================
class ReadNtuple:
    """ReadNtuple - python configuration for ReadNtuple to read input files and run algorithms
    - configure input files
    - configure algorithms
    - configure histograms
    """

    def __init__(self, alg_name):
        import ROOT
        
        self._alg   = None
        self._reg   = getRegistry()
        self._run   = ROOT.Anp.ReadNtuple()
        self._files = []
        self._dirs  = []        
        self._log   = getLog('ReadNtuple')        
        self._hist  = getRegistry()
        self._hist.AllowNonUniqueKeys()
        self._gpar  = {}

        self.SetPar('AlgName', alg_name)
        self.SetPar('AlgType', 'ReadNtuple')
        
    def Print(self):
        clog.info('ReadNtuple: print children algorithms:')
        self._alg.Print()

    #
    # Functions for python configuration
    #
    def SetKey(self, key, value):
        setRegistryKey(self._reg, key, value)

    def SetPar(self, key, value):
        setRegistryKey(self._reg, key, value)

    def SetGlobalPar(self, key, value):
        setRegistryKey(self._reg, key, value)
        
        if self._alg:
            self._alg.SetGlobalPar(key, value)
        else:
            self._gpar[key] = value

    def SetTopAlg(self, alg):
        if self._alg != None:
            self._log.warning('SetTopAlg - top algorithm is already configured')
            return
        
        self._alg = alg

        for key, value in self._gpar.iteritems():
            self._alg.SetGlobalPar(key, value)
            
        self._gpar = {}

    def AddTopAlg(self, alg_name, alg_list, Print="yes"):
        if self._alg != None:
            self._log.warning('AddTopAlg - Top algorithm is already configured')
            return
        
        self._alg = AlgConfig(alg_name, 'RunAlgs')
        self._alg.SetKey('Print', Print)
        self._alg.AddAlg(alg_list)

        for key, value in self._gpar.iteritems():
            self._alg.SetGlobalPar(key, value)
        self._gpar = {}

    def AddHistFile(self, hfile):
        self._hist.Set('ReadFile', hfile)

    def StoreInputFile(self, f):
        self._log.debug('StoreInputFile: %s' %f)
        self._files += [f]
                    
    def GetRegistryConfig(self):
        if type(self._alg) == None:
            self._log.warning('Missing top algorithm')
            return None

        reg = getRegistry()
        
        reg.Merge(self._reg)

        if self._alg != None:
            setRegistryKey(reg, 'SubAlgType', self._alg.GetAlgType())
            setRegistryKey(reg, 'SubAlgName', self._alg.GetAlgName())
            setRegistryKey(reg, self._alg.GetAlgName(),   self._alg.GetConfigRegistry())
            
        setRegistryKey(reg, 'HistMan', self._hist)

        input_reg = getRegistry()
        input_reg.AllowNonUniqueKeys()
        
        for file in self._files:
            setRegistryKey(input_reg, 'File', file)
            
        setRegistryKey(reg, 'InputFiles', input_reg)
        return reg
    
    #
    # Wrappers for C++ code: ReadNtuple member functions
    #
    def Execute(self, reg_path):
        self._run.Execute(reg_path)

    def ExecuteRegistry(self):
        self._run.ExecuteRegistry(self.GetRegistryConfig())

#========================================================================================================
class RunModule:
    """RunModule - python configuration interface for C++ RunModule
    - configure input files
    - configure algorithms
    - configure histograms
    """

    def __init__(self):
        import ROOT
        
        self._alg   = None
        self._reg   = getRegistry()
        self._run   = ROOT.Anp.RunModule()
        self._files = []
        self._dirs  = []        
        self._log   = getLog('RunModule')        
        self._hist  = getRegistry()
        self._hist.AllowNonUniqueKeys()
        self._gpar  = {}
        
    def Print(self):
        clog.info('RunModule: print children algorithms:')
        self._alg.Print()

    #========================================================================================================
    #
    # Functions for python configuration
    #
    def SetKey(self, key, value):
        setRegistryKey(self._reg, key, value)

    def SetPar(self, key, value):
        setRegistryKey(self._reg, key, value)

    def SetGlobalPar(self, key, value):
        setRegistryKey(self._reg, key, value)
        
        if self._alg:
            self._alg.SetGlobalPar(key, value)
        else:
            self._gpar[key] = value

    def SetTopAlg(self, alg):
        if self._alg != None:
            self._log.warning('SetTopAlg - top algorithm is already configured')
            return
        
        self._alg = alg

        for key, value in self._gpar.iteritems():
            self._alg.SetGlobalPar(key, value)
            
        self._gpar = {}

    def AddTopAlg(self, alg_name, alg_list, Print="yes"):
        if self._alg != None:
            self._log.warning('AddTopAlg - Top algorithm is already configured')
            return
        
        self._alg = AlgConfig(alg_name, 'RunAlgs')
        self._alg.SetKey('Print', Print)
        self._alg.AddAlg(alg_list)

        for key, value in self._gpar.iteritems():
            self._alg.SetGlobalPar(key, value)
        self._gpar = {}

    def AddHistFile(self, hfile):
        self._hist.Set('ReadFile', hfile)

    def StoreInputFile(self, file):
        self._log.debug('StoreInputFile: '+file)
        self._files += [file]

    def AddInputDir(self, target, key):
        #
        # Search "target" for files with name matching "key"
        #
        files = SearchInputDir(target, key)
        for f in files: self.AddInputFile(f)

    def StoreInputDir(self, target, key, option = ''):
        #
        # Search "target" for files with name matching "key"
        #
        files = SearchInputDir(target, key, option)
        for f in files: self._files += [f]

    def StoreRecursiveDir(self, target, key):
        #
        # Search "target" for files with name matching "key"
        #
        files = SearchRecursiveDir(target, key)
        for f in files: self._files += [f]
                    
    def GetRegistryConfig(self):
        if type(self._alg) == None:
            self._log.warning('Missing top algorithm')
            return None

        reg = getRegistry()
        
        reg.Merge(self._reg)        
        setRegistryKey(reg, 'AlgType', self._alg.GetAlgType())
        setRegistryKey(reg, 'AlgName', self._alg.GetAlgName())
        setRegistryKey(reg, 'HistMan', self._hist)
        setRegistryKey(reg, self._alg.GetAlgName(), self._alg.GetConfigRegistry())

        input_reg = getRegistry()
        input_reg.AllowNonUniqueKeys()
        
        for f in self._files:
            setRegistryKey(input_reg, 'File', f)
            
        setRegistryKey(reg, 'InputFiles', input_reg)
        return reg

    def ClearStoredFiles(self):
        self._files = []

    def GetStoredFiles(self):
        return self._files
    
    #========================================================================================================
    #
    # Wrappers for C++ code: RunModule member functions
    #
    def Config(self, reg = None):
        if reg == None:
            self._run.Config(self.GetRegistryConfig())
        else:
            self._run.Config(reg)

    def Init(self):
        self._run.Init()

    def Exec(self):
        self._run.Exec()

    def Execute(self, reg_path):
        self._run.Execute(reg_path)

    def Done(self):
        self._run.Done()

    def AddInputFile(self, file):
        self._run.AddInputFile(file)

    def ClearInputFiles(self):
        self._run.ClearInputFiles()

#========================================================================================================
class CutItem:
    """CutItem - python configuration wrapper for C++ CutItem class"""
        
    def __init__(self, name, conf='', abs=False, dummy=False, debug=None):

        self.cut_name  = name
        self.cut_conf  = conf
        self.cut_reg   = getRegistry()
        self.list_and  = []
        self.list_or   = []
        self.abs       = abs

        if name.count(' '):
            raise Exception('CutItem - name="%s" must not include space character' %name)

        if conf.count('[') != conf.count(']'):
            raise Exception('CutItem - [] brackets do not match: "%s"' %conf)

        if conf.count('(') != conf.count(')'):
            raise Exception('CutItem - () brackets do not match: "%s"' %conf)         

        self.cut_reg.SetVal('CutName',   self.cut_name)
        self.cut_reg.SetVal('CutConf',   self.cut_conf)

        if abs:
            self.cut_reg.SetVal('CutUseAbs', 'yes')
        else:
            self.cut_reg.SetVal('CutUseAbs', 'no')
        
        if dummy:
            self.cut_reg.SetVal('CutDummy', 'yes')

        if type(debug) == type(True):
            self.cut_reg.SetVal('Debug', debug)
        
    def IsValidConf(self, conf):
        
        if type(conf) != type(''):
            raise TypeError('CutItem::IsValidConf - invalid input type: %s' %conf)

        #
        # Lists of known operators
        #        
        oprs = ['>=', '>', '<=', '<', '==', '!=']

        #
        # Split using logical AND=&& or OR==|| and check that expressions are valid
        #
        if conf.count('&&') and conf.count('||'):
            raise TypeError('CutItem - conf="%s" can not contain both && and || operators' %conf)
        elif conf.count('||'):
            confs = conf.split('||')
        else:
            confs = conf.split('&&')

        for v in confs:
            if len(v) == 0:
                continue

            for o in oprs:
                if v.find(o) > 0:
                    break
                o = None
                
            if o == None:
                raise TypeError('CutItem::IsValidConf - "%s" does not contain known operator' %conf)

            parts = v.split(o)

            if len(parts) != 2:
                raise TypeError('CutItem - conf="%s" has invalid logic %s: %s, %s' %(conf, v, o, parts))

            var_name = parts[0]
            var_comp = parts[1]

            try:
                var_comp = float(var_comp)
            except ValueError:
                log.error('CutItem - conf="%s" contains comparison with non-number' %conf)
                sys.exit(1)
                
            #
            # Logical expression is valid and contains only known variables
            #
            log.debug('CutItem - conf="%s": %s %s %f' %(conf, var_name, o, var_comp))         

        return True

    def GetRegistry(self):
        return self.cut_reg

    def GetCutName(self):
        return self.cut_name

    def GetCutConf(self):
        return self.cut_conf

    def AddCuts(self, cuts, option):
        
        for cut in cuts:
            self.AddCut(cut, option)

    def AddCut(self, cut, option):

        if cut.GetCutName() in self.list_and + self.list_or:
            raise Exception('AddCut - duplicate cut: %s' %cut.GetCutName())
            
        if   option.lower() == 'and': self.list_and += [cut.GetCutName()]
        elif option.lower() == 'or' : self.list_or  += [cut.GetCutName()]
        else: 
            raise Exception('AddCut - unknown option %s: %s' %(cut.GetCutName(), option))

        self.cut_reg.SetVal(cut.GetCutName(), cut.GetRegistry())
        self.cut_reg.SetVal('ListAND',        ','.join(self.list_and))
        self.cut_reg.SetVal('ListOR',         ','.join(self.list_or))

        if len(self.list_and) > 0 and len(self.list_or) > 0:
            raise Exception('AddCut - both AND and OR are defined - logic error: %s' %cut.GetCutName())

        clog.debug('AddCut - %s %s'%(cut.GetCutName(),option) )
        
        return True

    def CutAsStr(self):
        text  = '%s: %s' %(self.cut_name, self.cut_conf)

        return text

    def __repr__(self):
        return self.CutAsStr()

#========================================================================================================
def getCutsAsLatex(cuts, outfile=None):

    if cuts == None:
        return ''

    text  = '\\resizebox{1.0\\textwidth}{!}{\n'
    text += '\\begin{tabular}{|l|l|}\n'
    text += '\\hline\n'
    text += 'Name & Definition\\\\\n'
    text += '\\hline\n'

    nwid = 4

    for cut in cuts:
        cut_name = getattr(cut, 'GetCutName', None)

        if cut_name:
            nwid = max([nwid, len(cut_name())])
    
    for cut in cuts:
        cut_name = getattr(cut, 'GetCutName', None)
        cut_conf = getattr(cut, 'GetRawConf', None)

        if cut_name and cut_conf:
            text += '%s & $%s$\\\\\n' %(cut_name().ljust(nwid), cut_conf().replace('&', '\\&'))

    text += '\\hline\n'
    text += '\\end{tabular}}\n'

    for c, n in getCutVarNames().iteritems():
        text = text.replace(c, n)

    if outfile:
        ofile = open(outfile, 'w')
        ofile.write(text)
        ofile.close()

    return text

#========================================================================================================
def addCuts(alg, key, cuts, save=False):

    if alg == None:
        raise Exception('addCuts - invalid alg')

    if type(cuts) != type([]):
        raise Exception('addCuts - invalid inputs for %s: %s, %s, %s' %(alg.GetAlgName(), key, cuts, dir(cuts)))

    keys = []
    
    for cut in cuts:
        if cut.GetCutName() in keys:
            raise Exception('addCuts - %s::%s - duplicate cut %s in %s' %(alg.GetAlgName(), key, cut.GetCutName(), keys))

        keys += [cut.GetCutName()]
        alg.SetPar('%s%s'  %(key, cut.GetCutName()), cut.GetRegistry())        

    alg.SetPar(key, ','.join(keys))
    
    if save:
        clog.info('Print cuts: %s' %key)
        print physicsLight.getCutsAsLatex(cuts, 'cuts_%s_%s.tex' %(alg.GetAlgName(), key))

    return cuts

#========================================================================================================
def addCutsToRegistry(reg, key, cuts):

    if reg == None:
        raise Exception('addCutsToRegistry - invalid reg')

    if type(cuts) != type([]):
        raise Exception('addCutsToRegistry - invalid inputs: %s, %s, %s' %(key, cuts, dir(cuts)))

    keys = []
    
    for cut in cuts:
        if cut.GetCutName() in keys:
            raise Exception('addCutsToRegistry - %s - duplicate cut %s in %s' %(key, cut.GetCutName(), keys))

        keys += [cut.GetCutName()]
        setRegistryKey(reg, '%s%s'  %(key, cut.GetCutName()), cut.GetRegistry())

    setRegistryKey(reg, key, ','.join(keys))

    return cuts

#========================================================================================================
def getAlgName(alg):
    
    if hasattr(alg, 'GetAlgName'):
        return alg.GetAlgName()
    elif type(alg) == str:
        return alg
    
    raise Exception('getAlgNam - wrong cand object type="%s" - exit now' %type(alg))
