# -*- coding: utf-8 -*-

import numpy as np
from vispy import scene
from vispy import visuals

# try:
#     from sip import setapi
#     setapi("QVariant", 2)
#     setapi("QString", 2)
# except ImportError:
#     pass

from PyQt4 import QtGui, QtCore


class DataPlotQWidgetPyVis(scene.SceneCanvas):

    def __init__(self,parent,xRange,yRange,bgColor=(0,0,0,1), axisColor=(0.5,0.5,0.5,0.5), tickColor=(0.25,0.25,0.25,0.5), xTicks = None,yTicks = None, yTicksInterval=None, xTicksInterval=None, label=None, useAntialias=True,useOpenGL=True):
        scene.SceneCanvas.__init__(self, keys=None)
        # self.measure_fps()
        # self.size = 800, 600
        self.unfreeze()
        self.view = self.central_widget.add_view()
        self.cameraMargin = 0.00001 # hack to not skipt to default of 0.l
        self.z = None
        ## do not set the aspect in this constructor, it does not work and changes the x axis
        self.view.camera = scene.cameras.PanZoomCamera(parent=self.view.scene, interactive=False, flip=(False,False,False), name="cam")
        # self.events.disconnect() # do not handle any key or mouse events etc
        self.xRange = xRange
        self.yRange = yRange
        self.bgColor = bgColor
        self.axisColor = axisColor
        self.tickColor = tickColor
        self.xTicks = xTicks
        self.yTicks = yTicks
        self.xTicksInterval = xTicksInterval
        self.yTicksInterval = yTicksInterval
        self.useOpenGL = useOpenGL
        self.useAntialias = useAntialias

        xAxisLineWidth = 1
        yAxisLineWidth = 1



        self.view.camera.set_range(x=self.xRange, y=self.yRange, z=self.z, margin=self.cameraMargin)



        if label is not None:
            self.labelColor = (0.5, 0.5, 0.5, 0.5)
            self.label = scene.visuals.Text(text=label,anchor_x='right', anchor_y='top', pos=(max(self.xRange),max(self.yRange)), color=self.labelColor, parent=self.view.scene)

        self.maxTicks = 30
        self.xAxisLineY = self.yRange[0]
        self.yAxisLineX = self.xRange[0]

        self.yAxisTickX = self.xRange[0]
        self.xAxisTickY = self.yRange[0]



        self.xTicksMajorLineMargin = 0.05
        self.yTicksMajorLineMargin = 0.05

        self.axisLabelFontSize = 8
        self.ticksMajorLineWidth = 0.5

        self.xTicksLabels = []
        self.xTicksMajorLines = []

        self.yTicksLabels = []
        self.yTicksMajorLines = []
        self.yTicksLabelTexts = []



        self.xTickLabelOffset = 0
        self.yTicksLabelTexts = None

        self.plotLines = []

        self.xTicksChanged()
        self.yTicksChanged()


        # if self.xAxisLineY is not None:
        #     self.xAxisLine = PlotLine(np.array([(self.xRange[0],self.xAxisLineY),(self.xRange[1],self.xAxisLineY)]),
        #                               color=axisColor, width=xAxisLineWidth,
        #                               method=("gl" if useOpenGL else "agg"),
        #                               antialias=useAntialias,
        #                               parent=self.view.scene)

        # Add a 3D axis to keep us oriented
        # scene.visuals.XYZAxis(parent=self.view.scene)

        self.create_native()
        self.native.setParent(parent)



        self.freeze()

    #disable mouse event processing
    def _process_mouse_event(self, event):
        pass

    def removeAllyRangeVisuals(self):
        self.yTicksLabels = self.removeAllVisualsInList(self.yTicksLabels)
        self.yTicksMajorLines = self.removeAllVisualsInList(self.yTicksMajorLines)

    def removeAllxRangeVisuals(self):
        self.xTicksLabels = self.removeAllVisualsInList(self.xTicksLabels)
        self.xTicksMajorLines = self.removeAllVisualsInList(self.xTicksMajorLines)

    def removeAllVisualsInList(self,visuals):
        for v in visuals:
            v.parent = None
        visuals = []
        return visuals

    def setXticks(self,xTicks, update=False):
        self.xTicks = xTicks
        self.xTicksChanged(update=update)

    def setYticks(self,yTicks, update=False):
        self.yTicks = yTicks
        self.yTicksChanged(update=update)

    def setXrange(self,xRange, update=False):
        self.xRange = xRange
        self.xRangeChanged(update=update)

    def setYrange(self,yRange, update=False):
        self.yRange = yRange
        self.yRangeChanged(update=update)

    def yRangeChanged(self, update=False):
        # self.removeAllyRangeVisuals()
        self.view.camera.set_range(x=self.xRange, y=self.yRange, z=self.z, margin=self.cameraMargin)
        if update:
            self.update()

    def xRangeChanged(self, update=False):
        # self.removeAllxRangeVisuals()
        self.view.camera.set_range(x=self.xRange, y=self.yRange, z=self.z, margin=self.cameraMargin)
        if update:
            self.update()

    def hideVisual(self,visual,visible=False,update=False):
        visual.visible = visible
        visual._visible = visible
        if visible != visual._vshare.visible:
            visual._vshare.visible = visible
            if update:
                self.update()

    def unHideVisual(self,visual,visible=True,update=False):
        self.hideVisual(visual,visible=visible,update=update)

    def convertPos(self,pos):
        pos = np.atleast_2d(pos).astype(np.float32)
        return np.concatenate((pos, np.zeros((pos.shape[0], 1), np.float32)), axis=1)

    def xTicksChanged(self, update=False):
        if (self.xTicksInterval is not None) or (self.xTicks is not None):
            if self.xTicks is None:
                self.xTicks = np.unique(self.roundBase(np.linspace(self.xRange[0], self.xRange[1], self.maxTicks), self.xTicksInterval))
            for i in range(len(self.xTicks), len(self.xTicksLabels)):
                self.hideVisual(self.xTicksLabels[i])
                self.hideVisual(self.xTicksMajorLines[i])

            self.xTicksLabelsChanged()
            self.xTicksLinesChanged()



        if update:
            self.update()

    def yTicksChanged(self, update=False):
        if (self.yTicksInterval is not None) or (self.yTicks is not None):
            if self.yTicks is None:
                self.yTicks = np.unique(self.roundBase(np.linspace(self.yRange[0], self.yRange[1], self.maxTicks), self.yTicksInterval))
            for i in range(len(self.yTicks), len(self.yTicksLabels)):
                self.hideVisual(self.yTicksLabels[i])
                self.hideVisual(self.yTicksMajorLines[i])

            self.yTicksLabelsChanged()
            self.yTicksLinesChanged()
        if update:
            self.update()

    def xTicksLinesChanged(self):
        margin = abs((self.yRange[1] - self.yRange[0]) * self.xTicksMajorLineMargin)
        lineSegments = []
        for i, t in enumerate(self.xTicks):
            lineSegments.append(np.array([(t, self.yRange[0] + margin), (t, self.yRange[1] - margin)]))

        if len(self.xTicksMajorLines) < 1:
            line = PlotLine(np.array(lineSegments),
                            color=self.axisColor, width=self.ticksMajorLineWidth,
                            method=("gl" if self.useOpenGL else "agg"),
                            antialias=self.useAntialias,
                            parent=self.view.scene,
                            connect="segments")
            self.xTicksMajorLines.append(line)
        else:
            self.xTicksMajorLines[0].setDataSegments(np.array(lineSegments), update=False)
            self.unHideVisual(self.xTicksMajorLines[0])
        # for i, t in enumerate(self.xTicks):
        #     if len(self.xTicksMajorLines) < i + 1:
        #         line = PlotLine(np.array([(t, self.yRange[0] + margin), (t, self.yRange[1] - margin)]),
        #                         color=self.axisColor, width=self.ticksMajorLineWidth,
        #                         method=("gl" if self.useOpenGL else "agg"),
        #                         antialias=self.useAntialias,
        #                         parent=self.view.scene)
        #         self.xTicksMajorLines.append(line)
        #     else:
        #         self.xTicksMajorLines[i].setData(np.array((t, t)),
        #                                          np.array((self.yRange[0] + margin, self.yRange[1] - margin)), update=False)
        #         self.unHideVisual(self.xTicksMajorLines[i])

    def yTicksLinesChanged(self):
        margin = abs((self.xRange[1] - self.xRange[0]) * self.yTicksMajorLineMargin)
        lineSegments = []
        for i, t in enumerate(self.yTicks):
            lineSegments.append(np.array([(self.xRange[0] + margin, t), (self.xRange[1] - margin, t)]))


        if len(self.yTicksMajorLines) < 1:
            line = PlotLine(np.array(lineSegments),
                            color=self.axisColor, width=self.ticksMajorLineWidth,
                            method=("gl" if self.useOpenGL else "agg"),
                            antialias=self.useAntialias,
                            parent=self.view.scene,
                            connect="segments")
            self.yTicksMajorLines.append(line)
        else:
            self.yTicksMajorLines[0].setDataSegments(np.array(lineSegments), update=False)
            self.unHideVisual(self.yTicksMajorLines[0])

    # for i, t in enumerate(self.yTicks):
    #     if len(self.yTicksMajorLines) < i + 1:
    #         line = PlotLine(np.array([(self.xRange[0] + margin, t), (self.xRange[1] - margin, t)]),
    #                         color=self.axisColor, width=self.ticksMajorLineWidth,
    #                         method=("gl" if self.useOpenGL else "agg"),
    #                         antialias=self.useAntialias,
    #                         parent=self.view.scene)
    #         self.yTicksMajorLines.append(line)
    #     else:
    #         self.yTicksMajorLines[i].setData(np.array((self.xRange[0] + margin, self.yRange[1] - margin)),
    #                                          np.array((t, t)), update=False)
    #         self.unHideVisual(self.yTicksMajorLines[i])

    # def xTicksLabelsChanged(self):
    #     for i, t in enumerate(self.xTicks):
    #         labelText = str(t + self.xTickLabelOffset)
    #         if len(self.xTicksLabels) < i + 1:
    #             label = scene.visuals.Text(text=labelText, font_size=self.axisLabelFontSize, anchor_x='center', anchor_y='bottom', pos=(t, self.xAxisTickY), color=self.axisColor,
    #                                        parent=self.view.scene)
    #             self.xTicksLabels.append(label)
    #         else:
    #             # hack to make it work, adapted from the setter pos in vispy/visuals/text/text.py TextVisual.pos(self,pos)
    #             self.xTicksLabels[i]._pos = self.convertPos(np.array((t, self.xAxisTickY)))
    #             # self.xTicksLabels[i]._pos_changed = True
    #             # # if self.xTicksLabels[i]._vertices is None:
    #             # #     self.xTicksLabels[i]._prepare_draw(self.view)
    #             self.xTicksLabels[i].text = labelText
    #             self.unHideVisual(self.xTicksLabels[i])

    def xTicksLabelsChanged(self):
        labelTexts = []
        labelTextsPos = []
        for i, t in enumerate(self.xTicks):
            labelText = str(t + self.xTickLabelOffset)

            labelTexts.append(labelText)
            labelTextsPos.append((t, self.xAxisTickY))

        if len(self.xTicksLabels) < 1:
            label = scene.visuals.Text(text=labelTexts, font_size=self.axisLabelFontSize, anchor_x='center', anchor_y='bottom', pos=labelTextsPos, color=self.axisColor,
                                       parent=self.view.scene)
            self.xTicksLabels.append(label)
        else:
            # hack to make it work, adapted from the setter pos in vispy/visuals/text/text.py TextVisual.pos(self,pos)
            self.xTicksLabels[0]._pos = self.convertPos(np.array(labelTextsPos))
            # self.xTicksLabels[i]._pos_changed = True
            # # if self.xTicksLabels[i]._vertices is None:
            # #     self.xTicksLabels[i]._prepare_draw(self.view)
            self.xTicksLabels[0].text = labelTexts
            self.unHideVisual(self.xTicksLabels[0])

    # def yTicksLabelsChanged(self):
    #     for i, t in enumerate(self.yTicks):
    #         if self.yTicksLabelTexts is not None:
    #             labelText = self.yTicksLabelTexts[i]
    #         else:
    #             labelText = str(t)
    #         if len(self.yTicksLabels) < i + 1:
    #             label = scene.visuals.Text(text=labelText, font_size=self.axisLabelFontSize, anchor_x='left', anchor_y='center', pos=(self.yAxisTickX, t),
    #                                        color=self.axisColor,
    #                                        parent=self.view.scene)
    #             self.yTicksLabels.append(label)
    #         else:
    #             # hack to make it work, adapted from the setter pos in vispy/visuals/text/text.py TextVisual.pos(self,pos)
    #             self.yTicksLabels[i]._pos = self.convertPos(np.array((self.yAxisTickX, t)))
    #             # self.yTicksLabels[i]._pos_changed = True
    #             # # if self.yTicksLabels[i]._vertices is None:
    #             # #     self.yTicksLabels[i]._prepare_draw(self.view)
    #             # self.yTicksLabels[i].update()
    #             self.yTicksLabels[i].text = labelText
    #             self.unHideVisual(self.yTicksLabels[i])

    def yTicksLabelsChanged(self):
        labelTexts = []
        labelTextsPos = []
        for i, t in enumerate(self.yTicks):
            if self.yTicksLabelTexts is not None:
                labelText = self.yTicksLabelTexts[i]
            else:
                labelText = str(t)
            labelTexts.append(labelText)
            labelTextsPos.append((self.yAxisTickX, t))

        if len(self.yTicksLabels) < 1:
            label = scene.visuals.Text(text=labelTexts, font_size=self.axisLabelFontSize, anchor_x='left', anchor_y='center', pos=labelTextsPos,
                                       color=self.axisColor,
                                       parent=self.view.scene)
            self.yTicksLabels.append(label)
        else:
            # hack to make it work, adapted from the setter pos in vispy/visuals/text/text.py TextVisual.pos(self,pos)
            self.yTicksLabels[0]._pos = self.convertPos(np.array(labelTextsPos))
            # self.yTicksLabels[i]._pos_changed = True
            # # if self.yTicksLabels[i]._vertices is None:
            # #     self.yTicksLabels[i]._prepare_draw(self.view)
            # self.yTicksLabels[i].update()
            self.yTicksLabels[0].text = labelTexts
            self.unHideVisual(self.yTicksLabels[0])

    def roundBase(self, values, base):
        return (base * np.round(np.array(values) / base)).astype(float)

    def setParent(self,parent):
        self.native.setParent(parent)

    def setBgColor(self, color):
        pass

    def setLineColor(self, color):
        pass

    def addLine(self, x, y, color=(0.5,0.5,0.5,1),width=1):
        plotLine = PlotLine(np.column_stack((np.array(x), np.array(y))),
                                      color=color, width=width,
                                      method=("gl" if self.useOpenGL else "agg"),
                                      antialias=self.useAntialias,
                                      parent=self.view.scene)
        self.plotLines.append(plotLine)
        return plotLine

    def removeLine(self, line):
        res = None
        for i,plotLine in enumerate(self.plotLines):
            if plotLine == line:
                plotLine.parent = None
                return self.plotLines.pop(i)
        return res



# class PlotText(scene.visuals.Text):
#     def __init__(self,*args, **kwargs):
#         scene.visuals.Text.__init__(self, *args, **kwargs)
#
#     def pos(self, pos,update=False):
#         pos = np.atleast_2d(pos).astype(np.float32)
#         if pos.shape[1] == 2:
#             pos = np.concatenate((pos, np.zeros((pos.shape[0], 1),
#                                                 np.float32)), axis=1)
#         elif pos.shape[1] != 3:
#             raise ValueError('pos must have 2 or 3 elements')
#         elif pos.shape[0] == 0:
#             raise ValueError('at least one position must be given')
#         self._pos = pos
#         self._pos_changed = True
#         if update:
#             self.update()



class PlotLine(scene.visuals.Line):
    def __init__(self,*args, **kwargs):
        scene.visuals.Line.__init__(self, *args, **kwargs)

    def setData(self,x,y,update=False):
        # self._pos = np.column_stack((np.array(x), np.array(y)))
        # if update:
        #     self.update()
        self.set_data(np.column_stack((np.array(x), np.array(y))))

    def setDataSegments(self,segment,update=False):
        # self._pos = np.column_stack((np.array(x), np.array(y)))
        # if update:
        #     self.update()
        self.set_data(segment)

    def vanish(self):
        parent = self.canvas
        parent.scene.update()
        res = parent.removeLine(self)
        return res is not None

class ChannelView(DataPlotQWidgetPyVis):
    def __init__(self,parent, nChannels,timeSignalDurationSeconds,channelRanges,paddingChannels=0.05,timeTickInterval=5, clipChannelView=False, useAntialias=True,useOpenGL=True):

        self.clipChannelView = clipChannelView
        self.nChannels = nChannels
        self.timeSignalDurationSeconds = timeSignalDurationSeconds
        self.timeTickInterval = timeTickInterval
        self.channelRanges = channelRanges
        self.paddingChannels = paddingChannels
        self.baseScale = 1.0-2.0*self.paddingChannels #0.8
        self.channelRowHeight = 1.0 / self.nChannels
        self.channelDataHeight = self.channelRowHeight * self.baseScale
        self.baseHeight = 1.0

        self.time = np.array([0])
        self.signals = []
        for iCh in range(0, self.nChannels):
            self.signals.append(np.array([0]))

        self.channelScale = []
        for iCh in range(0, self.nChannels):
            self.channelScale.append(1)

        self.timeTicks = np.unique(self.roundBase(self.time, self.timeTickInterval))
        DataPlotQWidgetPyVis.__init__(self,parent,(0,timeSignalDurationSeconds), (0, self.baseHeight), xTicksInterval=timeTickInterval, useAntialias=useAntialias,useOpenGL=useOpenGL)

        self.xTicksMajorLineMargin = self.channelRowHeight/8
        self.xTicks = self.timeTicks - self.time[0]
        self.unfreeze()
        self.signalLines = []

        self.addChannelData(self.time, self.signals)

        self.initYtickLines()
        self.yTickLabelsTextChanged()
        self.yTicksChanged()





        self.freeze()



    def updateView(self):
        self.updateChannels()
        self.updateTimeTicks()

    def updateTimeTicks(self):
        self.timeTicks = np.unique(self.roundBase(self.time, self.timeTickInterval))
        self.xTickLabelOffset = self.time[0]
        self.setXticks(self.timeTicks - self.time[0])

    def updateChannels(self):
        for iCh in range(0, self.nChannels):
            scale = self.baseScale * self.channelScale[iCh] * self.scaleBaseFactorChannel(iCh)
            offset = self.getChannelYCenter(iCh)
            offset0 = self.channelDataHeight * (0.0-(self.channelRanges[iCh][1] + self.channelRanges[iCh][0])/2) / (self.channelRanges[iCh][1] - self.channelRanges[iCh][0])
            signal = self.signals[iCh] * scale + offset + offset0
            if self.clipChannelView:
                signal = np.clip(signal, offset-self.channelDataHeight/2.0, offset+self.channelDataHeight/2.0)
            self.signalLines[iCh].setData(self.time - self.time[0],signal)

    def addChannelData(self,time,signals):
        self.time = time
        self.signals = signals
        for iCh in range(0, self.nChannels):
            self.signalLines.append(self.addLine(time - time[0], signals[iCh], color=(1,1,1,1), width=1))
        self.updateTimeTicks()
        self.updateChannels()

    def updateChannelData(self,time,signals):
        if not self.signalLines:
            self.addChannelData(time,signals)
        else:
            self.time = time
            self.signals = signals
            self.updateChannels()
            self.updateTimeTicks()

    def initYtickLines(self):
        self.yTicks = []
        self.yTicksLabelTexts = []
        for iCh in range(0, self.nChannels):
            self.yTicks.append(self.getChannelYCenter(iCh) + self.channelDataHeight * 0.25)
            self.yTicks.append(self.getChannelYCenter(iCh))
            self.yTicks.append(self.getChannelYCenter(iCh) - self.channelDataHeight * 0.25)
            self.yTicksLabelTexts.append("")
            self.yTicksLabelTexts.append("")
            self.yTicksLabelTexts.append("")


    def yTickLabelsTextChanged(self):
        if (self.yTicksLabelTexts is not None) and (self.yTicks is not None):
            for iCh in range(0, self.nChannels):
                self.yTickLabelTextChangedChannel(iCh)

    def yTickLabelTextChangedChannel(self,iCh):
        channelDataRangeDiff = (self.channelRanges[iCh][1] - self.channelRanges[iCh][0])
        channelDataCenter = (self.channelRanges[iCh][1] + self.channelRanges[iCh][0]) / 2.0
        self.yTicksLabelTexts[iCh * 3]     = str(channelDataCenter + channelDataRangeDiff * 0.25 / self.channelScale[iCh])
        self.yTicksLabelTexts[iCh * 3 + 1] = str(channelDataCenter)
        self.yTicksLabelTexts[iCh * 3 + 2] = str(channelDataCenter - channelDataRangeDiff * 0.25 / self.channelScale[iCh])

    def getChannelYCenter(self,iCh):
        return self.baseHeight - self.channelRowHeight/2.0 - iCh * self.channelRowHeight

    def scaleBaseFactorChannel(self,iCh):
        channelDataRangeDiff = (self.channelRanges[iCh][1] - self.channelRanges[iCh][0])
        return self.channelDataHeight/channelDataRangeDiff

    def setChannelScale(self,iCh,channelScale):
        self.channelScale[iCh] = channelScale
        self.yTickLabelTextChangedChannel(iCh)
        self.yTicksLabelsChanged()

    def setChannelRange(self,iCh,channelRange):
        # prevChannelRange = self.channelRange[iCh]
        # prevChannelRangeDiff = (prevChannelRange[1] - prevChannelRange[0])
        self.channelRanges[iCh] = channelRange
        self.yTickLabelTextChangedChannel(iCh)
        self.yTicksLabelsChanged()