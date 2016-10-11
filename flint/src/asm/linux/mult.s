#******************************************************************************
#*                                                                            *
#* Functions for arithmetic and number theory with large integers in C        *
#* Software supplement to the book "Cryptography in C and C++"                *
#* by Michael Welschenbach                                                    *
#*                                                                            *
#* Module mult.s           Revision: 15.06.2002                               *
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
#*      Multiplication, interface compatible with C function mul_l()          *
#*                                                                            *
#*      Product := f1 * f2                                                    *
#*                                                                            *
#*      Stack on calling of mul:         SP+12 ---> Offset product            *
#*                                       SP+ 8 ---> Offset f2                 *
#*                                       SP+ 4 ---> Offset f1                 *
#*                                       SP    ---> Return address            *
#*                                                                            *
#*      Return Value : ax = 0                                                 *
#*                                                                            *
#******************************************************************************
#
.equ    a,      0                       # CLINT factor 1 (working copy)     
.equ    b,      520                     # CLINT factor 2 (working copy)     
.equ    p,      1040                    # Product (storage space for result)
#
.equ    WORKSP, 2200                    # Working memory in stack area
#
.text
.globl  mult

mult:   pushl   %ebp                    # Store base ptr for caller
        movl    %esp,%ebp		                          
        subl    $WORKSP,%esp            # Local memory            
        movl    %esp,%eax               #   Store starting address
        pushl   %edi
        pushl   %esi
        pushl   %ebx
        movl    %eax,%ebx
#
        xorl    %eax,%eax
        xorl    %edx,%edx
        movl    8(%ebp),%esi            # Parameter f1
        movl    12(%ebp),%edi           # Parameter f2
        movw    (%esi),%ax
        movw    (%edi),%dx

        cmpw    $0,%ax                  # a = 0 ?
        jne     .mult01
        jmp     .multz
.mult01:
	cmpw    $0,%dx                  # b = 0 ?
        jne     .mult02                 # Then we're done!
        jmp     .multz
#
#>>>>>> Load operands
#
.mult02: 
	pushl   %edi                    # Store Seg:Offs f2         
        leal    b(%ebx),%edi            # Destination offset is b[ebx]
        movl    $0,%ecx			                            
        movw    %ax,%cx                 # l(a) (length of a) in ecx 
        cld				                            
rep     movsw                           # Load a in chunks of USHORT
        movsw                           #  plus one USHORT            
        movw    $0,(%edi)		                            
        popl    %esi                    # Get offset of b
	leal    a(%ebx),%edi            # Destination offset is a[ebx]
        movw    %dx,%cx                 # l(b) in cx                
        cld				  
rep     movsw                           # Load b in chunks of USHORT
        movsw                           #   plus one USHORT
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
        movw    b(%ebp),%ax             # #USHORTs in ax
        cmpw    $0,%ax
        je      .next1
        shll    %eax
        movl    %eax,%esi               # Index to high-order digit  
.l1:    cmpw    $0,b(%ebp,%esi)		# USHORT = 0 ?               
        jne     .l2                     # If not, then finished      
        subl    $2,%esi                 # Else: step back one USHORT 
        cmpl    $0,%esi                 # Index = 0 ?                
        je      .mazer1                 # Then argument = 0          
        jmp     .l1                     # Compare next               
.l2: 
.mazer1:
	shrl    %esi                    # #USHORTs
        movl    %esi,%eax               # Store number of digits
        movw    %ax,b(%ebp)

#Operand2
.next1: movw    a(%ebp),%ax             # #USHORTs in ax
        cmpw    $0,%ax
        je      .end1
        shll    %eax                    # Index to high-order digit
        movl    %eax,%esi
.l3:    cmpw    $0,a(%ebp,%esi)         # Digit = 0 ?                  
        jne     .l4                     # If not, then finished        
        subl    $2,%esi                 # Else: step back one USHORT   
        cmpl    $0,%esi                 # Index = 0 ?                  
        je      .mazer2                 # Then argument = 0            
        jmp     .l3                     # Compare next                 
.l4: 					
.mazer2: 
	shrl    %esi                    # #Digits
        movl    %esi,%eax               # ... store number of digits
        movw    %ax,a(%ebp)

.end1:  cmpw    $0,%ax
        jne     .l5
        popl    %ebp
        jmp     .multz
.l5:    cmpw    $0,b(%ebp)
        jne     .l6
        popl    %ebp
        jmp     .multz
#
#>>>>>> Start multiplication
#
#-----> Initialize working memory
#
.l6:    movl    $2,%esi                 # Prepare index i
        movw    %dx,%cx                 # l(b)
        shrw    %cx
        jnc     .mult0
        incw    %cx
.mult0: movl    $0,%eax
.mult1: movl    %eax,p(%ebp,%esi)
        addl    $4,%esi
        loop    .mult1
#
#-----> Outer loop
#
        movl    $2,%esi                 # Index i
        movl    $0,%ecx
        movw    b(%ebp),%cx             # Prepare counter for outer loop
        shrw    %cx
        jnc     .mult2
        incw    %cx
.mult2: pushl   %ecx                    # Store counter
#
#-----> Inner loop
#
        movl    $2,%edi                 # Index j
        movw    a(%ebp),%cx             # Prepare counter for inner loop
        shrw    %cx
        jnc     .mult2a
        incw    %cx
.mult2a: 
	xorl    %edx,%edx               # Dummy-carry
        movl    b(%ebp,%esi),%ebx       # Load b[i]
        addl    %edi,%esi               # Product index i+j
.mult3: pushl   %ecx                    # Store counter 
        movl    %edx,%ecx               # carry u in dx
        movl    %ebx,%eax
        mull    a(%ebp,%edi)
        addl    %ecx,%eax               # ax + u
        adcl    $0,%edx
        addl    %eax,p-2(%ebp,%esi)     # p[i+j-1]= ax + p[i+j-1]
        adcl    $0,%edx                 # New carry u in dx
        addl    $4,%edi                 # j=j+4
        addl    $4,%esi                 # i+j=i+j+4
        popl    %ecx                    # Get counter for inner loop
        loop    .mult3
#
#-----> End inner loop
#
        movl    %edx,p-2(%ebp,%esi)     # p[i+j-1]=u
        subl    %edi,%esi               # Restore i
        addl    $4,%esi                 # i=i+4
        popl    %ecx                    # Get counter for outer loop
        loop    .mult2
#
#-----> End outer loop
#
        addl    %edi,%esi
        subl    $4,%esi
.mult3a: 
	cmpw    $0,p(%ebp,%esi)
        jne     .mult4
        subl    $2,%esi
        jmp     .mult3a
.mult4: movl    %esi,%ecx
        shrl    %ecx
        movw    %cx,p(%ebp)
#
#-----> Store product
#
        leal    p(%ebp),%esi
        movl    %ebp,%ebx
        popl    %ebp
        movl    16(%ebp),%edi
        cmpw    $0,%cx
        je      .l7
        cld                             # l(p) is still in counter cx
rep     movsw
.l7:    movsw
#
#*******************************************************************************
#       Security: Purge stack

.muret: movl    %ebx,%edi
        movl    $(WORKSP-4)/4,%ecx
        cld
        movl    $0,%eax                 # Return-Value = 0: OK
rep     stosl                           # Overwrite with 0
#*******************************************************************************
#
        popl    %ebx
        popl    %esi                    # Restore registers for caller
        popl    %edi
        movl    %ebp,%esp
        popl    %ebp
        ret
#
.multz: movl    16(%ebp),%esi           # Product is 0
        movw    $0,(%esi)
        jmp     .muret
#



