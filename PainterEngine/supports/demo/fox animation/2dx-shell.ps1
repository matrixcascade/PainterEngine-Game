mkdir ./release
$image_width=192
$image_height=192
.\PainterEngine_PngToTRaw.exe ".\jump 0.png" ".\release\jump 0.traw" $image_width $image_height
.\PainterEngine_PngToTRaw.exe ".\jump up.png" ".\release\jump up.traw" $image_width $image_height
.\PainterEngine_PngToTRaw.exe ".\jump down.png" ".\release\jump down.traw" $image_width $image_height
$index=0
for($index=1;$index -le 6;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\run $index.png" ".\release\run $index.traw" $image_width $image_height
}
$index=0
for($index=1;$index -le 8;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\stand $index.png" ".\release\stand $index.traw" $image_width $image_height
}
$index=0
for($index=1;$index -le 6;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\walk $index.png" ".\release\walk $index.traw" $image_width $image_height
}

.\PainterEngine_2dxMakeTool.exe .\makefile.txt .\release\release.2dx

del .\release\*.traw