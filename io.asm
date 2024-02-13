4084                          .ORG   16516    //OUT routine entry point
4084   21 82 40               LD   hl,16514   //Port is stored in 16514 for OUT and IN commands
4087   4E                     LD   c,(hl)   
4088   21 83 40               LD   hl,16515   //Value is stored in 16515 for OUT command
408B   46                     LD   b,(hl)     
408C   ED 41                  OUT   (c),b   
408E   C9                     RET   
408F   21 82 40               LD   hl,16514   //IN routine entry point
4092   4E                     LD   c,(hl)   
4093   ED 40                  IN   b,(c)   
4095   48                     LD   c,b        //Returns value from IN in the BC registers as an unsigned int32
4096   06 00                  LD   b,0   
4098   C9                     RET

The above code gets poked into the address starting at 16416 that was reserved in the BASIC programs 1st line of code which reserved 23 bytes to hold the port, value, and machine language routines.

10 REM 12345678901234567890123
