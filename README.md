## Welcome to ZeroPilot3.5
ZeroPilot is WARG's custom flight controller! Before beginning work, please take a look at the desgin documentation and KT videos here: https://uwarg-docs.atlassian.net/wiki/spaces/ZP/pages/2668101634/ZeroPilot3.5+Design

## Building the Project
TODO:
- hardware build
- testing build

## Unit Testing with Google Test Framework

### Prerequisites
Before running unit tests, ensure you have the required dependencies installed:

```bash
# Install build tools and Google Test
sudo apt update
sudo apt install build-essential libgtest-dev libgmock-dev cmake ninja-build

# For ARM cross-compilation (hardware builds)
sudo apt install gcc-arm-none-eabi
```

### Test Structure
The project uses Google Test/Mock framework for unit testing with the following structure:

```
zeropilot3.5/tests/
├── CMakeLists.txt          # CMake configuration for all tests
├── testbuild.bash          # Automated build script
├── driver_mocks/           # Mock implementations of hardware interfaces
│   ├── iwdg_mock.hpp       # Watchdog mock
│   ├── logger_mock.hpp     # Logger mock
│   ├── rc_mock.hpp         # RC receiver mock
│   ├── queue_mock.hpp      # Message queue mock
│   └── *_fake.hpp          # Fake implementations for testing
├── system_manager/         # System Manager unit tests
│   ├── 21001_Default.cpp   # Basic functionality test
│   ├── 21002_Logger.cpp    # Logger message batching test
│   ├── 21003_AmQueue.cpp   # Attitude Manager queue test
│   ├── 21004_Watchdog.cpp  # Watchdog refresh test
│   └── 21005_RCData.cpp    # RC data handling test
└── attitude_manager/       # Attitude Manager unit tests (future)
```

### Running Tests

#### Method 1: Using CMake (Recommended)
```bash
# Navigate to tests directory
cd zeropilot3.5/tests

# Make the build script executable (first time only)
chmod +x testbuild.bash

# Build tests
./testbuild.bash

# Run tests after building
cd build
./gtestzeropilot3.5
```

#### Method 2: Manual Compilation (Individual Tests)
For quick testing of individual test files:

```bash
# From project root directory

# Test Logger message batching
g++ -I/usr/include -I./zeropilot3.5/include -I./zeropilot3.5/include/driver_ifaces \
    -I./zeropilot3.5/include/system_manager -I./zeropilot3.5/include/thread_msgs \
    -I./zeropilot3.5/tests/driver_mocks -std=gnu++17 \
    ./zeropilot3.5/tests/system_manager/21002_Logger.cpp \
    ./zeropilot3.5/src/system_manager/system_manager.cpp \
    -lgtest -lgmock -lgtest_main -pthread -o 21002_Logger && \
    ./21002_Logger && rm 21002_Logger

# Test Attitude Manager queue functionality
g++ -I/usr/include -I./zeropilot3.5/include -I./zeropilot3.5/include/driver_ifaces \
    -I./zeropilot3.5/include/system_manager -I./zeropilot3.5/include/thread_msgs \
    -I./zeropilot3.5/tests/driver_mocks -std=gnu++17 \
    ./zeropilot3.5/tests/system_manager/21003_AmQueue.cpp \
    ./zeropilot3.5/src/system_manager/system_manager.cpp \
    -lgtest -lgmock -lgtest_main -pthread -o 21003_AmQueue && \
    ./21003_AmQueue && rm 21003_AmQueue

# Test Watchdog functionality
g++ -I/usr/include -I./zeropilot3.5/include -I./zeropilot3.5/include/driver_ifaces \
    -I./zeropilot3.5/include/system_manager -I./zeropilot3.5/include/thread_msgs \
    -I./zeropilot3.5/tests/driver_mocks -std=gnu++17 \
    ./zeropilot3.5/tests/system_manager/21004_Watchdog.cpp \
    ./zeropilot3.5/src/system_manager/system_manager.cpp \
    -lgtest -lgmock -lgtest_main -pthread -o 21004_Watchdog && \
    ./21004_Watchdog && rm 21004_Watchdog

# Test RC Data handling
g++ -I/usr/include -I./zeropilot3.5/include -I./zeropilot3.5/include/driver_ifaces \
    -I./zeropilot3.5/include/system_manager -I./zeropilot3.5/include/thread_msgs \
    -I./zeropilot3.5/tests/driver_mocks -std=gnu++17 \
    ./zeropilot3.5/tests/system_manager/21005_RCData.cpp \
    ./zeropilot3.5/src/system_manager/system_manager.cpp \
    -lgtest -lgmock -lgtest_main -pthread -o 21005_RC && \
    ./21005_RC && rm 21005_RC
```

#### Method 3: Running Specific Tests
```bash
# Run only specific test cases
cd zeropilot3.5/tests/build
./gtestzeropilot3.5 --gtest_filter="SMTest.21004_Watchdog"
./gtestzeropilot3.5 --gtest_filter="SMTest.21005_*"      # All RC data tests
./gtestzeropilot3.5 --gtest_filter="SMTest.*"            # All System Manager tests
```

### Available Test Cases

#### System Manager Tests
- **21001_Default**: Basic SystemManager instantiation and default behavior
- **21002_Logger**: Tests logger message batching and queue processing (handles up to 16 messages per batch)
- **21003_AmQueue**: Verifies RC data is properly pushed to the Attitude Manager queue
- **21004_Watchdog**: Verifies watchdog refresh functionality through SystemManager
- **21005_RCData**: Comprehensive RC data handling tests including:
  - Basic RC data passthrough verification
  - Mock vs. custom data return testing  
  - End-to-end data flow from RC receiver to attitude manager queue
  - Connection status monitoring and timeout handling

### Mock Framework Details

#### Mock Objects
The test framework provides mock implementations for all hardware interfaces:

- **MockWatchdog**: Simulates independent watchdog functionality
- **MockLogger**: Captures and verifies log messages
- **MockRCReceiver**: Simulates RC receiver with configurable data
- **MockMessageQueue**: Simulates inter-thread message queues

#### Mock Usage Patterns
```cpp
// Using NiceMock for non-critical calls
NiceMock<MockLogger> mockLogger;

// Using strict Mock for verification
MockRCReceiver mockRC;
EXPECT_CALL(mockRC, getRCData())
    .Times(1)
    .WillOnce(::testing::Return(testData));

// Using fake implementations
mockRC.delegateToFake();  // Uses FakeRCReceiver for realistic behavior
```

### Test Output Examples
```
[==========] Running 4 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 4 tests from SMTest
[ RUN      ] SMTest.21005_RCData
[       OK ] SMTest.21005_RCData (0 ms)
[ RUN      ] SMTest.21005_GetRCDataReturnsData
[       OK ] SMTest.21005_GetRCDataReturnsData (0 ms)
[ RUN      ] SMTest.21005_GetRCDataReturnsCustomData
[       OK ] SMTest.21005_GetRCDataReturnsCustomData (0 ms)
[ RUN      ] SMTest.21005_GetRCDataFlowsToQueue
[       OK ] SMTest.21005_GetRCDataFlowsToQueue (0 ms)
[----------] 4 tests from SMTest (0 ms total)
[==========] 4 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 4 tests.
```

### Adding New Tests

1. **Create test file** in appropriate directory (e.g., `tests/system_manager/21006_NewFeature.cpp`)
2. **Add to CMakeLists.txt** in the relevant `*_TSRC` variable
3. **Include required headers** and mock objects
4. **Write test cases** using Google Test macros:
   ```cpp
   TEST(TestSuite, TestName) {
       // Arrange
       MockObject mock;
       EXPECT_CALL(mock, method()).Times(1);
       
       // Act
       ObjectUnderTest obj(&mock);
       obj.doSomething(); //should be sm.smUpdate();
       
       // Assert (handled by EXPECT_CALL)
   }
   ```

### Troubleshooting

**Common Issues:**
- **Permission denied for testbuild.bash**: Run `chmod +x zeropilot3.5/tests/testbuild.bash` to make the script executable
- **Missing headers**: Ensure all include paths are correct in compilation command
- **Linking errors**: Verify `-lgtest -lgmock -lgtest_main -pthread` flags are present
- **Mock call failures**: Check that expectations are set before the tested code runs
- **Compilation failures**: Ensure Google Test development packages are installed

**Debugging Tests:**
Use the VS Code debugger configuration provided below for step-through debugging.

## Debugging GoogleTest in VS Code
To debug the testing build in VS Code, add the following `launch.json` file to the `.vscode` folder:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "GTest Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/zeropilot3.5/tests/build/gtestzeropilot3.5",
            "args": [
                "--gtest_filter=*"
            ],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

**Debug Configuration Notes:**
- Update `miDebuggerPath` if your gdb is in a different location
- Use `--gtest_filter` to run specific tests during debugging:
  - `--gtest_filter="SMTest.21004_Watchdog"` - Debug single test
  - `--gtest_filter="SMTest.*"` - Debug all System Manager tests
  - `--gtest_filter="*RCData*"` - Debug all RC-related tests
- Set breakpoints in test files or source code before debugging
- Use VS Code's integrated terminal to run tests first to identify which ones to debug

**Advanced Debugging:**
```json
{
    "name": "GTest Debug Specific",
    "type": "cppdbg",
    "request": "launch",
    "program": "${workspaceFolder}/zeropilot3.5/tests/build/gtestzeropilot3.5",
    "args": [
        "--gtest_filter=SMTest.21005_GetRCDataFlowsToQueue",
        "--gtest_break_on_failure"
    ],
    "stopAtEntry": true,
    "cwd": "${workspaceFolder}",
    "environment": [],
    "externalConsole": false,
    "MIMode": "gdb",
    "miDebuggerPath": "/usr/bin/gdb"
}
```

## Quick Reference Commands

```bash
# Build all tests
cd zeropilot3.5/tests && ./testbuild.bash

# Run all tests  
cd zeropilot3.5/tests/build && ./gtestzeropilot3.5

# Run specific test
./gtestzeropilot3.5 --gtest_filter="SMTest.21005_RCData"

# Run with verbose output
./gtestzeropilot3.5 --gtest_filter="SMTest.*" --gtest_print_time=1

# Quick individual test compilation (from project root)
g++ -I/usr/include -I./zeropilot3.5/include -I./zeropilot3.5/include/driver_ifaces \
    -I./zeropilot3.5/include/system_manager -I./zeropilot3.5/include/thread_msgs \
    -I./zeropilot3.5/tests/driver_mocks -std=gnu++17 \
    ./zeropilot3.5/tests/system_manager/[TEST_FILE].cpp \
    ./zeropilot3.5/src/system_manager/system_manager.cpp \
    -lgtest -lgmock -lgtest_main -pthread -o test_output && ./test_output && rm test_output
```


# Written by Ryan Zhou

