Get the compiled version as part of **Comae Toolkit**.

# Hibr2Bin
Comae Hibernation File Decompressor (SANDMAN project)

Back in 2007 [1], after reversing Microsoft Windows Kernel Power Management functions and its compression algorithm.
I started an open source project called Enter SandMan that aimed at decompressing hibernation files on Windows and extracting information out of it via an interactive shell - 10 years later hibernation file based memory forensics became very popular in the Law Enforcement World and helped many investigators to solve many cases all over the World.

SandMan initially started as an open-source project, but in 2008 a German company called X-Ways stole[2][3][4] my open source code without giving any proper due credits. As a results, I stopped open sourcing my projects.

2017 is now here, and I thought it would be a good way to start the year by open sourcing the current version of Hibr2Bin which also had been recently documented [5] by Joe T. Sylve, Vico Marziale, Golden G. Richard III.

- [1] http://www.msuiche.net/2007/12/06/enter-sandman-japan-pacsec-2007/
- [2] http://lists.volatilesystems.com/pipermail/vol-users/2009-July/000111.html
- [3] http://computer.forensikblog.de/en/2008/04/the-3-vendors.html
- [4] http://www.msuiche.net/2008/04/03/x-ways-forensics-beta-2-and-hibernation-file/
- [5] https://www.504ensics.com/uploads/publications/modern-windows-hibernation.pdf

## Usage

```
  Hibr2Bin 3.0
  Copyright (C) 2007 - 2017, Matthieu Suiche <http://www.msuiche.net>
  Copyright (C) 2012 - 2014, MoonSols Limited <http://www.moonsols.com>
  Copyright (C) 2015 - 2017, Comae Technologies FZE <http://www.comae.io>

Usage: Hibr2Bin [Options] /INPUT <FILENAME> /OUTPUT <FILENAME>

Description:
  Enables users to uncompress Windows hibernation file.

Options:
  /PLATFORM, /P         Select platform (X64 or X86)

  /MAJOR, /V            Select major version (e.g. 6 for NT 6.1

  /MINOR, /M            Select minor version (e.g. 1 for NT 6.1)

  /OFFSET, /L           Data offset in hexadecimal (optional)

  /INPUT, /I            Input hiberfil.sys file.

  /OUTPUT, /O           Output hiberfil.sys file.

Versions:
  /MAJOR 5 /MINOR 1     Windows XP
  /MAJOR 5 /MINOR 2     Windows XP x64, Windows 2003 R2
  /MAJOR 6 /MINOR 0     Windows Vista, Windows Server 2008
  /MAJOR 6 /MINOR 1     Windows 7, Windows Server 2008 R2
  /MAJOR 6 /MINOR 2     Windows 8, Windows Server 2012
  /MAJOR 6 /MINOR 3     Windows 8.1, Windows Server 2012 R2
  /MAJOR 10 /MINOR 0    Windows 10, Windows Server 2016

  Uncompress a Windows 7 (NT 6.1) x64 hibernation file:
      HIBR2BIN /PLATFORM X64 /MAJOR 6 /MINOR 1 /INPUT hiberfil.sys /OUTPUT uncompressed.bin

  Uncompress a Windows 10 (NT 10.0) x86 hibernation file:
      HIBR2BIN /PLATFORM X86 /MAJOR 10 /MINOR 0 /INPUT hiberfil.sys /OUTPUT uncompressed.bin
```

## Contact
www.comae.io
