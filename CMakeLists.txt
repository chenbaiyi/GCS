cmake_minimum_required(VERSION 3.8)
project(GCMsg)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -fPIC")

include_directories(/home/chenbaiyi/Qt5.9.0/5.9/gcc_64/include/ /usr/include/ /usr/local/include)

link_directories(/home/chenbaiyi/Qt5.9.0/5.9/gcc_64/lib/ /usr/lib/ /usr/local/lib/)

set(SOURCE_FILES NetMoudle.cpp)

add_executable(NetMoudle ./redis/RedisSet.cpp ./redis/RedisHashMap.cpp NetMoudle.cpp CommStruDef.h
        ./tool/common_func.cpp ./tool/common_func.h GCCondBindMutex.cpp GCCondBindMutex.h
        ./redis/redisacc.cpp ./redis/redisacc.h redis/redisstd.h ./tool/CLog.cpp ./tool/CLog.h
        ./tool/Config.cpp ./tool/Config.h ./tool/CDBBase.h ./tool/CDBBase.cpp
        GCCMsgHandle.cpp GCCMsgHandle.h moc_GCCMsgHandle.cpp ./redis/RedisLocker.cpp)

target_link_libraries(NetMoudle pthread boost_system boost_serialization boost_thread event_core event_pthreads
        Qt5Core hiredis uuid log4cplus mysqlcppconn)