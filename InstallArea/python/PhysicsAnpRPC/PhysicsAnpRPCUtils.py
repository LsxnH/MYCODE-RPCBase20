
import logging
import os
import re
import sys
import ROOT

import PhysicsAnpBase.PhysicsAnpBaseConfig as physicsBase

#========================================================================================================
def getLog(name, level='INFO', debug=False, print_time=False):

    if print_time and name:
        f = logging.Formatter('%(asctime)s - %(name)s: %(levelname)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
    elif name:
        f = logging.Formatter('%(name)s: %(levelname)s - %(message)s')
    else:
        f = logging.Formatter('%(message)s')
        
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
# Global module variables
#
clog = getLog(os.path.basename(__file__))

canvases = {}

#========================================================================================================
class Quiet:
    '''
    Context manager for silencing certain ROOT operations.  Usage:
    with Quiet(level = ROOT.kInfo+1):
       foo_that_makes_output
    You can set a higher or lower warning level to ignore different
    kinds of messages.  After the end of indentation, the level is set
    back to what it was previously.
    '''

    def __init__(self, level = ROOT.kInfo + 1):
        self.level = level

    def __enter__(self):
        self.oldlevel = ROOT.gErrorIgnoreLevel
        ROOT.gErrorIgnoreLevel = self.level

    def __exit__(self, type, value, traceback):
        ROOT.gErrorIgnoreLevel = self.oldlevel

#========================================================================================================
# Load necessary shared libraries
#
def setPlotDefaults(root, options = None):

    root.gROOT.SetStyle('Plain')

    root.gStyle.SetFillColor(10)           
    root.gStyle.SetFrameFillColor(10)      
    root.gStyle.SetCanvasColor(10)         
    root.gStyle.SetPadColor(10)            
    root.gStyle.SetTitleFillColor(0)       
    root.gStyle.SetStatColor(10)   
    
    root.gStyle.SetCanvasBorderMode(0)
    root.gStyle.SetFrameBorderMode(0) 
    root.gStyle.SetPadBorderMode(0)   
    root.gStyle.SetDrawBorder(0)      
    root.gStyle.SetTitleBorderSize(0)
    
    root.gStyle.SetFuncWidth(2)
    root.gStyle.SetHistLineWidth(2)
    root.gStyle.SetFuncColor(2)
    
    root.gStyle.SetPadTopMargin(0.08)
    root.gStyle.SetPadBottomMargin(0.16)
    root.gStyle.SetPadLeftMargin(0.14)
    root.gStyle.SetPadRightMargin(0.08)
  
    # set axis ticks on top and right
    root.gStyle.SetPadTickX(1)         
    root.gStyle.SetPadTickY(1)         
  
    # Set the background color to white
    root.gStyle.SetFillColor(10)           
    root.gStyle.SetFrameFillColor(10)      
    root.gStyle.SetCanvasColor(10)         
    root.gStyle.SetPadColor(10)            
    root.gStyle.SetTitleFillColor(0)       
    root.gStyle.SetStatColor(10)           
  
  
    # Turn off all borders
    root.gStyle.SetCanvasBorderMode(0)
    root.gStyle.SetFrameBorderMode(0) 
    root.gStyle.SetPadBorderMode(0)   
    root.gStyle.SetDrawBorder(0)      
    root.gStyle.SetTitleBorderSize(0) 
  
    # Set the size of the default canvas
    root.gStyle.SetCanvasDefH(400)          
    root.gStyle.SetCanvasDefW(650)          
    #gStyle->SetCanvasDefX(10)
    #gStyle->SetCanvasDefY(10)   
  
    # Set fonts
    font = 42
    root.gStyle.SetLabelFont(font,'xyz')
    root.gStyle.SetStatFont(font)       
    root.gStyle.SetTitleFont(font)      
    root.gStyle.SetTitleFont(font,'xyz')
    root.gStyle.SetTextFont(font)       
    root.gStyle.SetTitleX(0.3)        
    root.gStyle.SetTitleW(0.4)        
  
   # Set Line Widths
   #gStyle->SetFrameLineWidth(0)
    root.gStyle.SetFuncWidth(2)
    root.gStyle.SetHistLineWidth(2)
    root.gStyle.SetFuncColor(2)
  
   # Set tick marks and turn off grids
    root.gStyle.SetNdivisions(505,'xyz')
  
   # Set Data/Stat/... and other options
    root.gStyle.SetOptDate(0)
    root.gStyle.SetDateX(0.1)
    root.gStyle.SetDateY(0.1)
   #gStyle->SetOptFile(0)
    #root.gStyle.SetOptStat(1110)
    root.gStyle.SetOptStat('reimo')
    root.gStyle.SetOptFit(111)
    root.gStyle.SetStatFormat('6.3f')
    root.gStyle.SetFitFormat('6.3f')
    root.gStyle.SetPaintTextFormat('0.2f')

   #gStyle->SetStatTextColor(1)
   #gStyle->SetStatColor(1)
   #gStyle->SetOptFit(1)
   #gStyle->SetStatH(0.20)
   #gStyle->SetStatStyle(0)
   #gStyle->SetStatW(0.30)
   #gStyle -SetStatLineColor(0)
    root.gStyle.SetStatX(0.919)
    root.gStyle.SetStatY(0.919)
    root.gStyle.SetOptTitle(0)
   #gStyle->SetStatStyle(0000)    # transparent mode of Stats PaveLabel
    root.gStyle.SetStatBorderSize(0)

    root.gStyle.SetLabelSize(0.065,'xyz')
   #gStyle -> SetLabelOffset(0.005,'xyz')
   #gStyle -> SetTitleY(.98)
    root.gStyle.SetTitleOffset(1.10,'xz')
    root.gStyle.SetTitleOffset(1.20,'y')
    root.gStyle.SetTitleSize(0.075, 'xyz')
    root.gStyle.SetLabelSize(0.075, 'xyz')
    #root.gStyle.SetTextAlign(22)
    root.gStyle.SetTextSize(0.12)
    
    #root.gStyle.SetPaperSize(root.TStyle.kA4)  
    root.gStyle.SetPalette(1)
  
   #root.gStyle.SetHistMinimumZero(True)
    root.TGaxis.SetExponentOffset(0.03, -0.055, 'x')

    root.gROOT.ForceStyle()

#========================================================================================================
def makeCanvas1d(name, logy=False):

    global canvases
    
    try:
        c = canvases[name]
        c.cd()
        c.Clear()
        c.Draw()
        return c
    except KeyError:
        pass

    ROOT.gStyle.SetPadTopMargin   (0.10)
    ROOT.gStyle.SetPadRightMargin (0.10) 
    ROOT.gStyle.SetPadLeftMargin  (0.18)
    ROOT.gStyle.SetPadBottomMargin(0.18)

    can = ROOT.TCanvas(name, name, 900, 600)
    
    if logy:
        can.SetLogy(True)

    can.cd()
    can.Draw()
    canvases[name] = can
    
    return can

#========================================================================================================
def makeCanvas2d(name, logy=False, right_margin=0.2):

    global canvases
    
    try:
        c = canvases[name]
        c.cd()
        c.Clear()
        c.Draw()
        return c
    except KeyError:
        pass

    ROOT.gStyle.SetPadTopMargin   (0.10)
    ROOT.gStyle.SetPadRightMargin (right_margin) 
    ROOT.gStyle.SetPadLeftMargin  (0.20)
    ROOT.gStyle.SetPadBottomMargin(0.20)

    can = ROOT.TCanvas(name, name, 1000, 900)
    
    if logy:
        can.SetLogy(True)

    can.cd()
    can.Draw()
    canvases[name] = can
    
    return can

#============================================================================================
def fillTH1(hist, value):
    
    if value >= hist.GetXaxis().GetXmax():
        hist.Fill(hist.GetXaxis().GetBinCenter(hist.GetNbinsX()))        
    elif value <= hist.GetXaxis().GetXmin():
        hist.Fill(hist.GetXaxis().GetBinCenter(1))
    else:
        hist.Fill(value) 

#============================================================================================
def fillTH2(hist, xvalue, yvalue):
    
    hist.Fill(xvalue, yvalue)

#========================================================================================================
def updateCanvas(can, options, name=None, wait=False, logy=False, debug=False):

    '''
    Process current canvas: 
      - set options as requested
      - if requested, update canvas and wait for primitive (click, etc) if requested
      - print canvas if output name is provided
    '''

    import ROOT

    if debug:
        clog.info('updateCanvas - canvas=%s, name=%s' %(can.GetName(), name))
    
    if not can:
        clog.info('updateCanvas - current canvas is closed: exit now')
        sys.exit(0)

    curr_name = can.GetName()
    curr_mode = None
    curr_logy = None

    do_silent = not options.wait and not wait
    
    if do_silent:
        curr_mode = can.IsBatch()
        can.SetBatch(True)
    
    if logy:
        curr_logy = can.GetLogy()
        can.SetLogy(True)

    if debug:
         clog.info('   do_silent      =%s' %do_silent)
         clog.info('   curr_mode      =%s' %curr_mode)
         clog.info('   curr_logy      =%s' %curr_logy)
         clog.info('   can.IsBatch()  =%s' %can.IsBatch())
        
    can.Modified()
    can.Update()

    if not do_silent:
        if debug:
             clog.info('   wait primitive...')
        can.WaitPrimitive()

    is_closed = True

    for c in ROOT.gROOT.GetListOfCanvases():
        if curr_name == c.GetName():
            is_closed = False
            break
        
    if is_closed:
        clog.info('updateCanvas - current canvas is closed: exit now')        
        sys.exit(0)

    if options.save and name != None:
        if debug:
             clog.info('   print canvas')
        
        with Quiet():
            if hasattr(options, 'png') and getattr(options, 'png'):
                can.Print('%s.png' %name, 'png')
            else:
                can.Print('%s.pdf' %name, 'pdf')
        
    if curr_logy != None:
        can.SetLogy(curr_logy)

    if curr_mode != None:
        can.SetBatch(curr_mode)

    if debug:
         clog.info('   all done')
