.include "hdr.asm"

.section ".rodata1" superfree

gfxpsrite:
.incbin "resource_sp.pic"
gfxpsrite_end:

palsprite:
.incbin "resource_sp.pal"
palsprite_end:

tilfont:
.incbin "resource_font.pic"
tilfont_end:

palfont:
.incbin "resource_font.pal"
palfont_end:

.ends

