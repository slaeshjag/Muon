@echo off
strip build\*.exe
strip build\*.dll
candle -nologo "muon.wxs" -out "muon.wixobj" -ext WixUIExtension
light -nologo "muon.wixobj" -out "muon.msi" -ext WixUIExtension