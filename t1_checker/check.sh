#!/bin/bash

function init {
    total_score=0
    task7_score=0
    pad=$(printf '%.1s' "."{1..70})
    padlength=71

    TESTS_OUT=tests-out
    TESTS=tests
    NUM_TESTS=10

    # Cleanup the previous run's temporary files
    rm -rf ${TESTS_OUT}

	mkdir -p ${TESTS_OUT}/task1
	mkdir -p ${TESTS_OUT}/task2

    # Run tests
}

function print_result {
    printf "%s" "$1"
    printf "%*.*s" 0 $((padlength - ${#1} - ${#2} )) "$pad"
    printf "%s\n" "$2"
}

function check_homework {
    echo "..............................TASK $1..............................."

    mkdir -p ${TESTS_OUT}/$1

    for (( i = 0; i < $3; ++i))
    do
        in_file="${TESTS}/input/$1/$i.in"
        ref_file="${TESTS}/ref_output/$1/$i.bmp"
        out_file="${TESTS_OUT}/$1/$i.bmp"

        test_id=$i
        test_score=$2
        task_str=$1
        task_num=${task_str:4}

        # if task_num is between 1 and 6
        if [ $task_num -ge 1 -a $task_num -le 6 ]
        then
            make -f ./Makefile.checker -s check16
            ./check16 $task_num $out_file < ${in_file}
        # else
        else
            make -f ./Makefile -s interactive
            ./interactive < ${in_file}
        fi

        diff ${out_file} ${ref_file} > /dev/null
        if [ $? -eq 0 ]
        then
            let "total_score += $test_score"
            print_result "Test ${test_id}" "${test_score}/${test_score}p passed"

            if [ $task_num -eq 7 ]
            then
                let "task7_score += $test_score"
            fi
        else
            print_result "Test ${test_id}" "0/${test_score}p failed"
        fi
    done
}

function check_valgrind {
    echo "..............................TASK 8..............................."

    mkdir -p ${TESTS_OUT}/task7

    # if task7 score is less than 30, skip this test
    if [ $task7_score -lt 30 ]
    then
        print_result "Skipping Valgrind check - you must first have maximum score for task 7"
        return
    fi

    for (( i = $2; i < $3; ++i))
    do
        in_file="${TESTS}/input/task7/$i.in"

        test_id=$((i - $2))
        test_score=$1

        make -f ./Makefile -s interactive
        valgrind --tool=memcheck --leak-check=full --error-exitcode=1 ./interactive < ${in_file} > /dev/null 2>&1

        if [ $? -eq 0 ]
        then
            let "total_score += $test_score"
            print_result "Test ${test_id}" "${test_score}/${test_score}p passed"
        else
            print_result "Test ${test_id}" "0/${test_score}p failed"
        fi
    done
}

function check_readme {
    echo -e "\n..............................TASK README.............................."

    ls . | grep -i "readme" &>/dev/null
    if [ $? -eq 0 ]; then
        print_result "Test README" "$1/$1p passed"
        let "total_score += $1"
    else
        print_result "Test README" "0/$1p failed"
    fi
}

function check_style {
    echo -e "\n..............................CODING STYLE............................."
    is_style_ok=1
    echo -e "\nRun cpplint\n" > result
    cpplint --filter=-legal/copyright,-readability/casting,-build/include_subdir,-runtime/threadsafe_fn,-build/header_guard,-runtime/int \
        --linelength=120 $( find . -name \*.h -or -name \*.c | grep -v -e "check16.c" -e "main.c" -e "bmp.c" ) &>> result

    if [ ! $? -eq 0 ]; then
        is_style_ok=0
    fi

    echo -e "\nRun clang-tidy\n" >> result
    clang-tidy -checks='-*,cppcoreguidelines*' --warnings-as-errors='-*,cppcoreguidelines*' $( find . -name \*.h -or -name \*.c | grep -v -e "check16.c" -e "main.c" -e "bmp.c" ) -- &>> result

    if [ ! $? -eq 0 ]; then
        is_style_ok=0
    fi

    if [ $is_style_ok -eq 0 ]; then
        print_result "Test Coding Style" "-20/0p failed"
        let "total_score -= 20"
    else
        print_result "Test Coding Style" "0/0p passed"
    fi

    # Display run summary
    awk -f parse.awk result

    # Display detailed report
    echo -e "\n================================"
    echo -e "= Detailed coding style report ="
    echo -e "================================\n"
    cat result
    echo " "
    echo " "
}

function run_checker {
    init
	check_homework task1 1 5 # 1 pct, 5 tests
	check_homework task2 1 5 # 1 pct, 5 tests
	check_homework task3 1 5 # 1 pct, 5 tests
	check_homework task4 1 5 # 1 pct, 5 tests
	check_homework task5 1 5 # 1 pct, 5 tests
	check_homework task6 3 5 # 3 pct, 5 tests
	check_homework task7 2 15 # 3 pct, 15 tests
    check_valgrind 2 3 13 # 2 pct, 10 tests, skip first 3
    check_readme 10 # 10 pct
    check_style

    print_result "Final score" "${total_score}/100"
}

TIMEOUT_VAL=1

export -f run_checker init check_homework check_readme check_style print_result
#timeout ${TIMEOUT_VAL} run_checker
#timeout ${TIMEOUT_VAL} bash <<EOT
#run_checker
#EOT
run_checker

if [ ! $? -eq 0 ]
then
    echo "TIMEOUT. Tests exceeded maximum allowed time of $TIMEOUT_VAL"
fi

