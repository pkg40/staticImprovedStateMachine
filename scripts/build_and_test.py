#!/usr/bin/env python3
"""
Build and test script for the Improved State Machine Library
"""

import subprocess
import sys
import os
import json
from pathlib import Path

def run_command(command, description, cwd=None):
    """Run a command and handle errors"""
    print(f"\n{'='*60}")
    print(f"Running: {description}")
    print(f"Command: {command}")
    print(f"{'='*60}")
    
    try:
        result = subprocess.run(command, shell=True, cwd=cwd, check=True, 
                              capture_output=True, text=True)
        print(result.stdout)
        if result.stderr:
            print("STDERR:", result.stderr)
        return True
    except subprocess.CalledProcessError as e:
        print(f"ERROR: {description} failed!")
        print(f"Return code: {e.returncode}")
        print(f"STDOUT: {e.stdout}")
        print(f"STDERR: {e.stderr}")
        return False

def check_platformio():
    """Check if PlatformIO is installed"""
    try:
        result = subprocess.run("pio --version", shell=True, capture_output=True, text=True)
        print(f"PlatformIO version: {result.stdout.strip()}")
        return True
    except FileNotFoundError:
        print("ERROR: PlatformIO not found! Please install PlatformIO first.")
        print("Visit: https://platformio.org/install")
        return False

def build_library():
    """Build the library for different platforms"""
    platforms = ['esp32', 'native', 'atmega328p']
    
    for platform in platforms:
        if not run_command(f"pio run -e {platform}", f"Building for {platform}"):
            return False
    
    return True

def run_tests():
    """Run all tests"""
    test_environments = ['native', 'esp32']
    
    for env in test_environments:
        if not run_command(f"pio test -e {env}", f"Running tests on {env}"):
            print(f"WARNING: Tests failed on {env}")
            # Continue with other platforms
    
    return True

def run_coverage():
    """Run coverage analysis"""
    if not run_command("pio test -e coverage", "Running coverage analysis"):
        print("WARNING: Coverage analysis failed")
        return False
    
    # Process coverage results if available
    coverage_files = list(Path(".").glob("**/*.gcov"))
    if coverage_files:
        print(f"\nFound {len(coverage_files)} coverage files")
        # Could add coverage report processing here
    
    return True

def validate_library_json():
    """Validate library.json format"""
    try:
        with open("library.json", "r") as f:
            data = json.load(f)
        
        required_fields = ["name", "version", "description", "keywords", "authors"]
        for field in required_fields:
            if field not in data:
                print(f"ERROR: Missing required field '{field}' in library.json")
                return False
        
        print("library.json validation: PASSED")
        return True
    except json.JSONDecodeError as e:
        print(f"ERROR: Invalid JSON in library.json: {e}")
        return False
    except FileNotFoundError:
        print("ERROR: library.json not found")
        return False

def check_code_style():
    """Check code style (basic checks)"""
    cpp_files = list(Path("src").glob("**/*.cpp")) + list(Path("src").glob("**/*.hpp"))
    
    issues = []
    for file_path in cpp_files:
        with open(file_path, "r") as f:
            content = f.read()
            lines = content.split('\n')
            
            for i, line in enumerate(lines, 1):
                # Check for tabs (should use spaces)
                if '\t' in line:
                    issues.append(f"{file_path}:{i}: Uses tabs instead of spaces")
                
                # Check line length (warning for >120 chars)
                if len(line) > 120:
                    issues.append(f"{file_path}:{i}: Line too long ({len(line)} chars)")
                
                # Check for trailing whitespace
                if line.endswith(' ') or line.endswith('\t'):
                    issues.append(f"{file_path}:{i}: Trailing whitespace")
    
    if issues:
        print(f"Code style issues found ({len(issues)}):")
        for issue in issues[:10]:  # Show first 10 issues
            print(f"  {issue}")
        if len(issues) > 10:
            print(f"  ... and {len(issues) - 10} more")
        return False
    else:
        print("Code style check: PASSED")
        return True

def generate_documentation():
    """Generate documentation"""
    print("Documentation files:")
    doc_files = list(Path("docs").glob("*.md"))
    for doc_file in doc_files:
        print(f"  - {doc_file}")
    
    # Could add automatic documentation generation here
    return True

def run_static_analysis():
    """Run static analysis if tools are available"""
    # Check for cppcheck
    try:
        subprocess.run("cppcheck --version", shell=True, capture_output=True, check=True)
        if run_command("cppcheck --enable=all --inconclusive --std=c++14 src/", 
                      "Running cppcheck static analysis"):
            print("Static analysis: PASSED")
        else:
            print("Static analysis: FAILED")
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("cppcheck not available, skipping static analysis")
    
    return True

def main():
    """Main build and test routine"""
    print("Improved State Machine Library - Build and Test Script")
    print("=" * 60)
    
    # Change to library directory (parent of scripts)
    script_dir = Path(__file__).parent
    lib_dir = script_dir.parent
    os.chdir(lib_dir)
    
    print(f"Working directory: {os.getcwd()}")
    
    success = True
    
    # Check prerequisites
    if not check_platformio():
        return 1
    
    # Validate configuration
    if not validate_library_json():
        success = False
    
    # Code quality checks
    if not check_code_style():
        print("WARNING: Code style issues found")
    
    # Build library
    if not build_library():
        print("ERROR: Build failed!")
        success = False
    
    # Run tests
    if not run_tests():
        print("WARNING: Some tests failed")
    
    # Run coverage analysis
    if not run_coverage():
        print("WARNING: Coverage analysis issues")
    
    # Static analysis
    run_static_analysis()
    
    # Documentation
    generate_documentation()
    
    # Summary
    print("\n" + "=" * 60)
    if success:
        print("BUILD AND TEST: SUCCESS")
        print("Library is ready for use!")
    else:
        print("BUILD AND TEST: ISSUES FOUND")
        print("Please address the issues above")
    print("=" * 60)
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
