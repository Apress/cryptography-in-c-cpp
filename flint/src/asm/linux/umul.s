#******************************************************************************
#*                                                                            *
#* Functions for arithmetic and number theory with large integers in C        *
#* Software supplement to the book "Cryptography in C and C++"                *
#* by Michael Welschenbach                                                    *
#*                                                                            *
#* Module umul.s           Revision: 15.06.2002                               *
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
#*      Multiplication, interface compatible with C function umul_l()         *
#*                                                                            *
#*      Product := a * u                                                      *
#*                                                                            *
#*      Stack on calling of umul:        SP+12 ---> Offset product            *
#*                                       SP+ 8 ---> Offset a (CLINT)          *
#*                                       SP+ 4 ---> Offset u (USHORT)         *
#*                                       SP    ---> Return address            *
#*                                                                            *
#*      Return Value: ax = 0                                                  *
#*                                                                            *
#******************************************************************************
#
.equ    fct,        0                   # CLINT factor  (working copy)
.equ    prd,      520                   # Product       (working copy)
#
.equ    WORKSP,  1560                   # Working memory in stack area
#
.text
.globl  umul

umul:   pushl   %ebp                    # Store value for calling procedue
        movl    %esp,%ebp		                        
        subl    $WORKSP,%esp            # Local memory          
        movl    %esp,%eax               # Store starting address
        pushl   %edi
        pushl   %esi
        pushl   %ebx
        movl    %eax,%ebx
#
        xorl    %eax,%eax
        xorl    %edx,%edx

        movw    12(%ebp),%dx            # USHORT factor in dx
        cmpw    $0,%dx
        jne     .l1
        jmp     .umultz

.l1:    movl    8(%ebp),%esi            # Offset of variable a
        movw    (%esi),%ax
        cmpw    $0,%ax                  # a = 0 ?
        jne     .umult01
        jmp     .umultz
#
#>>>>>> Lade Operanden
#
.umult01: 
        leal    fct(%ebx),%edi          # Destination offset is a[ebx]
        movl    $0,%ecx
        movw    %ax,%cx                 # l(a) in ecx
        cld
        rep     
	movsw                           # Load a in chunks of USHORT into a
        movsw                           #  plus one USHORT
        movw    $0,(%edi)
#
#>>>>>> Prepare access to local memory
#
        pushl   %ebp                    # BP retten
        movl    %ebx,%ebp               # Basisind. Adr. in SS
#
#>>>>>> Remove leading zeros from operands
#
        xorl    %eax,%eax
        movw    fct(%ebp),%ax           # #USHORTs in ax
        cmpw    $0,%ax
        je      .end1
        shll    $1,%eax                 # #Bytes
        movl    %eax,%esi
.l2:    cmpw    $0,fct(%ebp,%esi)       # USHORT = 0 ?
        jne     .l3                     # If not, we're done
        subl    $2,%esi                 # Else: Step back one USHORT
        cmpl    $0,%esi                 # Index = 0?        
        je      .mazer1                 # Then argument = 0 
        jmp     .l2                     # Compare next digit
.l3: 
.mazer1: 
	shrl    %esi                    # #USHORTs
        movl    %esi,%eax               # Store number of digits 
        movw    %ax,fct(%ebp)
#
.end1:  cmpw    $0,%ax
        jne     .l4
        popl    %ebp
        jmp     .umultz
#
#>>>>>> Start multiplication
#
#-----> Initialize working memory
#
.l4:    movl    $2,%esi                 # Prepare index i
        movw    fct(%ebp),%cx           # l(a)
        shrw    $1,%cx
        jnc     .umult0
        incw    %cx
.umult0: 
	movl    $0,%eax
.umult1: 
	movl    %eax,prd(%ebp,%esi)
        addl    $4,%esi
        loop    .umult1
#
#-----> Innere Schleife
#
        movl    $2,%edi                 # Index j
        movw    fct(%ebp),%cx           # Prepare counter inner loop
        shrw    $1,%cx
        jnc     .umult2a
        incw    %cx
.umult2a: 
        xorl    %ebx,%ebx
        movw    %dx,%bx                 # Kepp USHORT factor in bx
        movl    $0,%edx                 # Dummy carry
        movl    $2,%esi
        movl    $2,%edi
.umult3: 
	pushl   %ecx                    # Store counter 
        movl    %edx,%ecx               # Carry u in dx
        movl    %ebx,%eax               # Load USHORT factor in ax
        mull    fct(%ebp,%edi)
        addl    %ecx,%eax               # ax + u
        adcl    $0,%edx
        addl    %eax,prd(%ebp,%esi)     # p[i+j-1]= ax + p[i+j-1]
        adcl    $0,%edx                 # New carry u in dx
        addl    $4,%edi                 # j=j+4
        addl    $4,%esi                 # i+j=i+j+4
        popl    %ecx                    # Get counter inner loop
        loop    .umult3
#
#-----> End inner loop
#
        movl    %edx,prd(%ebp,%esi)     # p[i+j-1]=u
        addl    $2,%esi
.umult3a: 
        cmpw    $0,prd(%ebp,%esi)
        jne     .umult4
        subl    $2,%esi
        jmp     .umult3a
.umult4: 
	movl    %esi,%ecx
        shrl    $1,%ecx
        movw    %cx,prd(%ebp)
#
#-----> Store product
#
        leal    prd(%ebp),%esi
        movl    %ebp,%ebx
        popl    %ebp
        movl    16(%ebp),%edi

        cmpw    $0,%cx
        je      .l5
        cld                             # Length l(p) is still in counter cx
        rep     
	movsw
.l5:    movsw
#
#*******************************************************************************
#       Security: Purge stack

.muret: movl    %ebx,%edi
        movl    $(WORKSP-4)/4,%ecx
        cld
        movl    $0,%eax                 # Return-Value = 0: OK
        rep     
	stosl                           # Overwrite with 0
#*******************************************************************************
#
        popl    %ebx
        popl    %esi                    # Restore registers
        popl    %edi
        movl    %ebp,%esp
        popl    %ebp
        ret
#
.umultz:                                # Product is 0
	movl    16(%ebp),%esi           
        movw    $0,(%esi)
        jmp     .muret
#




