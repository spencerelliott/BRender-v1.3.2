;; Copyright (c) 1992,1994-1995 Argonaut Technologies Limited. All rights reserved.
;;
;; $Id: magicsym.asm 1.1 1997/12/10 16:41:08 jon Exp $
;; $Locker: $
;;
;; Define any magic symbols generated by compilers - needed when not
;; linking against vendors runtime
;;
	.486p
	.model flat
ifdef __HIGHC__
	.data
	public _mwloc
_mwloc			dd	0

endif

ifdef __WATCOMC__
ifdef __TARGET_MSC__

	.data
	; Pull in uSoft RTL
	;
	EXTRN	__fltused:NEAR

	; Supress Watcom RTL gubbins
	;

	public __8087
	public __init_387_emulator
	public _fltused_

__8087			dd	0
__init_387_emulator	dd	0
_fltused_		dd	0

	.code
                PUBLIC  __CHP 

__CHP:          push    eax
                fstcw   [esp]
                wait    
                push    dword ptr [esp]
                mov     byte ptr 1[esp],01fh
                fldcw   [esp]
                frndint 
                fldcw   4[esp]
                wait    
                lea     esp,8[esp]
                ret
endif
endif

		.code
	public __PRO
	public __EPI
__PRO label near
__EPI label near
		ret	

  	end

		 