#******************************************************************************
#*                                                                            *
#* Functions for arithmetic and number theory with large integers in C        *
#* Software supplement to the book "Cryptography in C and C++"                *
#* by Michael Welschenbach                                                    *
#*                                                                            *
#* Module sqr.s            Revision: 15.06.2002                               *
#*                                                                            *
#*  Copyright (C) 1998-2005 by Michael Welschenbach                           *
#*  Copyright (C) 2001-2005 by Springer-Verlag Berlin, Heidelberg             *
#*  Copyright (C) 2001-2005 by Apress L.P., Berkeley, CA                      *
#*  Copyright (C) 2002-2005 by Wydawnictwa MIKOM, Poland                      *
#*  Copyright (C) 2002-2005 by PHEI, P.R.China                                *
#*  Copyright (C) 2002-2005 by InfoBook, Korea                                *
#*  Copyright (C) 2002-2005 by Triumph Publishing, Russia                     *
#*                                                                            *
#*  All Rights Reserved                                                       *
#*                                                                            *
#*  The software may be used for noncommercial purposes and may be altered,   *
#*  as long as the following conditions are accepted without any              *
#*  qualification:                                                            *
#*                                                                            *
#*  (1) All changes to the sources must be identified in such a way that the  *
#*      changed software cannot be misinterpreted as the original software.   *
#*                                                                            *
#*  (2) The statements of copyright may not be removed or altered.            *
#*                                                                            *
#*  (3) The following DISCLAIMER is accepted:                                 *
#*                                                                            *
#*  DISCLAIMER:                                                               *
#*                                                                            *
#*  There is no warranty for the software contained in this distribution, to  *
#*  the extent permitted by applicable law. The copyright holders provide the *
#*  software `as is' without warranty of any kind, either expressed or        *
#*  implied, including, but not limited to, the implied warranty of fitness   *
#*  for a particular purpose. The entire risk as to the quality and           *
#*  performance of the program is with you.                                   *
#*                                                                            *
#*  In no event unless required by applicable law or agreed to in writing     *
#*  will the copyright holders, or any of the individual authors named in     *
#*  the source files, be liable to you for damages, including any general,    *
#*  special, incidental or consequential damages arising out of any use of    *
#*  the software or out of inability to use the software (including but not   *
#*  limited to any financial losses, loss of data or data being rendered      *
#*  inaccurate or losses sustained by you or by third parties as a result of  *
#*  a failure of the software to operate with any other programs), even if    *
#*  such holder or other party has been advised of the possibility of such    *
#*  damages.                                                                  *
#*                                                                            *
#******************************************************************************
#*                                                                            *
#*      Squaring, interface compatible with C function sqr_l()                *
#*                                                                            *
#*      Q := a * a mod M                                                      *
#*                                                                            *
#*      Stack on calling of sqr:          SP+ 8 ---> Offset q                 *
#*                                        SP+ 4 ---> Offset a                 *
#*                                        SP    ---> Return address           *
#*                                                                            *
#*      Return Value : ax = 0                                                 *
#*                                                                            *
#******************************************************************************
#
.equ    fct,       0                    # Factor (working memory)
.equ    prd,     520                    # Square (working memory)
#
.equ    WORKSP, 1560                    # Working memory in stack area
#
.text
.globl  sqr

sqr:    pushl   %ebp                    # Store base ptr for caller
        movl    %esp,%ebp
        subl    $WORKSP, %esp           # Local memory
        movl    %esp,%eax
        pushl   %ebx
        pushl   %edi
        pushl   %esi
        movl    %eax,%ebx               # Store starting address
#
        movl    8(%ebp),%esi            # Offset of variable a
        movw    (%esi),%dx              # Length l(a)
        cmpw    $0,%dx
        jnz     .sqr011
        jmp     .sqrnul
.sqr011: 
#
#>>>>>> Lade Operanden
#
.sqr02: leal    fct(%ebx),%edi          # Destination offset is a[bx]          
        movl    $0,%ecx			                                       
        movw    %dx,%cx                 # l(a) into cx                         
.l1:    cld				                                       
rep     movsw                           # Load a in chunks of USHORT into a[bx]
        movsw                           #  plus one USHORT
        movw    $0,(%edi)
#
#>>>>>> Prepare addressing of local working memory
#
        pushl   %ebp                    # Store bp
        movl    %ebx,%ebp               # Indexed addressing based on ss
#
#>>>>>> Remove leading zeros from operand
#
        xorl    %eax,%eax
        movw    fct(%ebp),%ax           # #USHORTs in ax
        cmpw    $0,%ax
        je      .end1
        shll    %eax                    # #Bytes, index to high-order digit
        movl    %eax,%esi
.l2:    cmpw    $0,fct(%ebp,%esi)       # digit = 0 ?                
        jne     .l3                     # If not, then we're done    
        subl    $2,%esi                 # Else: step back one USHORT 
        cmpl    $0,%esi                 # Index = 0?                 
        je      .mazer1                 # Then argument = 0          
        jmp     .l2                     # Compare next digit         
.l3: 
.mazer1: 
	shrl    %esi                    # #USHORTS
        movl    %esi,%eax               # Get base address of oeperand
        movw    %ax,fct(%ebp)           # Store number of digits

.end1:  cmpw    $0,%ax
        jne     .sqrmul
        popl    %ebp
        jmp     .sqrnul
#
#
#>>>>>> Beginn der Quadrierung
#
#----->  Arbeitsfeld initialisieren
#
.sqrmul: 
	movl    $0,%ecx
        movw    fct(%ebp),%cx           # l(f)
        shrw    %cx
        jnc     .l4
        incw    %cx
.l4:    cmpw    $1,%cx
        jne     ._sqr09
        jmp     ._xrgsq                 # If l(f)=1 do squaring in registers
._sqr09: 
	movl    $0,%eax
        movl    $2,%esi                 # Prepare index i
._sqr10: 
	movl    %eax,prd(%ebp,%esi)
        addl    $4,%esi
        loop    ._sqr10
#
#-----> Outer loop
#
        movl    $2,%esi                 # Prepare index i
        movl    $0,%ecx
        movw    fct(%ebp),%cx           # Counter outer loop
        shrw    %cx
        jc      ._sqr11
        decw    %cx                     # in cx = l(a)-1
._sqr11: 
	pushl   %ecx                    # Store counter
#
#-----> Innere Schleife
#
        movl    %esi,%edi               # index j=i+4
        addl    $4,%edi
        xorl    %ecx,%ecx
        movw    fct(%ebp),%cx           # Counter inner loop
        shrw    %cx
        jnc     .l5
        incw    %cx
.l5:    pushl   %esi
        addl    $2,%esi
        shrl    $2,%esi
        subl    %esi,%ecx
        popl    %esi                    # cx = l(a)-i
        movl    $0,%edx                 # Dummy carry
        movl    fct(%ebp,%esi),%ebx     # Load a[i]
        addl    %edi,%esi               # Product index i+j
._sqr12: 
	pushl   %ecx                    # Store counter
        movl    %edx,%ecx               # Carry u in edx
        movl    %ebx,%eax
        mull    fct(%ebp,%edi)
        addl    %ecx,%eax               # ax + u
        adcl    $0,%edx
        addl    %eax, prd-2(%ebp,%esi)  # q[i+j-1]= ax + q[i+j-1]
        adcl    $0,%edx                 # new carry u in dx
        addl    $4,%edi                 # j=j+4
        addl    $4,%esi                 # i+j=i+j+4
        popl    %ecx                    # Get counter inner loop
        loop    ._sqr12
#
#-----> End inner loop
#
        movl    %edx,prd-2(%ebp,%esi)   # q[i+j-1]=u
        subl    %edi,%esi               # Restore i
        addl    $4,%esi                 # i=i+4
        popl    %ecx                    # Get counter outer loop
        loop    ._sqr11
#
#-----> End outer loop
#
        addl    %edi,%esi
        movl    $0,prd-2(%ebp,%esi)
        movl    $0,prd+2(%ebp,%esi)
#
#-----> Multiply intermediate result by 2
#
        movl    $6,%esi                 # First word is zero 
        xorl    %ecx,%ecx
        movw    fct(%ebp),%cx
._xsll: 
        rcll    $1,prd(%ebp,%esi)       # Shift q in chunks of ULONG
        incl    %esi
        incl    %esi
        incl    %esi
        incl    %esi
        loop    ._xsll
#
#-----> Add squares to intermediate result q
#
.xsle:  movw    fct(%ebp),%cx
.l6:    shlw    %cx
        movw    %cx,prd(%ebp)
        movl    $2,%esi
        movl    $2,%edi
        movw    fct(%ebp),%cx           # l(a) is counter
        shrw    %cx
        jnc     .l7
        incw    %cx
.l7:    xorl    %eax,%eax
        pushf                           # Dummy carry
._sqr20: 
	movl    fct(%ebp,%esi),%eax
        mull    %eax                    # ax = lo, dx = hi
        popf
        adcl    %eax, prd(%ebp,%edi)
        adcl    %edx, prd+4(%ebp,%edi)
        pushf
        addl    $4,%esi
        addl    $8,%edi
        loop    ._sqr20
        popf                            # Adjust stack pointer
        jmp     ._sqr20a
#
#-----> Squaring in registers in case of l(a) = 1
#
._xrgsq: 
	movl    fct+2(%ebp),%eax
        mull    %eax
        movl    %eax, prd+2(%ebp)
        movl    %edx, prd+6(%ebp)
        movw    $4,prd(%ebp)
#
#-----> Determine length of remainder
#
._sqr20a: 
        xorl    %eax,%eax
        movw    prd(%ebp),%ax
        movl    %eax,%edi
        shll    %edi
        cmpw    $0,prd(%ebp,%edi)
        jnz     ._sqr60
        decw    prd(%ebp)
        jmp     ._sqr20a
#
#-----> Store square
#
._sqr60: 
	leal    prd(%ebp),%esi
        movl    %ebp,%ebx
        popl    %ebp
        movl    12(%ebp), %edi
        movl    $0,%ecx
        movw    (%esi),%cx
        shrw    %cx
        jnc     .l8
        incw    %cx
.l8:    cmpw    $0,%cx
        je      .l9
        cld
rep     movsl
.l9:    movsw
#
#*******************************************************************************
#       Security: Purge stack

.sqrret: 
	movl    %ebx,%edi
        movl    $(WORKSP-4)/4, %ecx
        cld
        movl    $0,%eax                 # Return-Value = 0: OK
rep     stosl                           # Overwrite with 0
#*******************************************************************************
#
        popl    %esi
        popl    %edi
        popl    %ebx
        movl    %ebp,%esp
        popl    %ebp
        ret
#
.sqrnul: 
	movl    12(%ebp), %esi
        movw    $0,(%esi)
        jmp     .sqrret
#





