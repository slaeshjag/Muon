@echo off
candle -nologo "muon.wxs" -out "muon.wixobj" -ext WixUIExtension
light -nologo "muon.wixobj" -out "muon.msi" -ext WixUIExtension