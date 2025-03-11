dataenc(file)
Prgm
(c)[string, list, mat]

Local ft,i,j,sz,rsz,csz,cel,t,ky,ky2,w,r,dir,eout

ClrIO
blnktab()
CustomOn

Disp file
InputStr "y/n",r
If r!="y" Then
	Pause "Quit"
	Goto quit
EndIf

(c) test if file exists and protect
Try
	Archive #file
Else
	Pause "Filename?"
	Goto quit
EndTry

(c) output file
Lbl name
InputStr "Output name",eout
If eout="eout" or eout="out" or eout=file
	Goto name

(c) get pass
Lbl pphr
InputStr "Passphrase",ky
If dim(ky)<16 Then
	Disp "Too short"
	Goto pphr
EndIf
Disp "Retype"
InputStr "passphrase",ky2
If ky2=ky Then
	Disp "Passphrases","don't match"
	Goto pphr
EndIf

(c) set cwd to tiaes\
setFold(tiaes)->dir
dir&"\"&file->file
dir&"\"&eout->eout

(c) set globals
setglbls(dim(ky))

(c) estimate time (based on TI-89 HW2)
ClrIO (c) clear pass from screen
Disp geteste(blccnt(file,16))
InputStr "Continue?",r
If r!="y"
	Goto quit

(c) file types
getType(#file)->ft
If ft="STR" Then
	Goto dostr
ElseIf ft="LIST" Then
	Goto dolst
ElseIf ft="MAT" Then
	Goto domat
Else
	Goto quit
EndIf

(c) strings
Lbl dostr
Disp "Busy"
kprep(ky)->w
cbcenc(#file,w)
out->#eout
Archive #eout
DelVar out,st
Goto end

(c) lists
Lbl dolst
Dim(#file)->sz
#file->#eout
Disp "Busy"
kprep(ky)->w
For i,1,sz
	#file[i]->cel
	getType(cel)->t
	If t="STR" and cel!="" Then
		cbcenc(cel,w)
		out->#eout[i]
	EndIf
EndFor
Archive #eout
DelVar out,st
Goto end

(c) matrices
Lbl domat
rowDim(#file)->rsz
colDim(#file)->csz
#file->#eout
Disp "Busy"
kprep(ky)->w
For j,1,csz
	For i,1,rsz
		#file[i,j]->cel
		getType(cel)-t
		If t="STR" and cel!="" Then
			cbcenc(cel,w)
			out->#eout[i,j]
		EndIf
	EndFor
EndFor
Archive #eout
DelVar out,st

Lbl end
Pause "Done"

Lbl quit
setFold(#dir)
EndPrgm
