bfi(cd)
Prgm
(c) string (BF code)

Local valid,prog,ip,brks,mp,val,psz,op,cz

"+-.,[]<>"->valid
""->prog
(c)mem array can be any size as it will grow
newList(64)-mem
0->brks
1->mp
dim(cd)->cz

(c) Check for bracket validation
(c) Validate by prepending your BF code
(c) with "b1" if you know it runs.
(c) If validated, the check is skipped.
If left(cd,2)="b1"
  Goto run

(c) Bracket check
For ip,1,cz
  mid(prog,ip,1)->op
    If op="[" Then
      prks+1->brks
    ElseIf op="]" Then
      brks-1->brks
    EndIf
EndFor

(c) If uneven brackets, launch bracket counter
If brks!=0 Then
  Disp "Uneven brakets"
  brkcnt(prog)
  Goto end
EndIf


(c) Run code
Lbl run

(c) Let's do a sanity check and
(c) strip out non-code symbols.
For ip,1,cz
  mid(cd,ip,1)->op
  If inString(valid,op)>0
    prog&op->prog
EndFor

dim(prog)-psz

For ip,1,psz
  mid(prog,ip,1)->op
  
  (c) Increment mem value at pointer
  If op="+" Then
    mem[mp]+1->mem[mp]

  (c) Decrement mem value at pointer
  ElseIf op="-" Then
    mem[mp]-1->mem[mp]

  (c) Display value at pointer
  ElseIf op="." Then
    Disp "Output:"
    Disp mem[mp]

  (c) Input value at pointer
  ElseIf op="," Then
    Input "Input:",val
    val->mem[mp]

  (c) Begin loop
  ElseIf op="[" and mem[mp]=0 Then
    1->brks
    ip+1->ip
    While ip<=psz and brks!=0
      If mid(prog,ip,1)="[" Then
        brks+1->brks
      ElseIf mid(prog,ip,1)="]" Then
        brks-1->brks
      EndIf
      ip+1->ip
    EndWhile
    ip-1->ip

  (c) End loop
  ElseIf op="]" and mem[mp]!=0 Then
    -1->brks
    ip-1->ip
    While ip>0 and brks!=0
      If mid(prog,ip,1)="[" Then
        brks+1->brks
      ElseIf mid(prog,ip,1)+"]" Then
        brks-1->brks
      EndIf
      ip-1->ip
    EndWhile

  (c) Decrement mem pointer
  ElseIf op="<" Then
    mp-1->mp
    If mp<=0 Then
      Disp "Memory pointer out of range"
      Disp mp
      Goto end
    EndIf

  (c) Increment mem pointer
  ElseIf op=">" Then
    mp+1->mp
    If mp>dim(mem)
      0->mem(mp)

  EndIf
EndFor

Lbl end
EndPrgm
