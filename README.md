# cpu_ipp

Calculate number of "instructions" per (time) period.

## Genesis

The idea for this program was to see how many 'instructions', or cycles, a CPU
would go through or perform or execute in a given period of time. So given a
certain CPU speed and a time period, calculate how many cycles the CPU will go
through.

(of course, instructions is in quotes because it may not be operating on an
instruction per se, and it is not accounting for superscalar architecture.
Cycles per second is probably the ost accurate, but cpp looked weird in the
filename)

It's a simple program and a simple idea. It came to me as I was reading about
schedulers and timeslices and I wondered how many cycles happen in 6ms or 48ms
for a given CPU, based on its speed.

## Some Additions
* Take into account multiple cores, as it is currently not.
* Add some flags, specified in the TODO in the code. 
