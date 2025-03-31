# attitude manager files
set(AM_SRC
    "src/attitude_manager/attitude_manager.cpp"
)
set(AM_INC
    "include/attitude_manager/"
)

# system manager files
set(SM_SRC
    "src/system_manager/system_manager.cpp"
)
set(SM_INC
    "include/system_manager/"
)

# combined files
set(ZP_SRC
    ${AM_SRC}
    ${SM_SRC}
)
set(ZP_INC
    "include/driver_ifaces/"
    "include/thread_msgs/"
    ${AM_INC}
    ${SM_INC}
)