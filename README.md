# VTUIlib - Vera TUI Library
Library to create Text user interfaces on the Commander X16 by writing directly to the VERA

The generic library is meant to be loaded as a binary and then just called as needed.
All functions are placed at the beginning of the library to make it as easy as possible
to know which addresses to call.

For more information see the [VTUI generic](VTUIlib-generic.md) library documentation

The acme library is meant to be included with !src statement directly into source that
is meant to be compiled with acme.

There are plans of creating an include file for ca65 as well.
