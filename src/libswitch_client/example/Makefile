TARGET = libswitch_client_demo

ROOT = ../../..
ThirdParty = $(ROOT)/thirdparty

CPPFLAGS = -g -Wall -std=c++20 # -DUSE_SELECT
CXXFLAGS = -I$(ROOT)/src/common \
		   -I$(ThirdParty)/EventLoop/include \
           -I$(ThirdParty)/json/include \
           -I$(ThirdParty)/argparse/include \
           -I$(ThirdParty)/toml11/include \
           -I..

LDFLAGS = 

DEP_LIBS += \
		../libswitch_client.a \
		$(ROOT)/src/common/libswitch_common.a \
		$(ThirdParty)/EventLoop/core/libel.a \
		$(ThirdParty)/EventLoop/extensions/console/libel_console.a -lreadline \
		$(ThirdParty)/EventLoop/extensions/aio_api/libel_aio.a -laio \

CXX      = g++
RM       = rm -f

SRCDIRS  = . # other sub directories
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

$(OBJDIR)/%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $<

-include $(DEPS)

rebuild: clean all

clean:
	@$(RM) $(OBJS) *.d

cleanall: clean
	@$(RM) $(TARGET)
