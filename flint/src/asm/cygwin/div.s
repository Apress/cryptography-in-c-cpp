#******************************************************************************
#*                                                                            *
#* Functions for arithmetic and number theory with large integers in C        *
#* Software supplement to the book "Cryptography in C and C++"                *
#* by Michael Welschenbach                                                    *
#*                                                                            *
#* Module div.s            Revision: 15.06.2002                               *
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
#*      Division, interface compatible with C-function div_l()                *
#*                                                                            *
#*      Quotient := Dividend div Divisor                                      *
#*      Remainder := Dividend mod Divisor                                     *
#*                                                                            *
#*      Stack on calling of div_l:        SP+16 ---> Offset remainder         *
#*                                        SP+12 ---> Offset quotient          *
#*                                        SP+ 8 ---> Offset divisor           *
#*                                        SP+ 4 ---> Offset dividend          *
#*                                        SP    ---> Return address           *
#*                                                                            *
#*      Return Value: ax = -1 if divisor = 0                                  *
#*                    ax =  0 else                                            *
#*                                                                            *
#******************************************************************************
#
.equ    a,      0		# Dividend (working copy)                  
.equ    b,      1030		# Divisor  (working copy)                  
.equ    q,      1546		# Quotient qhat (working memory)           
.equ    q1,     2566		# High-order digit of divisor (normalized) 
.equ    v1,     2570		# 2nd digit of divisor (normalized)        
.equ    v2,     2574		
.equ    d,      2578		# Exponent for normalization
.equ    uj1,    2582		# uj+1 (normalized)                        
.equ    uj2,    2586            # uj+2 (normalized)                        
#
.equ    WORKSP, 2600		# Working memory in stack area
#
.text
.globl  _div_l

_div_l: pushl   %ebp		# Store value for calling procedure
        movl    %esp,%ebp
        subl    $WORKSP,%esp    # lokal working memory
        movl    %esp,%eax
        pushl   %ebx
        pushl   %edi
        pushl   %esi
        movl    %eax,%ebx       # Store start address of working memory
#
        movl    8(%ebp),%esi    # Offset of variable a
        movl    12(%ebp),%edi   # Offset of variable b
        movw    (%esi),%ax      # l(a)
        movw    (%edi),%dx      # l(b)
        cmpw    $0,%dx		# b = 0 ?
        jne     .l1
        jmp     .divbyz		# Division by zero!
#
#>>>>>> Lade Operanden
#
.l1:    pushl   %edi		# Store offset b
        leal    a(%ebx),%edi    # Destination offset is a[bx]
        xorl    %ecx,%ecx
        movw    %ax,%cx		# l(a) in cx
        shrw    $1,%cx
        jnc     .l2
        incw    %cx
.l2:    cld     
	rep     
	movsl			# Load a as ULONG
        movsw			#  plus one USHORT
        movw    %ax,%cx
        shlw    $1,%cx
        leal    a+2(%ebx,%ecx),%edi
        movw    $0,(%edi)
#
        popl    %esi		# Hole Seg:Offs von b
        leal    b(%ebx),%edi    # Zieloffset ist b[bx]
        xorl    %ecx,%ecx
        movw    %dx,%cx		# l(b) in cx
        shrw    $1,%cx
        jnc     .l3
        incw    %cx
.l3:    cld     
        rep     
	movsl			# Load b as ULONG
        movsw			#  plus one USHORT
        movw    %dx,%cx
        shlw    $1,%cx
        leal    b+2(%ebx,%ecx),%edi
        movl    $0,(%edi)
#
#>>>>>> Prepare access to local memory
#
        pushl   %ebp		# Store ebp
        movl    %ebx,%ebp       # Indexed addressing based on ss
#
#>>>>>> Remove leading zeros from operands
#
        xorl    %eax,%eax
        xorl    %ecx,%ecx
        movw    a(%ebp),%ax     # #USHORTs into ax
        cmpw    $0,%ax
        je      .next1
        shll    $1,%eax		# Index to low-order byte
                                # of high-order digit
        movl    %eax,%esi
.l4:    cmpw    $0,a(%ebp,%esi) # USHORT = 0 ?
        jne     .l5		# If not we're done
        subl    $2,%esi		# Else:	 Step back one digit
        cmpl    $0,%esi		# Index = 0?
        je      .mazer1		# Then argument = 0
        jmp     .l4		# Next comparison    
.l5:				# Length determined
.mazer1:
	shrl    $1,%esi		# #Digits
        movl    %esi,%eax       # Basisadresse des Operanden holen
        movw    %ax,a(%ebp)     # Store number of digits

#Operand2
.next1: movw    b(%ebp),%ax     # #USHORTs in ax
        cmpw    $0,%ax
        je      .end1
        shll    $1,%eax		# #Bytes, index to low-order byte
                                # of high-order digit
        movl    %eax,%esi
.l6:    cmpw    $0,b(%ebp,%esi) # Digit = 0 ?
        jne     .l7		# If not we're done
        subl    $2,%esi		# Else: Step back one digit
        cmpl    $0,%esi		# Index = 0?            
        je      .mazer2		# Then argument = 0     
        jmp     .l6		# Next comparison       
.l7:				# Length found          
.mazer2:
	shrl    $1,%esi		# Number of digits
        movl    %esi,%eax       # Store number of digits
        movw    %ax,b(%ebp)     

.end1:  cmpw    $0,%ax
        jne     .l8
        popl    %ebp
        jmp     .divbyz
.l8:    cmpw    $0,a(%ebp)
        jne     .l9
        popl    %ebp
        jmp     .divz
#
#>>>>>> Test a < b ?
#
.l9:    movl    $0,%ecx
        movw    a(%ebp),%cx
        cmpw    b(%ebp),%cx     # l(a) - l(b)
        jnc     .div03
        jmp     .dra
.div03: jne     .div05          # If lengths are equal compare digits
        movl    %ecx,%eax       # cl = l(a) = l(b)
        shll    $1,%eax
        leal    a(%ebp),%esi
        leal    b(%ebp),%edi
        addl    %eax,%edi       # si points to high-order digit of a
        addl    %eax,%esi       # di points to high-order digit of b
        std     
        repe    
	cmpsw   
#	xchg    %esi,%edi
        cld     
        jnc     .div05		# If no carry occurs, q is positive
        jmp     .dra		# Else q := 0 and r := a
.div05: movw    b(%ebp),%ax
        shrw    $1,%ax
        jnc     .div05a
        incw    %ax
.div05a:
	cmpw    $1,%ax		# Test if length l(b) = 1
        jne     .dstart
        jmp     .dshort		# If so go to short division
#
#>>>>>> Start division
#
.dstart:
	movl    $0,%eax
        movw    b(%ebp),%ax
        shrw    $1,%ax
        jnc     .dm0
        incw    %ax
.dm0:   movl    %eax,%esi
        shll    $2,%esi
        subl    $2,%esi		# Pointer to low byte of b[l(b)]    
				# (pointer to high-order ULONG)     
        cmpl    $10,%esi	# Does divisor have 3 or more digits?
        jb      .dm1a		# Two digits are present at mininum!

        movl    b(%ebp,%esi),%ebx       # b[l(b)] in bx
        movl    b-4(%ebp,%esi),%eax
        movl    b-8(%ebp,%esi),%edx
#
        movl    $0,%ecx		# Prepare counter
#
.l10:   cmpl    $0x080000000,%ebx       # v1 >= 2^31 ?
        jae     .dm2		# If not ...
        incw    %cx
        clc     
        rcll    $1,%edx
        rcll    $1,%eax
        rcll    $1,%ebx		# v1 = v1 * 2
        jmp     .l10		# until v1 >= 2^31

.dm1a:  movl    b(%ebp,%esi),%ebx       # b[l(b)] in bx
        movl    b-4(%ebp,%esi),%eax
#
        movl    $0,%ecx		# Prepare counter
#
.l11:   cmpl    $0x080000000,%ebx       # v1 >= 2^31 ?
        jae     .dm2		# If not ...
        incw    %cx
        clc     
        rcll    $1,%eax
        rcll    $1,%ebx		# v1 = v1 * 2
        jmp     .l11		# until v1 >= 2^31
#
.dm2:   movw    %cx,d(%ebp)     # Store exponent
        movl    %ebx,v1(%ebp)   # v1
        movl    %eax,v2(%ebp)   # v2

.dm3:   incw    a(%ebp)		# l(a) = l(a) + 2
        incw    a(%ebp)
        movl    $0,%eax
        movw    a(%ebp),%ax
        shrw    $1,%ax
        jnc     .dm3a
        incw    %ax
.dm3a:  movl    %eax,%esi
        shll    $2,%esi
        subl    $2,%esi
        movl    $0,a(%ebp,%esi) # a[l(a)] = 0
#
.d2:    movl    $0,%eax
        movw    b(%ebp),%ax
        shrw    $1,%ax
        jnc     .d2a
        incw    %ax
.d2a:   movl    %eax,%esi
        shll    $2,%esi
        subl    $2,%esi		# si points to loByte of b[l(b)]
#
        movl    $0,%ecx
        movw    a(%ebp),%cx
        shrw    $1,%cx
        jnc     .d2b
        incw    %cx
.d2b:   movl    %ecx,%edi
        shll    $2,%edi
        subl    $2,%edi		# di points to loByte of a[l(a)]
        pushl   %edi
        subl    %esi,%edi
        incl    %edi		# di points to hiByte of a[l(a)-l(b)]
        movl    %edi,%ecx
        shrl    $2,%ecx		# Counter in cx
        popl    %esi		# si points to loByte of a[l(a)] (j+l(b))
        subl    $3,%edi		# di points to loByte of a[l(a)-l(b)] (j)
        pushl   %edi		# Possible length of q
#
#-----> Main loop of division
#
.d3:    cmpl    $14,%esi
        jb      .d3a
        pushl   %edi
        movl    a(%ebp,%esi),%edx      # uj   unshifed in dx (hiWord)
        movl    a-4(%ebp,%esi),%eax    # uj+1 unshifted
        movl    a-8(%ebp,%esi),%ebx    # uj+2 unshifted
        movl    a-12(%ebp,%esi),%edi   # uj+3 unshifted
        pushl   %ecx
        movw    d(%ebp),%cx
.l12:   cmpw    $0,%cx
        je      .l13
        rcll    $1,%edi
        rcll    $1,%ebx
        rcll    $1,%eax
        rcll    $1,%edx
        decw    %cx
        jmp     .l12
.l13:   popl    %ecx
        popl    %edi
        jmp     .qhat
#
.d3a:   movl    a(%ebp,%esi),%edx      # uj   unshifted
        movl    a-4(%ebp,%esi),%eax    # uj+1 unshifted
        movl    a-8(%ebp,%esi),%ebx    # uj+2 unshifted
        pushl   %ecx
        movw    d(%ebp),%cx
.l14:   cmpw    $0,%cx
        je      .l15
        rcll    $1,%ebx
        rcll    $1,%eax
        rcll    $1,%edx
        decw    %cx
        jmp     .l14
.l15:   popl    %ecx
#
#-----> Calculate and test qhat
#
.qhat:  movl    %eax,uj1(%ebp)  # Store shifted uj+1 
        movl    %ebx,uj2(%ebp)  # Store shifted uj+2
        movl    v1(%ebp),%ebx   # v1 in bx
        cmpl    %edx,%ebx
        je      .bm1		# If v1 = uj q = b - 1
#
        divl    %ebx		# eax <- qhat
# edx <- rhat: = uj*b + uj+1 -qhat*v1 = (uj*b + uj+1) mod v1
        movl    %eax,q1(%ebp)   # Store qhat
        cmpl    $0,%eax
        jne     .l16
        jmp     .dml		# If q = 0 process next digit
#
.bm1:   movl    $0x0ffffffff,%eax       # eax <- qhat
        movl    %eax,q1(%ebp)   # Store qhat
        movl    uj1(%ebp),%edx  # edx <- uj+1
        addl    v1(%ebp),%edx   # edx <- uj+1 + v1 =: rhat
        jc      .d4		# rhat >= b => v2 * qhat < rhat * b
#
.l16:   movl    %edx,%ebx       # ebx <- rhat
        mull    v2(%ebp)        # eax <- low(qhat * v2), edx <- high(qhat * v2)
        cmpl    %ebx,%edx       # rhat >= high(qhat * v2) ?
        jb      .d4		# CF == 1?  Then we're done
        ja      .l17		# rhat < high(qhat * v2) => decrement qhat
        cmpl    uj2(%ebp),%eax  # uj+2 >= low(qhat * v2) ?
        jbe     .d4		# CF == 1 OR ZF == 1? Then we're done
#
.l17:   decl    q1(%ebp)        # Correct qhat--
        addl    v1(%ebp),%ebx   # ebx <- rhat + v1
        jc      .d4		# rhat >= b => v2 * qhat < rhat * b
        subl    v2(%ebp),%eax   # (qhat * v2) - v2
        sbb     $0,%edx
#
        cmpl    %ebx,%edx       # rhat >= high(qhat * v2) ?
        jb      .d4		# CF == 1?  Then we're done
        ja      .l17		# rhat < high(qhat * v2) => decrement qhat
        cmpl    uj2(%ebp),%eax  # uj+2 >= low(qhat * v2) ?
        ja      .l17     # CF==0 + ZF==0? => Repeat decrement of qhat
#
.d4:    pushl   %ecx		# Store main counter
        pushl   %esi		# Store j + l(b) 
        pushl   %edi		# Store j
        movw    b(%ebp),%cx     # Load counter with l(b)
        shrw    $1,%cx
        jnc     .d4a
        incw    %cx
.d4a:   movl    q1(%ebp),%ebx   # q in bx
        movl    $2,%esi
        movl    $0,%edx		# Dummy carry
#
#-----> Multiplication and subtraction
#
.dms:   push    %edx
        movl    %ebx,%eax       # q in ax
        mull    b(%ebp,%esi)    # hi:dx lo:ax
        subl    %eax,a(%ebp,%edi)
        adcl    $0,%edx		# Carry to next subtrahend
        popl    %eax
        subl    %eax,a(%ebp,%edi)
        adcl    $0,%edx		# Carry to next subtrahend
        incl    %edi
        incl    %edi
        incl    %edi
        incl    %edi
        incl    %esi
        incl    %esi
        incl    %esi
        incl    %esi
        loop    .dms
        subl    %edx,a(%ebp,%edi)
        jnc     .dnc		# No correction necessary if carry = 0
#
#-----> Correction
#
        popl    %edi
        pushl   %edi
        xorl    %ecx,%ecx
        movw    b(%ebp),%cx     # Load counter with l(b)
        shrw    $1,%cx
        jnc     .d4b
        incw    %cx
.d4b:   movl    $2,%esi
        clc     
.d5:    movl    b(%ebp,%esi),%eax       # b[i]
        adcl    %eax,a(%ebp,%edi)       # a[j+i] + b[i]
        incl    %edi
        incl    %edi
        incl    %edi
        incl    %edi
        incl    %esi
        incl    %esi
        incl    %esi
        incl    %esi
        loop    .d5		# Outer loop
        jnc     .d51
        incl    a(%ebp,%edi)
.d51:   decl    q1(%ebp)        # q = q - 1
#
#
.dnc:   popl    %edi		# Get pointers and counters
        popl    %esi
        popl    %ecx
.dml:   movl    q1(%ebp),%eax   # Get q
        movl    %eax,q(%ebp,%edi)       # q[j] = q
        subl    $4,%edi
        subl    $4,%esi
        decl    %ecx		# Inner loop
        jz      .d6
        jmp     .d3
#
.d6:    popl    %edi		# Load length l(a)-l(b)-1 in ULONGs
        addl    $2,%edi		# di points to loByte of last USHORT
        movl    $0,%eax
.l18:   cmpw    q(%ebp,%edi),%ax        # q[l(a)-l(b)]=0 ?
        jne     .d7		# If so ...
        decl    %edi
        decl    %edi		# ... l(q) = l(a) - l(b) - 1
        cmpl    $0,%edi
        jne     .l18
.d7:    movl    %edi,%edx
        shrl    $1,%edx		# l(q) (#USHORTs) in dx
        movw    %dx,q(%ebp)     # Store l(q)
#
#-----> Determine length of remainder
#
        movl    $0,%eax
        movw    b(%ebp),%ax
        movl    %eax,%ecx
        shlw    $1,%ax
        movl    %eax,%edi
        movl    $0,%ebx
        incl    %edi
        incl    %edi
.d8:    decl    %edi
        decl    %edi		# di points to loByte of a[l(b)]
        cmpw    %bx,a(%ebp,%edi)
        loope   .d8
        jz      .d9		# If a[bp][di]!= 0 ...
        incw    %cx		# ... l has to be incremented
.d9:    movw    %cx,a(%ebp)     # Store l(r)
#
#-----> Store results
#
.dstore: 
	movl    %ebp,%esi
        movl    %ebp,%ebx
        popl    %ebp
        pushl   %esi
        addl    $q,%esi
        movl    16(%ebp),%edi   # Offset of quotient
        movl    $0,%ecx
        movw    (%esi),%cx
        shrw    $1,%cx
        jnc     .l19
        incw    %cx
.l19:   cld     
        cmpw    $0,%cx
        je      .qzero
        rep     
	movsl   
.qzero: movsw   
#
#-----> Store remainder
#
        popl    %esi
.drs:            
        movl    20(%ebp),%edi
        addl    $a,%esi
        movl    $0,%ecx
        movw    (%esi),%cx
        shrw    $1,%cx
        jnc     .l20
        incw    %cx
.l20:   cld     
        cmpw    $0,%cx
        je      .rzero
        rep     
	movsl   
.rzero: movsw   
        movl    $0,%eax		# Return-Value = 0: OK
#
#*******************************************************************************
#       Security: Purge stack

divret: movl    %ebx,%edi
        movl    $(WORKSP-4)/4,%ecx
        cld     
        rep     
	stosl			# Overwrite with  0 or -1
#*******************************************************************************
#
        popl    %esi
        popl    %edi
        popl    %ebx
        movl    %ebp,%esp
        popl    %ebp
        ret     
#
#
.divz:  movl    $0,%eax		# Return-Value = 0: Everything OK 
        movl    20(%ebp),%edi   # Destination Offset of remainder 
        movl    16(%ebp),%esi   # Destination offset of quotient  
        movw    %ax,(%edi)      # Remainder = 0                   
        movw    %ax,(%esi)      # Quotient = 0                    
        jmp     divret
#
.divbyz: 
	movl    $-1,%eax
        jmp     divret		# Return-Value = -1: Division by zero
#
#-----> Case q = 0 and r = a
#
.dra:   movl    %ebp,%esi
        movl    %ebp,%ebx
        popl    %ebp
        movl    16(%ebp),%edi
        movw    $0,(%edi)       # q = 0
        jmp     .drs		# Store remainder
#
#
#>>>>>> Short Division
#
#
.dshort: 
	movl    $0,%ecx
        movw    a(%ebp),%cx
        shrw    $1,%cx
        jnc     .dsh0
        incw    %cx
.dsh0:  movl    %ecx,%edi
        shll    $2,%edi
        subl    $2,%edi
        movl    $0,%edx
        movl    b+2(%ebp),%ebx
.dsh1:  movl    a(%ebp,%edi),%eax
        divl    %ebx
        movl    %eax,q(%ebp,%edi)
        subl    $4,%edi
        loop    .dsh1
.dsh2:  movl    $0,%ecx
        movw    a(%ebp),%cx
        movl    %ecx,%esi
        shll    $1,%esi
.l21:   movw    q(%ebp,%esi),%bx
        cmpw    $0,%bx
        jne     .dsh3
        decl    %esi
        decl    %esi
        decl    %ecx
        cmpw    $0,%cx
        jne     .l21
.dsh3:  movw    %cx,q(%ebp)
        movl    %edx,a+2(%ebp)
        movw    $2,a(%ebp)
        cmpw    $0,a+4(%ebp)
        jne     .dst
        movw    $1,a(%ebp)
        cmpw    $0,a+2(%ebp)
        jne     .dst
        movw    $0,a(%ebp)
.dst:   jmp     .dstore
#



