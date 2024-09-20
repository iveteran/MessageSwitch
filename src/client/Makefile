TARGET = switch_client

ROOT = ../..
ThirdParty = $(ROOT)/thirdparty

CPPFLAGS = -g -Wall -std=c++20 # -DUSE_SELECT
CXXFLAGS = -I../common \
		   -I$(ThirdParty)/EventLoop/include \
           -I$(ThirdParty)/json/include \
           -I$(ThirdParty)/argparse/include \
           -I$(ThirdParty)/toml11/include

LDFLAGS = 

DEP_LIBS += \
			../common/libswitch_common.a \
           $(ThirdParty)/EventLoop/core/libel.a \
           $(ThirdParty)/EventLoop/extensions/console/libconsole.a -lreadline

CXX      = g++
RM       = rm -f

SRCDIRS  = .
SRCEXTS  = .cpp
SOURCES  = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
OBJS     = $(foreach x,$(SRCEXTS), $(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))
DEPS     = $(patsubst %.o,%.d,$(OBJS))

.PHONY : all clean cleanall rebuild

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(DEP_LIBS)

%.d : %.cpp
	@$(CXX) -MM -MD $(CXXFLAGS) $<

%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $<

-include $(DEPS)

rebuild: clean all

clean:
	@$(RM) *.o *.d

cleanall: clean
	@$(RM) $(TARGET)
