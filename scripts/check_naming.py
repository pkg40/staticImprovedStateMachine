#!/usr/bin/env python3
"""
PlatformIO pre-build script for naming consistency checks
"""

import sys
import os
import re
from pathlib import Path

def check_naming_consistency():
    """Check naming consistency across the codebase"""
    print("🔍 Checking naming consistency...")
    
    src_dir = Path("src")
    if not src_dir.exists():
        print("❌ Source directory 'src' not found")
        return False
    
    cpp_files = list(src_dir.glob("**/*.cpp")) + list(src_dir.glob("**/*.hpp"))
    
    if not cpp_files:
        print("⚠️  No C++ source files found")
        return True
    
    issues = []
    
    # Naming patterns
    class_pattern = re.compile(r'\bclass\s+([A-Z][a-zA-Z0-9]*)\b')  # PascalCase classes
    enum_class_pattern = re.compile(r'\benum\s+class\s+([A-Z][a-zA-Z0-9]*)\b')  # PascalCase enum classes
    function_pattern = re.compile(r'\b([a-z][a-zA-Z0-9]*)\s*\(')    # camelCase functions
    variable_pattern = re.compile(r'\b([a-z][a-zA-Z0-9]*)\s*[;=]')  # camelCase variables
    constant_pattern = re.compile(r'\b([A-Z][A-Z0-9_]*)\b')         # SCREAMING_SNAKE constants
    
    for file_path in cpp_files:
        try:
            with open(file_path, "r", encoding="utf-8") as f:
                content = f.read()
                lines = content.split('\n')
                
                for i, line in enumerate(lines, 1):
                    # Skip comments and strings
                    stripped_line = line.strip()
                    if stripped_line.startswith('//') or stripped_line.startswith('/*') or '*/' in stripped_line:
                        continue
                    if '"' in line or "'" in line:
                        continue
                    
                    # Check for PascalCase class names (should be camelCase)
                    class_matches = class_pattern.findall(line)
                    for class_name in class_matches:
                        # Skip known exceptions and standard library classes
                        if class_name in ['MockSerial', 'Path', 'Json', 'Unity', 'Test', 'Setup', 'Loop',
                                        'Serial', 'String', 'Vector', 'Array', 'List', 'Map', 'Set']:
                            continue
                        # Skip if it's in a comment or string
                        if f'//{class_name}' in line or f'/*{class_name}' in line or f'"{class_name}"' in line:
                            continue
                        # Skip if this is actually an enum class (enum classes should be PascalCase)
                        if 'enum class' in line:
                            continue
                        if class_name[0].isupper() and len(class_name) > 1 and not class_name.isupper():
                            issues.append(f"❌ {file_path}:{i}: Class '{class_name}' should use camelCase (found PascalCase)")
                    
                    # Check for PascalCase enum classes (should be PascalCase - this is correct)
                    enum_class_matches = enum_class_pattern.findall(line)
                    for enum_name in enum_class_matches:
                        # Enum classes should be PascalCase, so this is actually correct
                        # We don't flag this as an issue
                        pass
                    
                    # Check for SCREAMING_SNAKE in variable names (should be camelCase)
                    if '=' in line or ';' in line:
                        var_matches = variable_pattern.findall(line)
                        for var_name in var_matches:
                            if var_name.isupper() and '_' in var_name and len(var_name) > 1:
                                # Skip known constants and macros
                                if var_name in ['NULL', 'TRUE', 'FALSE', 'MAX', 'MIN', 'SIZE', 'COUNT', 'PI',
                                              'ARDUINO', 'ESP32', 'NATIVE', 'DEBUG', 'RELEASE']:
                                    continue
                                # Skip if it's a macro definition
                                if '#define' in line:
                                    continue
                                issues.append(f"❌ {file_path}:{i}: Variable '{var_name}' should use camelCase (found SCREAMING_SNAKE)")
        
        except UnicodeDecodeError:
            print(f"⚠️  Could not read {file_path} (encoding issue)")
            continue
        except Exception as e:
            print(f"⚠️  Error reading {file_path}: {e}")
            continue
    
    if issues:
        print(f"❌ Naming consistency issues found ({len(issues)}):")
        for issue in issues[:15]:  # Show first 15 issues
            print(f"   {issue}")
        if len(issues) > 15:
            print(f"   ... and {len(issues) - 15} more")
        print("\n💡 To fix these issues:")
        print("   - Use camelCase for class names (e.g., 'myClass' instead of 'MyClass')")
        print("   - Use camelCase for variable names (e.g., 'myVariable' instead of 'MY_VARIABLE')")
        print("   - Use SCREAMING_SNAKE only for constants and macros")
        return False
    else:
        print("✅ Naming consistency check: PASSED")
        return True

if __name__ == "__main__":
    success = check_naming_consistency()
    sys.exit(0 if success else 1)
