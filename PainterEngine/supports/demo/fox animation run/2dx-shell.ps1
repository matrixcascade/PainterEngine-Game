mkdir ./release
$image_width=192
$image_height=192
$index=0
for($index=1;$index -le 6;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\run $index.png" ".\release\run $index.traw" $image_width $image_height
}
.\PainterEngine_2dxMakeTool.exe .\makefile.txt .\release\release.2dx

del .\release\*.traw