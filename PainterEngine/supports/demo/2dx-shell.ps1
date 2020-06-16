set-ExecutionPolicy RemoteSigned
$index=0
for($index=0;$index -le 12;$index++)
{
 .\PainterEngine_PngToTRaw.exe .\loading\$index.png .\loading\$index.traw
}
.\PainterEngine_2dxMakeTool.exe .\loading\2dxs.txt .\clock.2dx