
import logging
import os
import re
import sys

from PhysicsAnpBase.PhysicsAnpBaseConfig import getLog

#========================================================================================================
clog = getLog(os.path.basename(__file__))

#========================================================================================================
def findInputLocalFiles(paths, filekey='.*$.root', dirkey=None, save_files=None, print_files=True):

    """    
    Top level function for recursive directory search:
      - include files that match filekey pattern
      - descend directory that matches dirkey pattern
    """

    if type(paths) != type([]) or len(paths) < 1:
        clog.warning('findInputLocalFiles - missing input path(s)')
        return []

    files = []

    for path in paths:

        parts = path.split(',')
        for ipart in parts:
            clog.info('Search input path: %s' %ipart)
        
            if ipart.count('eos'):
                files += [ipart]
            else:
                files += _processLocalDirectory(ipart, filekey, dirkey)

    if print_files:
        for f in sorted(files):
            clog.info('Input file %8d kB: %s' %(os.path.getsize(f)/1024, f))

    if save_files != None:
        sfile = open(save_files, 'w')

        for f in sorted(files):
            sfile.write('%s\n' %f)

        sfile.close()

    return files

#========================================================================================================
def _processLocalDirectory(path, filekey, dirkey):
    
    """    
    Recursive directory search
    """
    
    files = []

    if os.path.isfile(path):
        if type(filekey) == type(''):
            matchFile = (re.search(filekey, path) != None)
        elif type(filekey) == type([]):
            matchFile = False
            
            for fkey in filekey:
                #
                # AND of multiple keys
                #
                parts = fkey.split(',')
                npass = 0
                
                for part in parts:
                    if (re.search(part, path) != None):
                        npass += 1
                if npass == len(parts) and len(parts) > 0:
                    matchFile = True
                    break
        else:
            matchFile = True
            
        if matchFile:
            files += [path]
        else:
            clog.info('Ignore input file \"%s\" miss matched the --file-key=\"%s\"' %(path, filekey))
            
        return files

    if not os.path.isdir(path):
        clog.error('Invalid path: %s' %path)
        sys.exit(1)

    for file in os.listdir(path):
        
        fpath = path.rstrip('/')+'/'+file

        if file in ['.', '..']:
            continue

        if os.path.isdir(fpath):
            
            if type(dirkey) == type(''):
                matchDir = (re.search(dirkey, fpath) != None)
            else:
                matchDir = True
                
            if matchDir:
                files += _processLocalDirectory(fpath, filekey, dirkey)
        else:
            files += _processLocalDirectory(fpath, filekey, dirkey)

    return files

#========================================================================================================
def SearchInputDir(target, key, option = ""):

    """ Search target for files with name matching key """

    def _matchKey(_val, _key):
        if type(_val) != str:
            return False
        
        if type(_key) == list:
            for i in range(0, len(_key)):
                if type(_key[i]) == str and (len(_key[i]) < 0 or _val.count(_key[i])):
                    return True
        elif type(_key) == str:
            if len(_key) < 0 or _val.count(_key):
                return True
        return False

    files = []

    plog.info('Search directory: '+str(target))
    
    if target.count('castor') :
        files = get_files_from_castor( target, option )
    elif target.count('eos') :
        files = get_files_from_eos( target, option )
    elif target.count('srm') : 
        files = get_files_from_srm( target, option)
    else :
        files = get_files_from_local_dir( target, option )

    ana_files = []
    for ifile in files:            

        if ifile.count('.root') == 0:
            continue
        
        if len(ifile) > 5 and _matchKey(ifile, key):
            ana_files.append(ifile)                
            plog.debug('Select: '+ifile)
        else:
            plog.debug('Ignore: '+ifile)

    plog.info('Number of included input files: '+str(len(ana_files)))

    return ana_files

#========================================================================================================
def get_files_from_castor( input, option ) :
    """ Find files on cern castor """
    
    files = []

    #
    # "input" is likely CASTOR directory: iterate over directory entries
    #
    def _listCastorPath(_path):
        from subprocess import Popen, PIPE
        _output = Popen(['nsls',_path], stdout=PIPE).communicate()[0]
	   
        if len(_output) == 0:
            return []
	       
        return _output.split('\n')
	           
    rpath = 'root://castoratlas/'+input.rstrip('/')
    flist = _listCastorPath(input)
	
    if option.lower().count('deep'):
        clist = flist
        flist = []
        for idir in clist:
            cpath = input.rstrip('/')+'/'+idir.rstrip('/')
	           
            if not _matchKey(cpath+'/'+idir, key):
                plog.debug('Ignore 0: '+cpath+'/'+idir)
                continue
            else:
                plog.debug('Select 0: '+cpath+'/'+idir)                   
	           
            dlist = _listCastorPath(cpath)
	           
            for ifile in dlist:
                flist.append(idir+'/'+ifile)

    files = [ rpath+'/'+ x for x in flist ]

    return files
	   
#========================================================================================================
def get_files_from_srm( input, option ) :
    """ Find files on penn srm """
    
    files = []

    #
    # "target" is a directory on the Penn srm
    #
    modpath = input
    res = re.match('/xrootd(.*)', input) 
    if res is not None :
        modpath = res.group(1)

    from subprocess import Popen, PIPE
    _output = Popen(['xrd', 'hn.at3f', 'dirlist', modpath ], stdout=PIPE).communicate()[0]
    
    if len(_output) == 0:
        return []

    for line in _output.split('\n') :
        if len(line) == 0 :
            continue

        try :
            fname = line.split()[4]
        except IndexError :
            print 'Could not parse output of xrd hn.at3f dirlist'
            continue
    
        if fname.find('root') != -1 :
            files.append('/xrootd'+fname)

    return files


#========================================================================================================
def get_files_from_eos( input, option ) :
    """ Find files on cern eos """

    #
    # "input" is likely eos directory: iterate over directory entries
    #
    def _listCastorPath(_path):
        from subprocess import Popen, PIPE
        eos = '/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select'
        _output = Popen([eos,"ls "+_path], stdout=PIPE).communicate()[0]
    
        if len(_output) == 0:
            return []
        
        return _output.split('\n')
            
    rpath = 'root://eosatlas/'+input.rstrip('/')
    flist = _listCastorPath(input)

    if option.lower().count('deep'):
        clist = flist
        flist = []
        for idir in clist:
            cpath = input.rstrip('/')+'/'+idir.rstrip('/')
            
            if not _matchKey(cpath+'/'+idir, key):
                plog.debug('Ignore 0: '+cpath+'/'+idir)
                continue
            else:
                plog.debug('Select 0: '+cpath+'/'+idir)                    
            
            dlist = _listCastorPath(cpath)
            
            for ifile in dlist:
                flist.append(idir+'/'+ifile)

    files = [ rpath+'/'+ x for x in flist ]

    return files
                
#========================================================================================================
def get_files_from_local_dir( input, option ) :
    """ Find files on a local directory """

    files = []

    if os.path.isfile(input):
        #
        # "input" is physical file
        # 
        if len(key) < 1 or os.path.basename(input).count(key):
            files += [input]
    elif os.path.isdir(input):
        #
        # "input" system directory: iterate over directory entries
        #            
        dlist = os.listdir(input)

        print dlist

        for file in dlist :
            files.append(input+'/'+file)

    return files


#========================================================================================================
def SearchRecursiveDir(target, key):
    """ Search local file system target for files with name matching key """

    import os
    files = []
    slog = getLog('SearchRecursiveDir')

    if len(key) < 3:
        slog.warning('Recursive directory search requires key at least 3 characters long')
        return files
    
    if not os.path.isdir(target):
        return files

    #
    # "target" system directory: iterate over directory entries
    #            
    dlist = os.listdir(target)
            
    for ifile in dlist:
        ipath = target.rstrip('/')+'/'+ifile
        if os.path.isfile(ipath):
            if ifile.count(key):
                files += [ipath]
        else:
            files += SearchRecursiveDir(ipath, key)
            
    slog.info('Number of included input files: '+str(len(files)))
    return files

#========================================================================================================
class Pushd:

    init_dir = None
    orig_dir = None

    def __init__(self, dirname):
        self.init_dir = os.path.realpath(dirname)
        self.orig_dir = None
        
    def __enter__(self):
        self.orig_dir = os.getcwd()
        os.chdir(self.init_dir)
        
        clog.debug('Pushd.__enter__ - CWD=%s' %os.getcwd())
        return self
 
    def __exit__(self, type, value, tb):
        os.chdir(self.orig_dir)
        clog.debug('Pushd.__exit__ - CWD=%s' %os.getcwd())

#========================================================================================================
def copyHistFromInputFiles(hpath, files, outfile):

    '''Copy histograms from the input files to the output file'''

    clog.info('copyHistFromInputFiles - process %d input files' %(len(files)))
    clog.info('copyHistFromInputFiles - copy histogram at path="%s"' %(hpath))
    

    import ROOT
    import os

    hcount = None
    hdir   = os.path.dirname(hpath)

    #
    # Collect Count histograms from input files
    #
    for f in files:        
        rfile = ROOT.TFile.Open(f, 'READ')
        
        if not rfile:
            clog.warning('copyHistFromInputFiles - failed to open: %s' %f)
            continue

        hc = rfile.Get(hpath)

        if not hc:
            clog.warning('copyHistFromInputFiles - missing histogram: %s:Count' %(f))
            continue

        if not hc.Class().GetName().count('TH1'):
            clog.warning('copyHistFromInputFiles - Count is not TH1: %s:Count' %(f))
            continue
        
        if hcount == None:
            hc.SetDirectory(0)
            hcount = hc

        else:
            hcount.Add(hc)
                
    #
    # Write Count histograms to output file
    #
    if not hcount:
        clog.warning('copyHistFromInputFiles - missing input histogram: %s' %hpath)
        return

    if not os.path.isfile(outfile):
        clog.warning('copyHistFromInputFiles - output file does not exist: %s' %outfile)
        return

    ofile = ROOT.TFile.Open(outfile, 'UPDATE')

    if not ofile:
        clog.warning('copyHistFromInputFiles - failed to open output file: %s' %outfile)
        return

    clog.info('copyHistFromInputFiles - will save "%s" histogram to %s' %(hpath, outfile))

    hcount.SetDirectory(ROOT.Anp.GetDir(ofile, hdir))

    ofile.Write()
    ofile.Close()

#========================================================================================================
def copyCountsFromInputFiles(dirname, files, outpath, outdir=None):

    """
    copyCountsFromInputFiles - copy counts histograms from input files to output file
    """

    if not isinstance(outpath, str):
        clog.warning('copyCountsFromInputFiles - invalid directory: %s' %(outpath))
        return False

    import ROOT

    hists = {}

    #
    # Collect Count histograms from input files
    #
    for f in files:
        
        rfile = ROOT.TFile.Open(f, 'READ')
        
        if not rfile:
            clog.warning('copyCounts - failed to open: %s' %f)
            continue

        rdir = rfile.Get(dirname)

        if not rdir:
            clog.warning('copyCounts - missing dir: %s:%s' %(f, dirname))
            return False
        
        for dir_key in rdir.GetListOfKeys():

            if not hasattr(dir_key, 'GetName') or not hasattr(dir_key, 'GetClassName'):
                clog.warning('copyCounts - ignore bad key: %s:%s/%s' %(f, dirname, nm_func()))
                continue

            if dir_key.GetClassName().count('TDirectory') == 0:
                clog.warning('copyCounts - ignore non-TDirectory: %s:%s' %(f, dirname))
                continue
            
            hdir = rdir.Get(dir_key.GetName())
         
            if not hdir:
                clog.warning('copyCounts - missing histogram dir: %s:%s/%s' %(f, dirname, dir_key.GetName()))
                continue
   
            for hkey in hdir.GetListOfKeys():

                if not hasattr(hkey, 'GetName') or not hasattr(hkey, 'GetClassName'):
                    clog.warning('copyCounts - ignore bad key: %s:%s/%s' %(f, dirname, dir_key.GetName()))
                    continue

                if hkey.GetClassName().count('TH1'):
                    h = hdir.Get(hkey.GetName())
                    h.SetDirectory(0)

                    if not h:
                        clog.info('copyCounts - missing TH1: %s:%s/%s/%s' %(f, dirname, dir_key.GetName(), hkey.GetName()))
                        continue
                    
                    hpath = '%s/%s/%s' %(dirname, dir_key.GetName(), hkey.GetName())
                    
                    clog.debug('copyCounts - found TH1: %s:%s' %(f, hpath))

                    if hpath not in hists:
                        c = h.Clone()
                        c.SetDirectory(0)

                        hists[hpath] = h
                    else:
                        hists[hpath].Add(h)

                else:
                    clog.warning('copyCounts - ignore non-TH1 key: %s:%s/%s/%s' %(f, dirname, dir_key.GetName(), hkey.GetName()))

        rfile.Close()

                
    #
    # Write Count histograms to output file
    #
    if outpath and not os.path.isfile(outpath):
        clog.warning('copyCounts - output file does not exist: %s' %outpath)
        clog.warning('copyCounts - current dir: %s' %os.getcwd())

        for f in os.listdir(os.getcwd()):
            print f
            print os.lstat('%s/%s' %(os.getcwd(), f))

        return False

    ofile = ROOT.TFile.Open(outpath, 'UPDATE')

    if not ofile:
        clog.warning('copyCounts - failed to open output file: %s' %outpath)
        return False

    clog.info('copyCounts - will save %d histogram(s) to %s' %(len(hists), outpath))

    for hpath, hist in hists.iteritems():
        dpath = os.path.dirname(hpath)

        if outdir:
            dpath = dpath.replace(dirname, outdir)

        rdir = ROOT.Anp.GetRecursiveDir(ofile, dpath)

        if not rdir:
            clog.warning('copyCounts - failed to make dir: %s:%s' %(outpath, dpath))
            continue

        clog.info('   save histogram: %s:%s' %(outpath, dpath))
        
        hist.SetDirectory(rdir)

    ofile.Write()
    ofile.Close()

    return True
