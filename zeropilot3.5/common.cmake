# attitude manager files
set(AM_SRC

)
set(AM_INC

)

# system manager files
set(SM_SRC

)
set(SM_INC

)

# telemetry manager files
set(TM_SRC

)
set(TM_INC
    "include/telemetry_manager"
)
set(COMMON_SRC

)
set(COMMON_INC
    "include/common"
)
# combined files
set(ZP_SRC
    ${AM_SRC}
    ${SM_SRC}
    ${TM_SRC}
    ${COMMON_SRC}
)
set(ZP_INC
    "include/driver_ifaces"
    ${AM_INC}
    ${SM_INC}
    ${TM_INC}
    ${COMMON_INC}
)