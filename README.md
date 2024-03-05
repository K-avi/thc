Simple thread pool implementation in C that I'll use 
in the https://github.com/fur0rem/lsrt project in order 
to multi-thread matrix computations on the CPU. 

I wanted to write my own pool because the ones that 
I could find on github were either using static circular
arrays or linked lists for their queues and I wanted 
to use a dynamic circular array. 

The code isn't necessarilly very portable, it's goal 
is to run on modern Linux installs.

My main reference for the implementation was :

    - Mathias Brossard's threadpool repo https://github.com/mbrossard/threadpool/tree/master

