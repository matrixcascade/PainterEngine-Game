$index=0
for($index=1;$index -le 6;$index++)
{
 .\PainterEngine_PngToTRaw.exe ".\$index.png" ".\$index.traw"
}

.\PainterEngine_2dxMakeTool.exe .\makefile.txt .\railgunExplosion.2dx