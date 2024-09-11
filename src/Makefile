TARGET_SERVER = switch_server
TARGET_CLIENT = switch_client

ROOT = ..
ThirdParty = $(ROOT)/thirdparty

CPPFLAGS = -g -Wall -std=c++20# -DUSE_SELECT
CXXFLAGS = -I$(ThirdParty)/EventLoop/include \
		   -I$(ThirdParty)/json/include \
		   -I$(ThirdParty)/argparse/include

LDFLAGS = 

DEP_LIBS += \
           $(ThirdParty)/EventLoop/src/libel.a

CXX      = g++
RM       = rm -f

TARGET_SERVER_OBJS = switch.o switch_server.o switch_command_handler.o switch_message.o switch_types.o switch_context.o
TARGET_CLIENT_OBJS = switch_client_main.o switch_client.o switch_message.o switch_types.o

%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $<

.PHONY : all clean cleanall rebuild

all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER) : $(TARGET_SERVER_OBJS) $(DEP_LIBS)
	$(CXX) -o $(TARGET_SERVER) $(TARGET_SERVER_OBJS) $(DEP_LIBS) $(LDFLAGS)

$(TARGET_CLIENT) : $(TARGET_CLIENT_OBJS) $(DEP_LIBS)
	$(CXX) -o $(TARGET_CLIENT) $(TARGET_CLIENT_OBJS) $(DEP_LIBS) $(LDFLAGS)

rebuild: clean all

clean:
	@$(RM) *.o *.d

cleanall: clean
	@$(RM) $(TARGET_SERVER) $(TARGET_CLIENT)
