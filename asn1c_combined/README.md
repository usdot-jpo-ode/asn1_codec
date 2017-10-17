# Fix for name mangling problems

# Fix for Makefile

The following compile flags must be set for the converter-example.c to compile (whether you need this or not is another story).

- -DPDU=<something>     // this allows the #ifndef statements in the above c file to work correctly.
- -DASN_PDU_COLLECTION  // if you have several message types you want to use.


