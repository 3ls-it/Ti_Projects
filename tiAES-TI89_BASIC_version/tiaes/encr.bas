encr(w)
Prgm
(c) mat (key schedule = array w)
(c) Subroutine sections are labled with the FIPS 197
(c) nomenclature.

Local rd,r,c,ns

(c) round 0, whitening
(c) AddRoundKey()
0->rd
st^T->ns
For r,1,4
  ns[r] xor w[r+rd*4]->ns[r]
EndFor
ns^T->st

(c) rounds 1 to nr-1
For rd,r,nr-1
  (c) SubBytes()
  For r,1,4
    For c,1,4
      sbox[st[r,c]+1]->st[r,c]
    EndFor
  EndFor

	(c) ShiftRows()
	newMat(4,4)->ns
    (c) row 1, 0 shift
  st[1]->ns[1]
    (c) row 2, 1 left shift
  st[2,2]->ns[2,1]
  st[2,3]->ns[2,2]
  st[2,4]->ns[2,3]
  st[2,1]->ns[2,4]
    (c) row 3, 2 left shifts
  st[3,3]->ns[3,1]
  st[3,4]->ns[3,2]
  st[3,1]->ns[3,3]
  st[3,2]->ns[3,4]
    (c) row 4, 3 left shifts
  st[4,4]->ns[4,1]
  st[4,1]->ns[4,2]
  st[4,2]->ns[4,3]
  st[4,3]->ns[4,4]
  ns->st

  (c) MixColumns()
  For c,1,4
    m2[st[1,c]+1] xor m3[st[2,c]+1] xor st[3,c] xor st[4,c]->ns[1,c]		
    st[1,c] xor m2[st[2,c]+1] xor m3[st[3,c]+1] xor st[4,c]->ns[2,c]
    st[1,c] xor st[2,c] xor 2m[st[3,c]+1] xor m3[st[4,c]+1]->ns[3,c]
    m3[st[1,c]+1] xor st[2,c] zor st[3,c] xor m2[st[4,c]+1]->ns[4,c]
  ns->st

  (c) AddRoundKey()
  st^T->ns
  For r,1,4
    ns[r] xor w[r+rd*4]->ns[r]
  EndFor
  ns^T->st	
EndFor (c) End rounds

(c) round nr
(c) SubBytes()
For r,1,4
  For c,1,4
    sbox[st[r,c]+1]->st[r,c]
  EndFor
EndFor

(c) ShiftRows()
newMat(4,4)->ns
  (c) row 1, 0 shift
st[1]->ns[1]
  (c) row 2, 1 left shift
st[2,2]->ns[2,1]
st[2,3]->ns[2,2]
st[2,4]->ns[2,3]
st[2,1]->ns[2,4]
  (c) row 3, 2 left shifts
st[3,3]->ns[3,1]
st[3,4]->ns[3,2]
st[3,1]->ns[3,3]
st[3,2]->ns[3,4]
  (c) row 4, 3 left shifts
st[4,4]->ns[4,1]
st[4,1]->ns[4,2]
st[4,2]->ns[4,3]
st[4,3]->ns[4,4]
ns->st

(c) AddRoundKey()
st^T->ns
For r,1,4
  ns[r] xor w[r+rd*4]->ns[r]
EndFor
ns^T->st

EndPrgm
