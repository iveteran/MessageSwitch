TARGET = libswitch_client.a

ROOT = ../..
ThirdParty = $(ROOT)/thirdparty

CPPFLAGS = -g -Wall -std=c++20 # -DUSE_SELECT
CXXFLAGS = -I$(ROOT)/src/common \
		   -I$(ThirdParty)/EventLoop/include \
           -I$(ThirdParty)/argparse/include \

LDFLAGS = 

DEP_LIBS += 

CXX      = g++
RM       = rm -f
AR       = ar -r

SRCDIRS  = . # other sub directories
SRCEXTS  = .cpp
SOURCES  = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
OBJS     = $(foreach x,$(SRCEXTS), $(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))
DEPS     = $(patsubst %.o,%.d,$(OBJS))

.PHONY : all clean cleanall rebuild

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(RM) $(TARGET)
	$(AR) $(TARGET) $(OBJS)

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
