
REGS    : d e f g h dLTe dEQe dLTEe
OUTPUTS : z x
INPUTS  : a b c j

g = a + b
e =+ g + c // c // d
g = g - b  
dEQe = g == e
dLTe = d > g
g = dEQe * dLTe
z = g
// // ..

g = g * b
h = g * c
g = j * g