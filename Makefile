FASTROM=1

ifeq ($(strip $(PVSNESLIB_HOME)),)
$(error "Please create an environment variable PVSNESLIB_HOME by following this guide: https://github.com/alekmaul/pvsneslib/wiki/Installation")
endif

#---------------------------------------------------------------------------------
# ROMNAME is used in snes_rules file
export ROMNAME := spdemo
export ROMTITLE := SPRITE DEMO
export COUNTRY := 0

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules

.PHONY: all

all: bitmaps buildWithSummary
buildActual: $(OBJS) $(ROMNAME).sfc

clean: cleanBuildRes cleanRom cleanGfx

resource_sp.pic: resource_sp.bmp
	$(GFXCONV) -s 8 -o 16 -u 16 -p -t bmp -i $<

resource_font.pic: resource_font.bmp
	$(GFXCONV) -s 8 -o 16 -u 16 -p -t bmp -i $<

bitmaps : resource_sp.pic resource_font.pic
