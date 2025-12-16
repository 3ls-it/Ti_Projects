modexp(b,e,n)
Func
(c)base,exp,mod

Local p

If e=1 Then
    Return mod(b,n)
Else
    modexp(b,int(e/2),n)->p
    If mod(e,2)=0 Then
        Return mod(p^2,n)
    Else
        Return mod(b*p^2,n)
    EndIf
EndIf

EndFunc
