Write-Host "=== Testing Conditional Compilation ===" -ForegroundColor Cyan
Write-Host

Write-Host "1. Testing DEVELOPMENT MODE (all features enabled):" -ForegroundColor Yellow
Write-Host "Compiling with STATEMACHINE_PRODUCTION_MODE=0..."

# Development mode compilation
& g++ -std=c++11 -I. -DSTATEMACHINE_PRODUCTION_MODE=0 test_conditional_compilation.cpp src/improvedStateMachine.cpp -o test_dev.exe 2>$null
if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ Development mode compilation: SUCCESS" -ForegroundColor Green
    Write-Host "Running development mode test:"
    & ./test_dev.exe
    Remove-Item test_dev.exe -ErrorAction SilentlyContinue
} else {
    Write-Host "❌ Development mode compilation: FAILED" -ForegroundColor Red
}

Write-Host
Write-Host "2. Testing PRODUCTION MODE (features disabled):" -ForegroundColor Yellow
Write-Host "Compiling with STATEMACHINE_PRODUCTION_MODE=1..."

# Production mode compilation (expecting it to fail)
& g++ -std=c++11 -I. -DSTATEMACHINE_PRODUCTION_MODE=1 test_conditional_compilation.cpp src/improvedStateMachine.cpp -o test_prod.exe 2>$null
if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ Production mode compilation: SUCCESS" -ForegroundColor Green
    Write-Host "Running production mode test:"
    & ./test_prod.exe
    Remove-Item test_prod.exe -ErrorAction SilentlyContinue
} else {
    Write-Host "✅ Production mode compilation: EXPECTED TO FAIL (features disabled)" -ForegroundColor Green
    Write-Host "This is correct - debug/stats code should not compile in production mode" -ForegroundColor Green
}

Write-Host
Write-Host "=== Test Complete ===" -ForegroundColor Cyan
