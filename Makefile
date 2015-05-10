CC = sdcc
CPP = sdcpp
AS = sdas8051

BOARD = SRF_STICK
BAUD_RATE = 115200

DEFINES = -DBOARD=BOARD_$(BOARD) -DBAUD_RATE=$(BAUD_RATE)
INCLUDES =
CODEFLAGS = --model-small --opt-code-speed
CFLAGS = $(DEFINES) $(INCLUDES) $(CODEFLAGS) -Wp,-Wall,-MD,$(@:%.rel=%.d),-MT,$@

LDFLAGS = --out-fmt-ihx \
	  --code-loc 0x000 --code-size 0x8000 \
	  --xram-loc 0xF000 --xram-size 0xF00 \
	  --iram-size 0x100

PROGRAMS = blinktest delaytest mmtest serialtest

LIBRARY = modules.lib

CFILES = clock.c delay.c led.c radio.c serial.c

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
