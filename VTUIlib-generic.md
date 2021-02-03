# VTUI library Programmers Reference

Version 0.1

*Author: Jimmy Dansbo*

**This is preliminary documentation and can change at any point.**

This document describes the **generic** **V**ERA **T**ext **U**ser **I**nterface library.

## Overview

The VTUI library is meant to provide a set of functions for creating Text User Interfaces using
VERA in the Commander X16 computer. The library is split up into 3 flavors.
* Generic (the one documented here)
* ACME include file
* CA65 include file
The generic library is meant to be compiled into a binary by it self and loaded by the users
program. The choice of compiler/assembler is entirely up to the user as long as it is possible
to store values to zeropage and in registers before calling subroutines.

The other two flavors are include files for their respective assemblers and will be documented
separately.

## Loading
