INPUTS  : a b c d e f h
OUTPUTS : out1 out2
REGS    : r1 r2 r3 r4 r5 r6 r7

while ( a ) //
{ }
r1 = a + b
r2 = c + d
while ( r1 ) {
  r3 = d + e 
  while ( r2 )
  { r2 = d - d }  
  if ( r3 ) {
    r3 = f - f }
    r1 = a - a
}
while ( r5 ) {
   r5 = c - c
}
if (	r3 ) {
  r6 = r3 + r4
  r5 = b + r3
}
out1 = r6
out2 = r5

if ( r5 )
{ 
while ( r6 )
{ 
} 
} 
if ( r5 ) 
out2 = r7 
r7 = r5 + r6
{
 }