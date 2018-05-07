/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE
Copyright (c) 2017 - 2018, Comae Technologies DMCC

Module Name:

    Hiberfil.h

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

BOOLEAN
ProcessHiberfil(
    _In_ PPROGRAM_ARGUMENTS Vars,
    _Out_ MemoryBlock **OutMemoryBlock
);