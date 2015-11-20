INPUTS  : a b c j
OUTPUTS : z x
REGS    : d e f g h dLTe dEQe dLTEe

g = a + b
e f g ? c : e
g = g - b  
dEQe = g == e
dLTe = d > g
g = dEQe * dLTe
z = g
//

g = g * b
h = g * c
g = j * g