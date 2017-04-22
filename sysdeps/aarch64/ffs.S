#include <sysdep.h>
    
    .syntax unified
    .text
      
ENTRY (__ffs)
    cmp   x0, #0
    b.eq  done
    rbit  x1, x0
    clz   x0, x1
    add   x0, x0, #1
done:
    ret
END(__ffs)

weak_alias (_ffs, ffs)
weak_alias (__ffs, ffsl)
libc_hidden_def (__ffs)
libc_hidden_builtin_def (ffs)
