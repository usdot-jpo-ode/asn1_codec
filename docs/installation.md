# Installation and Setup

For now these are working notes.

- To build the `acm` you first must build `libj2735.a`
- To build `libj2735.a` you must have the j2735 scheme file.
    - Put the schema file in the `asn1c_j2735` directory.
    - type `make`
    - This should build the library needed above and put it in the `asn1c_codec/j2735/lib` directory.
- Create an in source or out of source build.
- Run `cmake <path to root repository>`
- Run `make`



