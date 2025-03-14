ant()
Prgm
'(c) Langton's Ant
'(c) Cellular Automaton

Local i,yp,xp,yh,xh,dir

setGraph("grid","off")
setGraph("axes","off")
setGraph("labels","off")

38->yp
79->xp
76->yh
158->xh

0->dir
PxlOn yp,xp
yp-1->yp

While true
  '(c) If on,
  '(c) turn anti-clockwise
  '(c) then set off
  If pxlTest(yp,xp) Then
    PxlOff yp,xp
    mod(dir-1,4)->dir
  Else
  '(c) Turn clockwise
  '(c) set on
    PxlOn yp,xp
    mod(dir+1,4)->dir
  EndIf

  '(c) Move one cell
  If dir=0 Then
    yp-1->yp
  ElseIf dir=1 Then
    xp+1->xp
  ElseIf dir=2 Then
    yp+1->yp
  Else
    xp-1->xp
  EndIf

  '(c) Allow screen
  '(c) wrapping
  If xp>xh Then
    0->xp
  ElseIf xp<0 Then
    xh->xp
  EndIf

  If yp>yh Then
    0->yp
  ElseIf yp<0 Then
    yh->yp
  EndIf
EndWhile

EndPrgm
