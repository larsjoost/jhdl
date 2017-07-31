# jhdl

The software is under development and cannot be used yet.

The aim of the project is to make a series of very fast HDL tools supporting VHDL and Verilog. The tools will be:

* VHDL and Verilog syntax checker
* VHDL and Verilog analyser
* VHDL and Verilog compiling and conversion to SystemC
* SystemC simulator

VHDL and Verilog formatted filename dependency list. Could be used to generate file lists for synthesis tools or tar tool.

The code is written for maintainability and should be easily expandable.

Source code are located in ./src
 ./src/ast              Contains abstract syntax tree and scanner
 ./src/vhdl/scanner     Is the VHDL specific scanner parts
 ./src/vhdl/parser      Contains the VHDL parser that transforms the VHDL syntax into an abstract syntax tree
 ./src/verilog/scanner  Is the Verilog specific scanner parts
 ./src/verilog/parser   Contains the Verilog parser that transforms the Verilog syntax into an abstract syntax tree
 ./src/generator        Transforms the abstract syntax tree into C++
 ./src/kernel/lib       Contains the helper functions to compile and simulate the generated C++
 ./src/templates        Contains templates for the testsuite
 ./config               Contains minumum configuration file
 ./std                  The standard VHDL packages

The processing of VHDL to C++ is done using the following steps:
* The VHDL code is scanned for tokens (./src/ast and ./src/vhdl/scanner)
* The tokens are parsed into an abstract syntax tree (./src/vhdl/parser)
* The abstract syntax tree is converted to C++ code (./src/generator)
* The generated C++ code is compiled (./std and ./src/kernel/lib)
* The compiled C++ is simulated (src/hdls)

Unit tests are located in ./test

Regression tests are located in ./testsuite

