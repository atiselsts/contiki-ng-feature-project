# Change this to your Tensorflow Lite Micro directory:
TFLM?=/home/atis/source/tflite-micro

CONTIKI_PROJECT = main
all: $(CONTIKI_PROJECT)

OPTIMIZATIONS=-Os

CUSTOM_RULE_LINK=1

PROJECT_SOURCEFILES += nn.cpp

CFLAGS += -I$(TFLM)

CXXFLAGS += -DTF_LITE_STATIC_MEMORY -DTF_LITE_MCU_DEBUG_LOG
CXXFLAGS += -I$(TFLM)/tensorflow/lite/micro/tools/make/downloads/flatbuffers/include

LDFLAGS += -L.
TARGET_LIBFILES += -ltensorflow-microlite -lm

CONTIKI = ../..
include $(CONTIKI)/Makefile.include

# Some platforms require that CXX is used as the linker as there is some C++ code
$(BUILD_DIR_BOARD)/%.$(TARGET): %.o $(PROJECT_OBJECTFILES) $(PROJECT_LIBRARIES) $(CONTIKI_NG_TARGET_LIB)
	$(TRACE_LD)
	$(Q)$(CXX) $(LDFLAGS) $(TARGET_STARTFILES) ${filter-out %.a,$^}  ${filter %.a,$^} $(TARGET_LIBFILES) -o $@
