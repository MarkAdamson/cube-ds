.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

GAME_TITLE	:=	Rubik's Cube for DS
GAME_SUBTITLE1	:=	v0.7
GAME_SUBTITLE2	:=	by Mark Adamson
GAME_ICON      :=      ../cube2.bmp
include $(DEVKITARM)/ds_rules
TARGET		:=	$(shell basename $(CURDIR))
BUILD		:=	build
SOURCES		:=	source
DATA		:=	data  
INCLUDES	:=	include
GRAPHICS	:=	gfx

ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -O2\
 			-march=armv5te -mtune=arm946e-s -fomit-frame-pointer\
			-ffast-math \
			$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM9
CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=ds_arm9.specs -g $(ARCH) -mno-fpu -Wl,-Map,$(notdir $*.map)

LIBS	:= -L$(DEVKITPRO)/libwoopsi/lib -lwoopsi -lfat -lnds9 -lpng -lz -ljpeg
#LIBS		+= -lfat -lnds9
 
LIBDIRS	:=	$(LIBNDS)
LIBDIRS		+= $(DEVKITPRO)/libwoopsi
LIBDIRS += $(PORTLIBS)
 
ifneq ($(BUILD),$(notdir $(CURDIR)))
 
export OUTPUT	:=	$(CURDIR)/$(TARGET)
 
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
PNGFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.png)))
 
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(PNGFILES:.png=.o) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)
 
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)
 
.PHONY: $(BUILD) clean
 
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
 
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).arm9 $(TARGET).ds.gba 
 
 
else
 
DEPENDS	:=	$(OFILES:.o=.d)
 
$(OUTPUT).nds	: 	$(OUTPUT).arm9
$(OUTPUT).arm9	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)

%.bin.o	:	%.bin
	@echo $(notdir $<)
	@$(bin2o)

%.s %.h	: %.png %.grit
	grit $< -fts -o$*


-include $(DEPENDS)
 
endif
