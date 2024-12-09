#!/bin/bash

# 来自 https://github.com/MingLLuo/CS162HW-TestScript 测试脚本

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

pass_count=0
fail_count=0

run_test() {
  test_case=$1
  expected_output=$2
  actual_output=$(echo -e "$test_case\nexit" | ./shell 2>&1)
  actual_exit_code=$?

  if [ "$actual_output" = "$expected_output" ] && [ "$actual_exit_code" -eq 0 ]; then
    echo -e "${GREEN}PASS${NC}: Test case '$test_case' succeeded."
    ((pass_count++))
  else
    echo -e "${RED}FAIL${NC}: Test case '$test_case' failed."
    echo -e "Expected output: \n$expected_output"
    echo -e "Actual output: \n$actual_output"
    ((fail_count++))
  fi
}

# Test changing directories
test_change_directory() {
  echo "Testing changing directories..."

  # Test 'cd' command with valid directory
  run_test "cd /tmp" ""

  # Test 'cd' command with invalid directory
  run_test "cd /nonexistent" "cd: No such file or directory"

  # Test 'pwd' command
  echo "Testing pwd command..."
  run_test "pwd" "$(pwd)"

  # Test comprehensive directory support
  echo "Testing comprehensive directory support..."

  # Create a new directory
  mkdir -p /tmp/test_dir

  # Change directory to the new directory
  run_test "cd /tmp/test_dir" ""

  # List files in the directory
  expected_ls_output=$(ls)
  run_test "ls" "$expected_ls_output"
}

# Test starting programs using full path
test_start_programs_full_path() {
  echo "Testing starting programs using full path..."
  expected_ls_output=$(ls)
  run_test "/bin/ls" "$expected_ls_output"
}

# Test starting programs with arguments
test_start_programs_with_arguments() {
  echo "Testing starting programs with arguments..."
  run_test "/bin/echo Hello" "Hello"
}

# Test starting programs in PATH
test_start_programs_in_path() {
  echo "Testing starting programs in PATH..."
  run_test "ls" "$(ls)"
}

# Test handling programs not in PATH
test_handle_programs_not_in_path() {
  echo "Testing handling programs not in PATH..."
  run_test "nonexistent_program" "execv: No such file or directory"
}

# Test redirecting standard output
test_redirect_standard_output() {
  echo "Testing redirecting standard output..."
  run_test "echo Hello > /tmp/output.txt" ""
  actual_output=$(cat /tmp/output.txt)
  expected_output="Hello"
  if [ "$actual_output" = "$expected_output" ]; then
    echo -e "${GREEN}PASS${NC}: Test case 'Redirect standard output' succeeded."
  else
    echo -e "${RED}FAIL${NC}: Test case 'Redirect standard output' failed."
    echo "Expected output: $expected_output"
    echo "Actual output: $actual_output"
  fi
}

# Test case for piping once
test_pipe_once() {
  echo "Testing pipe once..."
  run_test "cat /etc/passwd | head -3" "$(cat /etc/passwd | head -3)"
}

# Test case for piping at deep depth
test_pipe_deep_depth() {
  echo "Testing pipe at deep depth..."
  run_test "ls | head -1 | tail -1" "$(ls | head -1 | tail -1)"
}

# Test case for comprehensive piping, will use the hw-list file~
test_comprehensive_piping() {
  echo "Testing comprehensive piping..."
  run_test "ls -l | grep '.c' | wc -l" "$(ls -l | grep '.c' | wc -l)"
}

test_background_process() {
  echo "Testing background processes..."
  echo "it will take up to 6s"
  run_test "./loop 4 &" "$(./loop 4 &)"
}

test_change_directory
test_start_programs_full_path
test_start_programs_with_arguments
test_start_programs_in_path
test_handle_programs_not_in_path
test_redirect_standard_output
test_pipe_once
test_pipe_deep_depth
test_comprehensive_piping
test_background_process


if [ "$fail_count" -eq "0" ]; then
  echo -e "\n${GREEN}Passed tests: $pass_count${NC}"
  echo "All tests passed."
else
  echo -e "${RED}Failed tests: $fail_count${NC}"
  exit $fail_count
fi
