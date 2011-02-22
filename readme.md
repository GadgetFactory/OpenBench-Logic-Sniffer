The OpenBench Logic Sniffer is a $50 Open Source Logic Analyzer developed in collaboration between Gadget Factory (www.gadgetfactory.net) and Dangerous Prototypes (www.dangerousprototypes.com). 

Overview
--------
This git repository is a super project to bring together all of the Openbench Logic Sniffer projects into one place. It contains a master build script and all of the subprojects that comprise the software for the OLS.


Purchase
--------
* [Gadget Factory](http://www.gadgetfactory.net/index.php?main_page=product_info&cPath=10&products_id=30)
* [Seeed Studio](http://www.seeedstudio.com/depot/preorder-open-workbench-logic-sniffer-p-612.html?cPath=75)

Prerequisites
-------------
Before building ensure you have the following installed:
* git, make, gcc-mingw, g++. Get these with cygwin for Windows
* perl
* unzip, zip
* [ant](http://ant.apache.org)
* maven

Build
-----

  `
  $ cd build
  $ ant update
  $ ant build
  $ ant run
  $ ant dist`

run 'ant help' to see options.