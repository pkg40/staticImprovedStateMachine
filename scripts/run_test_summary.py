#!/usr/bin/env python3
"""
Simple script to run individual test suites and generate a comprehensive summary
"""

import subprocess
import sys
import os
import re

def run_test_suite(env, test_name, description):
    """Run a single test suite and return results"""
    print(f"\n{'='*60}")
    print(f"Running: {description}")
    print(f"Command: pio test -e {env} --filter {test_name}")
    print(f"{'='*60}")
    
    try:
        result = subprocess.run(
            f"pio test -e {env} --filter {test_name}", 
            shell=True, 
            capture_output=True, 
            text=True,
            timeout=300  # 5 minute timeout
        )
        
        # Parse the output for test results
        output = result.stdout
        stderr = result.stderr
        
        # Look for test summary patterns
        test_count = 0
        passed_count = 0
        failed_count = 0
        
        # Parse Unity output format
        if "test cases:" in output:
            match = re.search(r'(\d+) test cases: (\d+) succeeded', output)
            if match:
                test_count = int(match.group(1))
                passed_count = int(match.group(2))
                failed_count = test_count - passed_count
        
        success = result.returncode == 0
        
        print(f"Exit code: {result.returncode}")
        if stderr:
            print(f"STDERR: {stderr}")
        
        return {
            'name': test_name,
            'description': description,
            'success': success,
            'test_count': test_count,
            'passed': passed_count,
            'failed': failed_count,
            'output': output
        }
        
    except subprocess.TimeoutExpired:
        print("Test timed out after 5 minutes")
        return {
            'name': test_name,
            'description': description,
            'success': False,
            'test_count': 0,
            'passed': 0,
            'failed': 0,
            'output': "TIMEOUT"
        }
    except Exception as e:
        print(f"Error running test: {e}")
        return {
            'name': test_name,
            'description': description,
            'success': False,
            'test_count': 0,
            'passed': 0,
            'failed': 0,
            'output': f"ERROR: {e}"
        }

def main():
    """Main function to run all test suites and generate summary"""
    
    # Change to the project directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.join(script_dir, '..')
    os.chdir(project_dir)
    
    print("State Machine Test Suite Summary Generator")
    print("==========================================")
    
    # Define test suites to run
    test_suites = [
        ("test_runner_embedded_unity", "test_safety", "Safety Tests"),
        ("test_runner_embedded_unity", "test_basic", "Basic Functionality Tests"),
        ("test_runner_embedded_unity", "test_comp_1", "Comprehensive Tests 1"),
        ("test_runner_embedded_unity", "test_comp_2", "Comprehensive Tests 2"),
        ("test_runner_embedded_unity", "test_comp_3", "Statistics & Scoreboard Tests"),
        ("test_runner_embedded_unity", "test_comp_4", "Random Coverage Tests"),
        ("test_runner_embedded_unity", "test_comp_5", "Final Validation Tests"),
        ("test_runner_embedded_unity", "test_conditional_compilation", "Conditional Compilation Tests"),
        ("test_runner_embedded_unity", "test_naming_consistency", "Naming Consistency Tests"),
    ]
    
    results = []
    total_tests = 0
    total_passed = 0
    total_failed = 0
    successful_suites = 0
    
    # Run each test suite
    for env, test_name, description in test_suites:
        result = run_test_suite(env, test_name, description)
        results.append(result)
        
        total_tests += result['test_count']
        total_passed += result['passed']
        total_failed += result['failed']
        if result['success']:
            successful_suites += 1
    
    # Print comprehensive summary
    print(f"\n{'='*80}")
    print("COMPREHENSIVE TEST SUMMARY")
    print(f"{'='*80}")
    print(f"Total Test Suites: {len(test_suites)}")
    print(f"Successful Suites: {successful_suites}")
    print(f"Failed Suites: {len(test_suites) - successful_suites}")
    print(f"Total Tests: {total_tests}")
    print(f"Total Passed: {total_passed}")
    print(f"Total Failed: {total_failed}")
    if total_tests > 0:
        success_rate = (total_passed / total_tests) * 100
        print(f"Overall Success Rate: {success_rate:.1f}%")
    
    print(f"\n{'='*80}")
    print("SUITE BREAKDOWN")
    print(f"{'='*80}")
    print(f"{'Suite Name':<30} {'Status':<10} {'Tests':<8} {'Passed':<8} {'Failed':<8} {'Rate':<8}")
    print("-" * 80)
    
    for result in results:
        status = "✅ PASS" if result['success'] else "❌ FAIL"
        if result['test_count'] > 0:
            rate = f"{(result['passed'] / result['test_count']) * 100:.1f}%"
        else:
            rate = "N/A"
        
        print(f"{result['description']:<30} {status:<10} {result['test_count']:<8} {result['passed']:<8} {result['failed']:<8} {rate:<8}")
    
    print(f"{'='*80}")
    
    # Return overall success
    overall_success = successful_suites == len(test_suites)
    if overall_success:
        print("\n🎉 All test suites completed successfully!")
        return 0
    else:
        print(f"\n💥 {len(test_suites) - successful_suites} test suite(s) failed!")
        return 1

if __name__ == "__main__":
    sys.exit(main())
