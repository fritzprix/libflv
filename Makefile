DBG_CXXFLAGS :=	-O0 -g3 -Wall -fmessage-length=0 -D__DBG
REL_CXXFLAGS :=  -O2 -g0 -Wall -fmessage-length=0
DYN_FLAGS := -shared -fPIC
AR := ar

DBG_OBJS :=		libflv.do
REL_OBJS := 		libflv.o

UTEST := 	test

LIBS = -L$(CURDIR)

DBG_DYNAMIC_TARGET :=	libflvd.so
DBG_STATIC_TARGET := 	libflvd.a
DBG_TEST_TARGET := 		flvtestd

REL_DYNAMIC_TARGET :=	libflv.so
REL_STATIC_TARGET := 	libflv.a
REL_TEST_TARGET := 		flvtest 
TEST=

.PHONY = clean all 

all : debug

debug : $(DBG_TEST_TARGET) $(DBG_DYNAMIC_TARGET) $(DBG_STATIC_TARGET)

release : $(REL_TEST_TARGET) $(REL_DYNAMIC_TARGET) $(REL_STATIC_TARGET)

$(DBG_TEST_TARGET) : $(UTEST:%=%.do) $(DBG_OBJS)
	$(CXX) -o $@ $(DBG_CXXFLAGS) $(LIBS) $(DBG_OBJS) $<

$(DBG_STATIC_TARGET) : $(DBG_OBJS)
	$(AR) rcs -o $@ $<
	
$(DBG_DYNAMIC_TARGET) : $(DBG_OBJS)
	$(CXX) -o $@ $(DYN_FLAGS) $(DBG_CXXFLAGS) $(LIBS) $(DBG_OBJS)

$(REL_TEST_TARGET) : $(UTEST:%=%.o) $(REL_OBJS)
	$(CXX) -o $@ $(REL_CXXFLAGS) $(LIBS) $(REL_OBJS) $<
	
	
$(REL_STATIC_TARGET) : $(REL_OBJS)
	$(AR) rcs -o $@ $<
	
$(REL_DYNAMIC_TARGET) : $(REL_OBJS)
	$(CXX) -o $@ $(DYN_FLAGS) $(REL_CXXFLAGS) $(LIBS) $(REL_OBJS)
	
%.do:%.cpp
	$(CXX) -c -o $@ $(DBG_CXXFLAGS) $(LIBS) $(DYN_FLAGS) $< 
	
%.o:%.cpp
	$(CXX) -c -o $@ $(REL_CXXFLAGS) $(LIBS) $(DYN_FLAGS) $<
	
clean:
	rm -rf $(DBG_OBJS) *.do *.o $(DBG_TEST_TARGET) $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET) $(REL_OBJS) $(REL_TEST_TARGET) $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET)
	
