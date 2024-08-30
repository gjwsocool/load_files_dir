pwd=${shell pwd}
src_dir=$(pwd)/src/
include_dir=$(pwd)/include/
test_dir=$(pwd)/test/

TARGET=TcpServer
########################## src section #################################
obj_src=${shell find $(src_dir) -name "*.cpp"}
OBJS=${patsubst %.cpp,%.o,$(obj_src)}
#添加测试文件的内容
test_src=${shell find ${test_dir} -name "*.cpp"}
TEST_OBJ=${patsubst %.cpp,%.o,${test_src}}
tests=${patsubst %.cpp,%,${test_src}}
####################### flag section ########################################
GXX=g++
CFLAGS=-I${include_dir}
#############################################################################
$(TARGET): $(OBJS)
	$(GXX) $(CFLAGS)  -o $@ $(OBJS)

%.o:%.cpp
	$(GXX) ${CFLAGS} -c -o $@ $<
#添加编译测试文件的规则使用
.PHONY: test
test: $(tests)
$(tests): $(TEST_OBJ)
	$(GXX) $(CFLAGS)  -o $@ $(TEST_OBJ)

$(TEST_OBJ): $(test_src)
	$(GXX) ${CFLAGS} -c $< -o $@


clean:
	rm $(TARGET)
	rm $(OBJS)
	rm ${TEST_OBJ}
	rm ${tests}