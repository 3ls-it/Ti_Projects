cbcenc(ps,w)
Prgm
(c) string,mat
(c) takes a plaintext string and key schedule
 
Local cs,i,sl,pb,cb,mb,iv,ivs
 
dim(ps)+16->sl
(c) empty cipher string
""->cs

(c) get CBC initialization vector
set_iv()->iv
ord_str(iv)->ivs

For i,1,sl,16
    (c) load 16 bytes of ps into state
    char_ord(mid(ps,i,16))->st
    iv xor st->st
    encr(w)
    cs&ord_str(st)->cs
    st->iv
EndFor

(c) Prepend IV to output
ivs&cs->out
EndPrgm
