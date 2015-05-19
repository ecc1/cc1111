CC = sdcc
CPP = sdcpp
AS = sdas8051

FREQUENCY = 915
BAUD_RATE = 115200
BOARD = SRF_STICK

DEFINES = -DFREQUENCY=$(FREQUENCY) -DBAUD_RATE=$(BAUD_RATE) -DBOARD=BOARD_$(BOARD)
INCLUDES =
CODEFLAGS = --model-small --opt-code-speed --stack-auto
CFLAGS = $(DEFINES) $(INCLUDES) $(CODEFLAGS) -Wp,-Wall,-MD,$(@:%.rel=%.d),-MT,$@

LDFLAGS = --out-fmt-ihx \
	  --code-loc 0x000 --code-size 0x8000 \
	  --xram-loc 0xF000 --xram-size 0xF00 \
	  --iram-size 0x100

PROGRAMS = blinktest delaytest rfinttest rxtest serialtest timetest

LIBRARY = modules.lib

CFILES = 4b6b.c clock.c crc.c delay.c led.c radio.c serial.c

all: $(PROGRAMS:%=%.hex)

OBJS = $(CFILES:%.c=%.rel)

$(LIBRARY): $(OBJS)
ifdef VERBOSE
	sdcclib $@ $(OBJS)
else
	@echo AR $@ && sdcclib $@ $(OBJS)
endif

%.hex: %.rel $(LIBRARY)
ifdef VERBOSE
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBRARY)
else
	@echo LD $@ && $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBRARY)
endif

# Keep program .rel files
.SECONDARY: $(PROGRAMS:%=%.rel)

%.rel: %.c
ifdef VERBOSE
	$(CC) -c $(CFLAGS) -o $@ $<
else
	@echo CC $< && $(CC) -c $(CFLAGS) -o $@ $<
endif

BYPRODUCTS = *.asm *.d *.lib *.lk *.lnk *.lst *.map *.mem *.rel *.rst *.sym *~

clean:
	rm -f $(if $(VERBOSE),-v) $(BYPRODUCTS)

DEPS = $(PROGRAMS:%=%.d) $(CFILES:%.c=%.d)

-include $(DEPS)
