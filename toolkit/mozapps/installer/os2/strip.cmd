@rem compress binaries for optimum performance without disturbing chkdll32
@rem yes to: abort if in use, delete debug & extra data, leave non-resident names;
@rem align no-bounday/page-shift; no backup; use stdio; discard existing exe/dll settings;
@rem normal priority; packing: LZ, medium run lth, medium fixup; recursive search;
@rem unpack before pack; pack files; leave stub, remove debug & extra data;
lxlite *.exe *.dll /yua /ydd /yxd /ynl /anp /b- /cs+ /d /i- /ml1 /mr2 /mf2 /r+ /u+ /x- /zs:0 /zx /zd
