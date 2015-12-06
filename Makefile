CC = sdcc
CPP = sdcpp
AS = sdas8051

FREQUENCY = 915
BAUD_RATE = 115200
BOARD = SRF_STICK
VERBOSE = 0

DEFINES = \
	-DBOARD=BOARD_$(BOARD) \
	-DFREQUENCY=$(FREQUENCY) \
	-DBAUD_RATE=$(BAUD_RATE) \
	-DVERBOSE=$(VERBOSE)
CODEFLAGS = --model-small --opt-code-speed --stack-auto
CFLAGS = $(DEFINES) $(CODEFLAGS) -Wp,-Wall,-MD,$(@:%.rel=%.d),-MT,$@

LDFLAGS = \
	--out-fmt-ihx \
	--code-loc 0x0000 --code-size 0x8000 \
	--xram-loc 0xF000 --xram-size 0x0F00 \
	--iram-size 0x0100

PROGRAMS = blinktest delaytest rxtest serialecho serialtest timetest txtest usbecho usbserial usbtest

LIBRARY = modules.lib

CFILES = 4b6b.c background.c clock.c crc.c debug.c delay.c dma.c led.c radio.c rf.c serial.c set_stdio.c usb.c usb_descriptor.c

all: $(PROGRAMS:%=%.hex)

OBJS = $(CFILES:%.c=%.rel)

$(LIBRARY): $(OBJS)
ifdef V
	sdcclib $@ $(OBJS)
else
	@echo AR $@ && sdcclib $@ $(OBJS)
endif

%.hex: %.rel $(LIBRARY)
ifdef V
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBRARY)
else
	@echo LD $@ && $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBRARY)
endif

# Keep program .rel files
.SECONDARY: $(PROGRAMS:%=%.rel)

%.rel: %.c
ifdef V
	$(CC) -c $(CFLAGS) -o $@ $<
else
	@echo CC $< && $(CC) -c $(CFLAGS) -o $@ $<
endif

BYPRODUCTS = *.asm *.d *.lib *.lk *.lnk *.lst *.map *.mem *.rel *.rst *.sym *~

clean:
	rm -f $(if $V,-v) $(BYPRODUCTS)

DEPS = $(PROGRAMS:%=%.d) $(CFILES:%.c=%.d)

-include $(DEPS)
