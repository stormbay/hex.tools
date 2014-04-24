
OBJS =

CC    := $(CROSS_PREFIX)gcc
STRIP := $(CROSS_PREFIX)strip

CFLAGS  += -Wall
LDFLAGS +=

OBJS += main.o	\
		swap.o

#FORCE_STATIC := --static

LOCAL_MODULE  := hextools

SWAP_SYM_MODULE  := hexswap

CFLAGS  += -DSYMLINK_NAME_HEXSWAP=\"$(SWAP_SYM_MODULE)\"


.PHONY: all
all: $(LOCAL_MODULE)

.PHONY: clean
clean: 
	@rm -f $(OBJS) $(LOCAL_MODULE)
	@rm -f $(SWAP_SYM_MODULE)

$(OBJS):%o:%c *.h
	@$(CC) -c $(CFLAGS) $< -o $@

$(LOCAL_MODULE): $(OBJS)
	@echo "--Compiling '$(LOCAL_MODULE)' ..."
	@$(CC) $^ -o $@ $(FORCE_STATIC)
	@$(STRIP) $@
	@ln -sf $@ $(SWAP_SYM_MODULE)

