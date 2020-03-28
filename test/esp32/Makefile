# unit testing

unity_dir = $(build_dir)/unity

COMP_DIRS := $(realpath ../../components ./components)
COMPS := $(foreach dir,$(COMP_DIRS),$(shell cd $(dir) && ls))


####### ESP32 build command ############
build_opts := -G Ninja 
ifdef V
build_opts += -v
endif
ifndef BUILD_BASE
build_dir := ./build
else
build_opts += -B $(BUILD_BASE)
build_dir := $(BUILD_BASE)
endif
build_opts += -C $(build_dir)/unity
build_cmd := env COMP_DIRS="$(COMP_DIRS)" idf.py $(build_opts)


.PHONY: build clean flash run all unity unity-clean

unity: $(unity_dir)/unity.stamp

unity-clean:
	-test -f $(unity_dir)/unity.stamp && rm -r $(unity_dir)

port ?= /dev/ttyUSB0



$(unity_dir)/unity.stamp:
	mkdir -p $(unity_dir)
	cp -upr $(IDF_PATH)/tools/unit-test-app/* $(unity_dir)
	patch -s -p1 -d $(unity_dir) < ./unity.diff
	touch $(unity_dir)/unity.stamp

build: unity
	$(build_cmd) -T "$(COMPS)" reconfigure build
clean:
	-rm -r ./build
flash: unity
	$(build_cmd) --port $(port) flash
run:
	python3 ./run_tests.py $(port)

all: build flash run


##################### OpenOCD ###################
esp32_ocd_sh :=  sh $(realpath ./esp32_ocd.sh)

flash-ocd: unity
	(cd $(build_dir) && $(esp32_ocd_sh) flash)
flash-app-ocd: unity
	(cd $(build_dir) && $(esp32_ocd_sh) flash_app)
ocd:
	$(esp32_ocd_sh) server
ocd-loop:
	$(esp32_ocd_sh) server_loop

all-ocd: build flash-app-ocd run
