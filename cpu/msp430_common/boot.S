.file "boot.S"                  ; useful for error messages :-)
        
;;;------------------------------------------------------------
	.section .__riot_resetvec, "a"
	.word __riot_boot_start

;;;------------------------------------------------------------
	.section .boot_text, "ax",@progbits
	.global __riot_boot_start
	.type __riot_boot_start, @function

;;;------------------------------------------------------------
.align(2)
__riot_boot_start:
	mov #LINKER_isr_sp, R1  ; constant provided by ldscript 
	br #riot_boot
