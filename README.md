The Body Network Simulator (BNS) Framework is a WBAN realistic simulation framework built upon Castalia that comprises:
- A wireless channel model (Goswami et. al).
- A node mobility model (MoBAN).
- A node temperature model (Temperature Manager).
- A IEEE 802.15.6 MAC (baselineBANMac)
- A temperature based routing protocol (LATOR)
- A Personal Health Device application using IEEE 11073 protocol (agent and manager)

For more information on Castalia refer to: github.com/boulis/Castalia

To install you must have OMNeT++ 4.6 and Antidote Library.
Follow the steps:


# OMNeT++ 4.6

Website: https://omnetpp.org/software/2014/12/02/omnet-4-6-released

Download:
$ ipfs get /ipns/ipfs.omnetpp.org/release/4.6/omnetpp-4.6-src.tgz

Untar and unzip the source file:
$ tar xvfz omnetpp-4.6-src.tgz

In general OMNeT++ requires that its bin directory should be in the PATH. You should add a line something like this to your .bashrc:
$ export PATH=$PATH:$HOME/omnetpp-4.6/bin

You may also have to specify the path where shared libraries are loaded from. Use:
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/omnetpp-4.6/lib

If configure complains about not finding the Tcl library directory, you may specify it by setting the TCL_LIBRARY environment variable.

In the top-level OMNeT++ directory, type:
$ ./configure

The configure script detects installed software and configuration of your system.
It writes the results into the Makefile.inc file, which will be read by the makefiles during the build process.

When ./configure has finished, you can compile OMNeT++. Type in the terminal:
$ make


# Antidote Library

Install the modified Antidote Library.
Use the code and follow the steps in: https://github.com/conqlima/Antidote

# Castalia with BNS Framework

If you’ve downloaded a compressed file (instead of cloning the project) then untar and unzip it:

$ tar –xvzf Castalia-master.tar.gz

$ cd Castalia/Castalia  (or cd Castalia-yourchosenname)

Check if the makemake file path to Antidote is correct.
Execute the makemake and make.

$ ./makemake

$ make





