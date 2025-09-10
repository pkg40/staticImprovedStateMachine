#!/usr/bin/env python3
"""
Script to run all test suites and generate comprehensive reports
"""

import subprocess
import sys
import os

def run_command(cmd, description):
    """Run a command and return success status"""
    print(f"\n{'='*60}")
    print(f"Running: {description}")
    print(f"Command: {cmd}")
    print(f"{'='*60}")
    
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        print(result.stdout)
        if result.stderr:
            print("STDERR:", result.stderr)
        return result.returncode == 0
    except Exception as e:
        print(f"Error running command: {e}")
        return False

def main():
    """Main function to orchestrate test execution"""
    
    # Change to the project directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.join(script_dir, '..')
    os.chdir(project_dir)
    
    print("State Machine Test Suite Runner")
    print("===============================")
    
    # Test environments to run
    test_environments = [
        ("test_master_runner", "Master Test Runner (All Suites)"),
        ("test_native_only", "Native Tests Only"),
    ]
    
    results = {}
    
    for env, description in test_environments:
        cmd = f"pio test -e {env}"
        success = run_command(cmd, description)
        results[env] = success
        
        if not success:
            print(f"❌ {description} FAILED")
        else:
            print(f"✅ {description} PASSED")
    
    # Print final summary
    print(f"\n{'='*60}")
    print("FINAL SUMMARY")
    print(f"{'='*60}")
    
    for env, description in test_environments:
        status = "✅ PASSED" if results[env] else "❌ FAILED"
        print(f"{description}: {status}")
    
    # Return overall success
    overall_success = all(results.values())
    if overall_success:
        print("\n🎉 All test suites completed successfully!")
        return 0
    else:
        print("\n💥 Some test suites failed!")
        return 1

if __name__ == "__main__":
    sys.exit(main())
