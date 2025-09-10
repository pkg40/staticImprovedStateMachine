@echo off
echo State Machine Test Suite Runner
echo ===============================

echo.
echo Running Safety Tests...
pio test -e test_runner_embedded_unity --filter test_safety

echo.
echo Running Basic Functionality Tests...
pio test -e test_runner_embedded_unity --filter test_basic

echo.
echo Running Comprehensive Tests 1...
pio test -e test_runner_embedded_unity --filter test_comp_1

echo.
echo Running Comprehensive Tests 2...
pio test -e test_runner_embedded_unity --filter test_comp_2

echo.
echo Running Statistics & Scoreboard Tests...
pio test -e test_runner_embedded_unity --filter test_comp_3

echo.
echo Running Random Coverage Tests...
pio test -e test_runner_embedded_unity --filter test_comp_4

echo.
echo Running Final Validation Tests...
pio test -e test_runner_embedded_unity --filter test_comp_5

echo.
echo Running Conditional Compilation Tests...
pio test -e test_runner_embedded_unity --filter test_conditional_compilation

echo.
echo Running Naming Consistency Tests...
pio test -e test_runner_embedded_unity --filter test_naming_consistency

echo.
echo ===============================
echo All test suites completed!
echo ===============================
pause
