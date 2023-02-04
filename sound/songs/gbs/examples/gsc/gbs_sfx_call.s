	.include "asm/macros.inc"

	.section .rodata
	.global gbs_sfx_call
	.align 1

gbs_sfx_call_Ch5:
	gbs_switch 4
	pitch_sweep 6, 7
	square_note 4, 15, 7, 1952
	square_note 4, 15, 7, 1952
	square_note 4, 15, 7, 1952
	square_note 4, 15, 7, 1952
	square_note 4, 15, 7, 1952
	pitch_sweep 0, 8
	square_note 4, 0, 0, 0
	sound_ret

	.align 4
gbs_sfx_call:
	.byte 1	@ NumTrks
	.byte 0	@ NumBlks
	.byte 5	@ Priority
	.byte 0	@ Reverb

	.int voicegroup000

	.int gbs_sfx_call_Ch5
