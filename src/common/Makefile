TARGET   = libswitch_common.a

ROOT = ../..
ThirdParty = $(ROOT)/thirdparty

CPPFLAGS = -g -Wall -std=c++20# -DUSE_SELECT
#CPPFLAGS = -Wall -std=c++20 -D_BINARY_MSG_EXTEND_PACKAGING
CXXFLAGS = -I$(ThirdParty)/EventLoop/include \
           -I$(ThirdParty)/json/include \

CXX      = g++
RM       = rm -f
AR       = ar -r

SRCDIRS  = . utils
SRCEXTS  = .cpp
SOURCES  = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
OBJS     = $(foreach x,$(SRCEXTS), $(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))
DEPS     = $(patsubst %.o,%.d,$(OBJS))

.PHONY : all clean cleanall rebuild

$(TARGET) : $(OBJS)
	$(RM) $(TARGET)
	$(AR) $(TARGET) $(OBJS)

%.d : %.cpp
	@$(CXX) -MM -MD $(CXXFLAGS) $<

$(OBJDIR)/%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $<

-include $(DEPS)

all : $(TARGET)

rebuild: clean all

clean:
	@$(RM) $(OBJS) *.d

cleanall: clean
	@$(RM) $(TARGET)
